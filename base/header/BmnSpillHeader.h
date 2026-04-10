#ifndef BMNSPILLHEADER_H
#define BMNSPILLHEADER_H
// STL
#include <chrono>
#include <map>
#include <unordered_map>
//  ROOT
#include <TTimeStamp.h>
// bmnroot
#include "BmnAliases.h"
#include "BmnEnums.h"
#include "BmnFunctionSet.h"
#include "RawTypes.h"

using SysClock = std::chrono::system_clock;
using SysPoint = std::chrono::time_point<SysClock>;
using std::array;
using std::string;
using std::vector;

typedef std::map<string, vector<std::pair<SysPoint, UInt_t>>> ScalerNameSliceCntMap;

class BmnSpillHeader : public TObject
{
  private:
    /** Event Id **/
    UInt_t fEventIdStart;
    /** Event Id **/
    UInt_t fEventIdEnd;
    /** Event count **/
    UInt_t fEventCnt;
    /** Period Id **/
    UInt_t fPeriodId;
    /** Spill Id **/
    UInt_t fSpillId;
    SysPoint fStartTS;
    SysPoint fStopTS;
    //    ScalerNameSliceCntMap fScalers;   // name -> count for each time slice
    MapString2Cnt fScalersTotal;   // name -> total spill MSC count
    vector<ULong64_t> fScalersVec;
    MapString2Cnt fEventTrigCount;   // name -> total spill trigger (TRC bit inside events)

    //    Name2SerCh fScalersMap;
    array<UInt_t, kT0_BIN_BLOCK_WORDS> fRawT0Cntrs;

    UInt_t fBeamTrigger;
    UInt_t fBTnBusy;
    UInt_t fL0;
    UInt_t fTrigProtection;
    UInt_t fBC1;
    UInt_t fBC2;
    UInt_t fBC3;
    UInt_t fAccepted;
    UInt_t fCand;
    UInt_t fBP;
    UInt_t fAP;
    UInt_t fRj;
    UInt_t fAll;
    UInt_t fAvail;
    UInt_t fBC1H;    ///< BC1 (high threshold)
    UInt_t fBC1BP;   ///< BC1 (before protection)
    UInt_t fBC1xBC2;
    UInt_t fBC1nBusy;
    UInt_t fIntTrig;
    UInt_t fSRCTrig;
    UInt_t fTrignBusy;   ///< Trigger * !Busy

  public:
    /** Default constructor */
    BmnSpillHeader();

    /** Constructor */
    BmnSpillHeader(UInt_t run_id, UInt_t event_id, TTimeStamp event_time);

    /** Destructor */
    virtual ~BmnSpillHeader();

    void Clear();

    /** Get the the last event ID for the spill */
    UInt_t GetLastEventId() { return fEventIdEnd; }
    /** Get the the first event ID for the spill */
    UInt_t GetFirstEventId() { return fEventIdStart; }
    /** Get the event count in the spill */
    UInt_t GetNEvents() { return fEventCnt; }

    /** Get the period ID */
    UInt_t GetPeriodId() { return fPeriodId; }

    /** Get the spill ID */
    UInt_t GetSpillId() { return fSpillId; }

    //    ScalerNameSliceCntMap& ScalersVec() { return fScalers; }

    vector<ULong64_t>& ScalersVec() { return fScalersVec; }

    MapString2Cnt& Scalers() { return fScalersTotal; }

    MapString2Cnt& EventTriggerCnt() { return fEventTrigCount; }

    array<UInt_t, kT0_BIN_BLOCK_WORDS>& RawT0Counters() { return fRawT0Cntrs; }

    /** Get the start TS */
    SysPoint GetStartTime() { return fStartTS; }

    /** Get the stop TS */
    SysPoint GetStopTime() { return fStopTS; }

    /** Get the start TS */
    TTimeStamp GetStartTS() { return BmnFunctionSet::TimePoint2TS(fStartTS); }

    /** Get the stop TS */
    TTimeStamp GetStopTS() { return BmnFunctionSet::TimePoint2TS(fStopTS); }

    void SetCand(UInt_t _v) { fCand = _v; }

