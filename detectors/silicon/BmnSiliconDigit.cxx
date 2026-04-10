#include "BmnSiliconDigit.h"

BmnSiliconDigit::BmnSiliconDigit()
    : BmnStripDigit()
{}

BmnSiliconDigit::BmnSiliconDigit(BmnSiliconDigit* digit)
    : BmnStripDigit(digit)
{}

BmnSiliconDigit::BmnSiliconDigit(Int_t iStation,
                                 Int_t iModule,
                                 Int_t iLayer,
                                 Int_t iStrip,
                                 Double_t iSignal,
                                 Double_t iStripNoise)
    : BmnStripDigit(iStation, iModule, iLayer, iStrip, iSignal, iStripNoise)
{}

BmnSiliconDigit::~BmnSiliconDigit() {}
