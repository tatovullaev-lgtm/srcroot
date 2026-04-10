#ifndef WfmProcessor_H
#define WfmProcessor_H

#include "BmnDigiContainerTemplate.h"
#include "FairLogger.h"
#include "PronyFitter.h"

#include <iostream>
#include <numeric>

class WfmProcessor
{

  public:
    WfmProcessor()
        : fdigiPars{false, 0, 0, 0.0f, 0, false, false, {}}
        , fSignalLength(0)
        , fAZik(nullptr)
    {}

    virtual ~WfmProcessor();

  protected:
    struct digiPars
    {
        bool isWriteWfm;
        int gateBegin;
        int gateEnd;
        float threshold;
        int signalType;
        bool doInvert;

        bool isfit;
        std::vector<std::complex<float>> harmonics;
    } fdigiPars;

    void ProcessWfm(std::vector<float>& wfm, BmnDigiContainerTemplate* digi);
    int fSignalLength;
    std::complex<float>** fAZik;   // Inverse Harmo matrix for fit

  private:
    void MeanRMScalc(std::vector<float> wfm, float* Mean, float* RMS, int begin, int end, int step = 1);
    ClassDef(WfmProcessor, 1);
};
#endif /* WfmProcessor_H */
