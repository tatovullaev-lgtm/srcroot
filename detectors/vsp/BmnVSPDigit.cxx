#include "BmnVSPDigit.h"

BmnVSPDigit::BmnVSPDigit()
    : BmnStripDigit()
{}

BmnVSPDigit::BmnVSPDigit(BmnVSPDigit* digit)
    : BmnStripDigit(digit)
    , fTimeSinceBegin_ns(digit->GetTimeSinceBegin_ns())
{}

BmnVSPDigit::BmnVSPDigit(Int_t iStation, Int_t iModule, Int_t iLayer, Int_t iStrip, Double_t iSignal)
    : BmnStripDigit(iStation, iModule, iLayer, iStrip, iSignal)
{}

BmnVSPDigit::BmnVSPDigit(Int_t iStation, Int_t iModule, Int_t iLayer, Int_t iStrip, Double_t iSignal, Double_t iNoise)
    : BmnStripDigit(iStation, iModule, iLayer, iStrip, iSignal, iNoise)
{}

BmnVSPDigit::~BmnVSPDigit() {}