    UInt_t GetCand() { return fCand; }

    void SetAccepted(UInt_t _v) { fAccepted = _v; }

    UInt_t GetAccepted() { return fAccepted; }

    void SetBefo(UInt_t _v) { fBP = _v; }

    UInt_t GetBefo() { return fBP; }

    void SetAfter(UInt_t _v) { fAP = _v; }

    UInt_t GetAfter() { return fAP; }

    void SetRjct(UInt_t _v) { fRj = _v; }

    UInt_t GetRjct() { return fRj; }

    void SetAll(UInt_t _v) { fAll = _v; }

    UInt_t GetAll() { return fAll; }

    void SetAvail(UInt_t _v) { fAvail = _v; }

    UInt_t GetAvail() { return fAvail; }

    void SetBT(UInt_t _v) { fBeamTrigger = _v; }

    UInt_t GetBT() { return fBeamTrigger; }

    void SetBTnBusy(UInt_t _v) { fBTnBusy = _v; }

    UInt_t GetBTnBusy() { return fBTnBusy; }

    void SetL0(UInt_t _v) { fL0 = _v; }

    UInt_t GetL0() { return fL0; }

    void SetProt(UInt_t _v) { fTrigProtection = _v; }

    UInt_t GetProt() { return fTrigProtection; }

    void SetBC1(UInt_t _v) { fBC1 = _v; }

    UInt_t GetBC1() { return fBC1; }

    void SetBC2(UInt_t _v) { fBC2 = _v; }

    UInt_t GetBC2() { return fBC2; }

    void SetBC3(UInt_t _v) { fBC3 = _v; }

    UInt_t GetBC3() { return fBC3; }
    // SRC trig counters
    void SetBC1H(UInt_t _v) { fBC1H = _v; }

    UInt_t GetBC1H() { return fBC1H; }

    void SetBC1BP(UInt_t _v) { fBC1BP = _v; }

    UInt_t GetBC1BP() { return fBC1BP; }

    void SetBC1xBC2(UInt_t _v) { fBC1xBC2 = _v; }

    UInt_t GetBC1xBC2() { return fBC1xBC2; }

    void SetBC1nBusy(UInt_t _v) { fBC1nBusy = _v; }

    UInt_t GetBC1nBusy() { return fBC1nBusy; }

    void SetIntTrig(UInt_t _v) { fIntTrig = _v; }

    UInt_t GetIntTrig() { return fIntTrig; }

    void SetSRCTrig(UInt_t _v) { fSRCTrig = _v; }

    UInt_t GetSRCTrig() { return fSRCTrig; }

    void SetTrignBusy(UInt_t _v) { fTrignBusy = _v; }

    UInt_t GetTrignBusy() { return fTrignBusy; }

    /** Set the first event ID for the spill
     * @param evid : unique event id
     */
    void SetFirstEventId(UInt_t event_id) { fEventIdStart = event_id; }

    /** Set the last event ID for the spill
     * @param evid : unique event id
     */
    void SetLastEventId(UInt_t event_id) { fEventIdEnd = event_id; }

    /** Set the event count in the spill */
    void SetNEvents(UInt_t events) { fEventCnt = events; }

    /** Set the period ID
     * @param evid : unique period id
     */
    void SetPeriodId(UInt_t period_id) { fPeriodId = period_id; }

    /** Set the spill ID
     * @param spill : spill id
     */
    void SetSpillId(UInt_t spill) { fSpillId = spill; }

    /** Set the time for this event in */
    void SetStartTime(SysPoint time) { fStartTS = time; }

    /** Set the time for this event in */
    void SetStopTime(SysPoint time) { fStopTS = time; }

    /** Set the time for this event in */
    void SetStartTS(TTimeStamp ts) { fStartTS = BmnFunctionSet::TimeStamp2TP(ts); }

    /** Set the time for this event in */
    void SetStopTS(TTimeStamp ts) { fStopTS = BmnFunctionSet::TimeStamp2TP(ts); }

    ClassDef(BmnSpillHeader, 10)
};

#endif /* BMNSPILLHEADER_H */
