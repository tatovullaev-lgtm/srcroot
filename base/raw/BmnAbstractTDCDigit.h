#ifndef BMNABSTRACTTDCDIGIT_H
#define BMNABSTRACTTDCDIGIT_H

#include "TObject.h"

class BmnAbstractTDCDigit : public TObject
{
  public:
    BmnAbstractTDCDigit() = default;
    BmnAbstractTDCDigit(uint32_t serial,
                        uint32_t channel,
                        int64_t timeSec,
                        int64_t timeNsec,
                        int16_t toa,
                        uint16_t tot);

    virtual ~BmnAbstractTDCDigit() = default;

    uint32_t GetSerial() const { return fSerial; }
    uint32_t GetChannel() const { return fChannel; }
    int64_t GetTimeSec() const { return fTimeSec; }
    int64_t GetTimeNsec() const { return fTimeNsec; }
    int16_t GetToa() const { return fToa; }
    uint16_t GetTot() const { return fTot; }

  private:
    uint32_t fSerial = 0;
    uint32_t fChannel = 0;
    int64_t fTimeSec = 0;
    int64_t fTimeNsec = 0;
    int16_t fToa = 0;
    uint16_t fTot = 0;

    ClassDef(BmnAbstractTDCDigit, 1);
};

#endif   // BMNABSTRACTTDCDIGIT_H
