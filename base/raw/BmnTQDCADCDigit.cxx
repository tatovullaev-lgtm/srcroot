#include "BmnTQDCADCDigit.h"

BmnTQDCADCDigit::BmnTQDCADCDigit() :  BmnADCDigit() {
    fTrigTimestamp = 0;
    fAdcTimestamp = 0;
}

BmnTQDCADCDigit::BmnTQDCADCDigit(
        UInt_t iSerial, UInt_t iChannel, UChar_t iSlot, UInt_t n, Short_t *iValue, UShort_t TrigTS, UShort_t AdcTS) :
BmnADCDigit(iSerial, iChannel, n, iValue)
{
    fTrigTimestamp = TrigTS;
    fAdcTimestamp = AdcTS;
    fSlot = iSlot;
}

BmnTQDCADCDigit::~BmnTQDCADCDigit() {
}
