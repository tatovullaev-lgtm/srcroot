#ifndef BMNMSCDIGIT_H
#define BMNMSCDIGIT_H

#include <array>
#include <chrono>
// ROOT
#include "TObject.h"
// BmnRoot
#include "BmnFunctionSet.h"
#include "RawTypes.h"

using SysClock = std::chrono::system_clock;
using SysPoint = std::chrono::time_point<SysClock>;

template<typename IntType = UChar_t>
class BmnMSCDigit : public TObject
{
  public:
    /** Constructor
     * \param[in] iSerial : crate serial
     * \param[in] iSlot : crate slot
     * \param[in] iValue : counters array ptr
     * \param[in] evId : spill's last eventID in case of EndOfSpill data,
     *  or eventID in case of normal spill data
     * \param[in] iTime : timestamp for normal spill data
     */
    BmnMSCDigit(UInt_t iSerial = 0,
                UInt_t iSlot = 0,
                std::array<IntType, MSC_N_COUNTERS> iValue = {},
                UInt_t evId = 0,
                SysPoint time = SysPoint::min(),
                UChar_t ext_cond = 0)
        : fSerial(iSerial)
        , fSlot(iSlot)
        , fValue(iValue)
        //    , fTime(time)
        , fTS(BmnFunctionSet::TimePoint2TS(time))
        , fEventId(evId)
        , fExtCond(ext_cond)
    {
        //    for (UInt_t i = 0; i < NVals; ++i)
        //        fValue[i] = iValue[i];
    }

    /** Constructor
     * \param[in] iSerial : crate serial
     * \param[in] iSlot : crate slot
     * \param[in] evId : spill's last eventID in case of EndOfSpill data,
     *  or eventID in case of normal spill data
     * \param[in] iTime : timestamp for normal spill data
     */
    BmnMSCDigit(UInt_t iSerial, UInt_t iSlot, UInt_t evId, SysPoint time = SysPoint::min())
        : fSerial(iSerial)
        , fSlot(iSlot)
        , fTS(BmnFunctionSet::TimePoint2TS(time))
        , fEventId(evId)
        , fExtCond(0)
    {}

    UInt_t GetSerial() const { return fSerial; }

    UInt_t GetSlot() const { return fSlot; }

    std::array<IntType, MSC_N_COUNTERS>& GetValue() { return fValue; }

    SysPoint GetTime() const { return BmnFunctionSet::TimeStamp2TP(fTS); }

    TTimeStamp GetTS() const { return fTS; }

    UInt_t GetLastEventId() const { return fEventId; }

    bool IsSpill() { return (fExtCond & MSC_EC_SPILL); }

    bool IsBusy() { return (fExtCond & MSC_EC_BUSY); }

    constexpr static const UInt_t GetNVals() { return NVals; }
    /** Destructor **/
    virtual ~BmnMSCDigit() {}

  private:
    static const UInt_t NVals = MSC_N_COUNTERS;
    UInt_t fSerial;
    UInt_t fSlot;
    std::array<IntType, NVals> fValue;
    //    SysPoint fTime;
    TTimeStamp fTS;
    UInt_t fEventId;
    UChar_t fExtCond;

    ClassDef(BmnMSCDigit, 5);
};

#endif /* BMNMSCDIGIT_H */
