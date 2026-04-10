#include "BmnTrigDigit.h"

BmnTrigDigit::BmnTrigDigit()
    : TNamed()
{
    fMod = -1;
    fTime = -1.0;
    fAmp = -1.0;
    fTimestamp = -1.0;
}

BmnTrigDigit::BmnTrigDigit(Short_t iMod, Double_t iTime, Double_t iAmp, Double_t iTimestamp)
    : TNamed()
{
    fMod = iMod;
    fTime = iTime;
    fAmp = iAmp;
    fTimestamp = iTimestamp;
}

BmnTrigDigit::~BmnTrigDigit() {}
