#include "BmnGemStripDigit.h"

BmnGemStripDigit::BmnGemStripDigit()
    : BmnStripDigit()
{}

BmnGemStripDigit::BmnGemStripDigit(BmnGemStripDigit* digit)
    : BmnStripDigit(digit)
{}

BmnGemStripDigit::BmnGemStripDigit(Int_t iStation,
                                   Int_t iModule,
                                   Int_t iStripLayer,
                                   Int_t iStripNumber,
                                   Double_t iStripSignal,
                                   Double_t iStripNoise)
    : BmnStripDigit(iStation, iModule, iStripLayer, iStripNumber, iStripSignal, iStripNoise)
{}

BmnGemStripDigit::~BmnGemStripDigit() {}
