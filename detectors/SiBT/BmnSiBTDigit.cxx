#include "BmnSiBTDigit.h"

BmnSiBTDigit::BmnSiBTDigit()
    : BmnStripDigit()
{}

BmnSiBTDigit::BmnSiBTDigit(BmnSiBTDigit* digit)
    : BmnStripDigit(digit)
{}

BmnSiBTDigit::BmnSiBTDigit(Int_t iStation,
                           Int_t iModule,
                           Int_t iStripLayer,
                           Int_t iStripNumber,
                           Double_t iStripSignal,
                           Double_t iStripNoise)
    : BmnStripDigit(iStation, iModule, iStripLayer, iStripNumber, iStripSignal, iStripNoise)
{}

BmnSiBTDigit::~BmnSiBTDigit() {}
