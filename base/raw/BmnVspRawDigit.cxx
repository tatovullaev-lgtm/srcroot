#include "BmnVspRawDigit.h"

BmnVspRawDigit::BmnVspRawDigit(uint64_t TrgEvId, uint16_t DpbIdx, uint64_t TrgTimeFLES)
    : fTriggerFLES(TrgEvId)
    , fDpbIdx(DpbIdx)
    , fTrgTimeFLES(TrgTimeFLES)
    , fHitMissedEvts(0)
{}

BmnVspRawDigit::~BmnVspRawDigit() {}

void BmnVspRawDigit::AppendHit(uint64_t HitTime, uint16_t ElinkIdx, uint16_t Chan, uint16_t Adc)
{
    fVecHitTimeFLES.push_back(HitTime);
    fVecElinkIdx.push_back(ElinkIdx);
    fVecChan.push_back(Chan);
    fVecAdc.push_back(Adc);
}
