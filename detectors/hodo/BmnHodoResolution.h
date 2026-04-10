#ifndef BmnHodoResolution_H
#define BmnHodoResolution_H

#include "TRandom.h"

#include <cmath>

namespace BmnHodoResolution
{

struct Kernel
{
    double stochastic, constant;

    Kernel()
        : stochastic(0)
        , constant(0)
    {}
    Kernel(double stochastic_, double constant_)
        : stochastic(stochastic_)
        , constant(constant_)
    {}

    double sigma(double z2) const { return sqrt(stochastic * stochastic * z2 + constant * constant * z2 * z2); }

    double operator()(double z2) const { return gRandom->Gaus(z2, sigma(z2)); }
};

}   // namespace BmnHodoResolution

#endif   // BmnHodoResolution_H
