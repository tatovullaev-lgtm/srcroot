#ifndef BmnHgndCalibration_H
#define BmnHgndCalibration_H

#include <boost/math/special_functions/lambert_w.hpp>
#include <cmath>

namespace BmnHgndCalibration
{

struct TdcFromTot
{
    double offset, p, tau, RC;
    double scale;

    TdcFromTot(double offset_, double p_, double tau_, double RC_)
        : offset(offset_)
        , p(p_)
        , tau(tau_)
        , RC(RC_)
    {
        const double e = std::exp(1.0);
        const double Tp = (2 * e - 1) * p * tau * RC / (e - 1) / (RC * (1 - p) - tau);
        scale = Tp / 2. / RC * std::sqrt((e - 1) / p) * std::exp(Tp / 2. / RC);
    }

    double operator()(double tot) const
    {
        if (!std::isfinite(scale))
            return offset;
        double arg = scale * std::exp(-tot / 2. / RC);
        double tdc = offset + 2 * RC * boost::math::lambert_w0(arg);
        return std::isfinite(tdc) ? tdc : offset;
    }
};

struct TdcFromTotApprox
{
    double offset, p, tau, RC;
    double scale;

    TdcFromTotApprox(double offset_, double p_, double tau_, double RC_)
        : offset(offset_)
        , p(p_)
        , tau(tau_)
        , RC(RC_)
    {
        const double e = std::exp(1.0);
        const double Tp = (2 * e - 1) * p * tau * RC / (e - 1) / (RC * (1 - p) - tau);
        scale = Tp * std::sqrt((e - 1) / p) * std::exp(Tp / 2. / RC);
    }

    double operator()(double tot) const
    {
        if (!std::isfinite(scale))
            return offset;
        double tdc = offset + scale * std::exp(-tot / 2. / RC);
        return std::isfinite(tdc) ? tdc : offset;
    }
};

struct QdcFromTot
{
    double p, tau, RC;
    double theta;   // threshold in mV
    double Rload;   // load in Ohm
    double Tp, scale;
    TdcFromTot tdcRaw;   // TDC with zero offset

    QdcFromTot(double p_, double tau_, double RC_, double theta_ = 20.0, double Rload_ = 33.0)
        : p(p_)
        , tau(tau_)
        , RC(RC_)
        , theta(theta_)
        , Rload(Rload_)
        , tdcRaw(0.0, p_, tau_, RC_)   // zero-offset TDC for internal computation
    {
        const double e = std::exp(1.0);
        Tp = (2 * e - 1) * p * tau * RC / (e - 1) / (RC * (1 - p) - tau);
        double como = p * Tp / 2. / (e - 1);
        scale = theta / Rload * (RC - (1 - p) * tau + como);
    }

    double operator()(double tot) const
    {
        if (!std::isfinite(scale))
            return 0.0;

        // Reuse TDC logic here
        const double tdc = tdcRaw(tot);
        const double qdc = scale * std::exp((tot + tdc - Tp) / RC);
        return std::isfinite(qdc) ? qdc : 0.0;
    }
};

}   // namespace BmnHgndCalibration

#endif   // BmnHgndCalibration_H