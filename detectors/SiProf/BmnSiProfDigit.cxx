#include "BmnSiProfDigit.h"

BmnSiProfDigit::BmnSiProfDigit()
    : BmnStripDigit()
{}

BmnSiProfDigit::BmnSiProfDigit(BmnSiProfDigit* digit)
    : BmnStripDigit(digit)
{}

BmnSiProfDigit::BmnSiProfDigit(Int_t iStation,
                               Int_t iModule,
                               Int_t iStripLayer,
                               Int_t iStripNumber,
                               Double_t iStripSignal,
                               Double_t iStripNoise)
    : BmnStripDigit(iStation, iModule, iStripLayer, iStripNumber, iStripSignal, iStripNoise)
{}

BmnSiProfDigit::~BmnSiProfDigit() {}