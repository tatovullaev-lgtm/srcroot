#include "WfmProcessor.h"

void WfmProcessor::ProcessWfm(std::vector<float>& wfm, BmnDigiContainerTemplate* digi)
{
    assert(fdigiPars.gateBegin > 0 && fdigiPars.gateEnd > 0);
    if (fdigiPars.gateBegin >= (int)wfm.size()) {
        LOG(error) << "WfmProcessor : Filling " << digi->GetClassName() << ". waveform too short: accessing "
                   << fdigiPars.gateBegin << "/" << wfm.size() << ". Check calibration file ";
        fdigiPars.gateBegin = wfm.size() - 1;
    }
    if (fdigiPars.gateEnd >= (int)wfm.size()) {
        LOG(error) << "WfmProcessor : Filling " << digi->GetClassName() << ". waveform too short: accessing "
                   << fdigiPars.gateEnd << "/" << wfm.size() << ". Check calibration file ";
        fdigiPars.gateEnd = wfm.size() - 1;
    }

    // Invert
    if (fdigiPars.doInvert) {
        LOG(debug4) << "WfmProcessor : Filling " << digi->GetClassName() << ". Inverting";
        float myconstant{-1.0};
        std::transform(wfm.begin(), wfm.end(), wfm.begin(),
                       std::bind(std::multiplies<float>(), myconstant, std::placeholders::_1));
    }

    // Zero level calculation
    LOG(debug4) << "WfmProcessor : Filling " << digi->GetClassName() << ". ZL calc";
    float ZL = std::accumulate(wfm.begin(), wfm.begin() + fdigiPars.gateBegin, 0.0) / fdigiPars.gateBegin;
    std::transform(wfm.begin(), wfm.end(), wfm.begin(), [ZL](float value) { return value - ZL; });
    digi->fZL = (int)ZL;

    // MAX and Integral calculation including borders. ZL removed
    LOG(debug4) << "WfmProcessor : Filling " << digi->GetClassName() << ". MAX & INT search";
    digi->fIntegral = (int)std::accumulate(wfm.begin() + fdigiPars.gateBegin, wfm.begin() + fdigiPars.gateEnd + 1, 0.0);
    auto const max_iter = std::max_element(wfm.begin() + fdigiPars.gateBegin, wfm.begin() + fdigiPars.gateEnd + 1);
    digi->fAmpl = (int)*max_iter;
    digi->fTimeMax = (int)std::distance(wfm.begin(), max_iter);
    if (fdigiPars.isWriteWfm)
        digi->fWfm = wfm;

    // Prony fitting procedure
    if (fdigiPars.isfit) {
        LOG(debug4) << "WfmProcessor : Filling " << digi->GetClassName() << ". Fitting";
        PsdSignalFitting::PronyFitter Pfitter;
        Pfitter.Initialize(fdigiPars.harmonics.size(), fdigiPars.harmonics.size(), fdigiPars.gateBegin,
                           fdigiPars.gateEnd);
        Pfitter.SetDebugMode(0);
        Pfitter.SetWaveform(wfm, 0.0);
        // Pfitter.ResetAmplitudes();
        int SignalBeg = Pfitter.CalcSignalBeginStraight();
        if (SignalBeg < 0)
            return;
        if (static_cast<std::size_t>(SignalBeg) > wfm.size())
            return;
        if (static_cast<std::size_t>(SignalBeg + fSignalLength) > wfm.size())
            SignalBeg = fdigiPars.gateBegin;
        Pfitter.SetExternalHarmonics(fdigiPars.harmonics);
        Pfitter.SetSignalBegin(SignalBeg);
        Pfitter.CalculateFitAmplitudesFast(fSignalLength, fAZik);

        digi->fFitIntegral = Pfitter.GetIntegral(fdigiPars.gateBegin, fdigiPars.gateEnd);
        digi->fFitAmpl = Pfitter.GetMaxAmplitude() - Pfitter.GetZeroLevel();
        float fit_R2 = Pfitter.GetRSquare(fdigiPars.gateBegin, fdigiPars.gateEnd);
        digi->fFitR2 = (fit_R2 > 2.0) ? 2.0 : fit_R2;
        digi->fFitZL = Pfitter.GetZeroLevel();
        digi->fFitTimeMax = Pfitter.GetSignalMaxTime();

        if (fdigiPars.isWriteWfm)
            digi->fFitWfm = Pfitter.GetFitWfm();
    }
}

void WfmProcessor::MeanRMScalc(std::vector<float> wfm, float* Mean, float* RMS, int begin, int end, int step)
{
    begin = (begin < 0) ? 0 : begin;
    if (begin > end) {
        float swap = end;
        end = begin;
        begin = swap;
    };
    step = TMath::Abs(step);
    *Mean = *RMS = 0.;
    int Delta = 0;
    for (int n = begin; n <= end; n += step) {
        *Mean += wfm[n];
        Delta++;
    }
    *Mean /= (float)Delta;
    for (int n = begin; n <= end; n += step)
        *RMS += (wfm[n] - *Mean) * (wfm[n] - *Mean);
    *RMS = sqrt(*RMS / ((float)Delta));
    // printf("AMPL %.2f, RMS %.2f\n",*Mean,*RMS);
}

WfmProcessor::~WfmProcessor()
{
    if (fdigiPars.isfit && fAZik) {
        int model_order = fdigiPars.harmonics.size() + 1;
        for (int i = 0; i < model_order; i++)
            delete[] fAZik[i];
        delete[] fAZik;
    }
}
