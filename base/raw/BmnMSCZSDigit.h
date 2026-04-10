#pragma once

#include <chrono>
#include <vector>
// ROOT
#include "TObject.h"
// BmnRoot
#include "BmnFunctionSet.h"
#include "RawTypes.h"

using SysClock = std::chrono::system_clock;
using SysPoint = std::chrono::time_point<SysClock>;

/**
 * Raw MSC block with zero suppression and for one specific channel
 */
template<typename IntType = UChar_t>
class BmnMSCZSDigit : public TObject
{
  public:
    /** Constructor
     * \param[in] iSerial : crate serial
     * \param[in] size : value count
     * \param[in] iTime : timestamp for normal spill data
     */
    BmnMSCZSDigit(uint32_t serial = 0, uint8_t channel = 0, SysPoint time = SysPoint::min(), size_t size = 0)
        : fSerial(serial)
        , fChannel(channel)
        , fTS(BmnFunctionSet::TimePoint2TS(time))
        , fSize(size)
    //    , fTime(time)
    {
        fValue.resize(fSize);
        fTimes.resize(fSize);
        fExtCond.resize(fSize);
    }

    uint32_t GetSerial() const { return fSerial; }

    uint8_t GetChannel() const { return fChannel; }

    std::vector<IntType>& GetValues() { return fValue; }

    std::vector<SysPoint>& GetTimes() { return fTimes; }

    std::vector<uint8_t>& GetExtCond() { return fExtCond; }

    SysPoint GetTime() const { return BmnFunctionSet::TimeStamp2TP(fTS); }

    TTimeStamp GetTS() const { return fTS; }

    //    bool IsSpill() { return (fExtCond & MSC_EC_SPILL); }
    //
    //    bool IsBusy() { return (fExtCond & MSC_EC_BUSY); }

    size_t GetNVals() { return fSize; }
    /** Destructor **/
    virtual ~BmnMSCZSDigit() {}

  private:
    uint32_t fSerial;
    uint8_t fChannel;
    TTimeStamp fTS;
    size_t fSize;
    std::vector<IntType> fValue;
    std::vector<SysPoint> fTimes;
    std::vector<uint8_t> fExtCond;
    //    SysPoint fTime;

    ClassDef(BmnMSCZSDigit, 1);
};
