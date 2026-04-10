#include "BmnAbstractTDCDigit.h"

BmnAbstractTDCDigit::BmnAbstractTDCDigit(uint32_t serial,
                                         uint32_t channel,
                                         int64_t timeSec,
                                         int64_t timeNsec,
                                         int16_t toa,
                                         uint16_t tot)
    : fSerial(serial)
    , fChannel(channel)
    , fTimeSec(timeSec)
    , fTimeNsec(timeNsec)
    , fToa(toa)
    , fTot(tot)
{}
