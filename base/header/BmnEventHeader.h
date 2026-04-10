#ifndef BMNEVENTHEADER_H
#define BMNEVENTHEADER_H

#include <map>
#include <unordered_map>
#include <vector>
// ROOT
#include "TTimeStamp.h"
// FairRoot
#include "FairEventHeader.h"
// BmnRoot
#include "BmnEnums.h"
#include "BmnFunctionSet.h"
#include "BmnTrigInfo.h"
#include "BmnTrigUnion.h"

using SysClock = std::chrono::system_clock;
using SysPoint = std::chrono::time_point<SysClock>;
using std::map;
using std::unordered_map;
using std::vector;

class BmnEventHeader : public FairEventHeader
{
  private:
    /** Event Header branch name **/
    TString fHeaderName;   //!

    /** Event Id **/
    UInt_t fEventId;
    /** Period Id **/
    UInt_t fPeriodId;
    /** Event Time in TTimeStamp **/
    TTimeStamp fEventTimeTS;
    /** VSP Trigger Time in TTimeStamp **/
    TTimeStamp fEventVspTS;
    /** Event Type (payload = 0 or pedestal = 1) **/
    BmnEventType fEventType;
    /** Tripped Gems (1 bit for 1 GEM module) **/
    Bool_t fTripWord;
    /** start of new spill **/
    Bool_t fSpillStart;
    /** T0 information for current event**/
    Double_t fStartSignalTime;    // ns
    Double_t fStartSignalWidth;   // ns
    BmnTrigInfo fTrigInfo;
    //    UInt_t fTrigUnion;
    UInt_t fInputsAR;
    UInt_t fInputsBR;
    vector<uint32_t> fInputsVec;
    TTimeStamp fSpillStartTS;
    Int_t fSpillId;

    unordered_map<UInt_t, Long64_t> fTimeShift;

  public:
    /** Default constructor */
    BmnEventHeader();

    /** Constructor */
    BmnEventHeader(UInt_t run_id,
                   UInt_t event_id,
                   TTimeStamp event_time,
                   BmnEventType event_type,
                   Bool_t trip,
                   BmnTrigInfo& trig_info);

    /** Constructor */
    BmnEventHeader(UInt_t run_id,
                   UInt_t event_id,
                   TTimeStamp event_time,
                   BmnEventType event_type,
                   Bool_t trip,
                   BmnTrigInfo& trig_info,
                   unordered_map<UInt_t, Long64_t> time_shift);

    /** Destructor */
    virtual ~BmnEventHeader();

    virtual void Register(Bool_t Persistence = kTRUE)
    {
        //        FairRootManager::Instance()->Register(fHeaderName.Data(), "EvtHeader", this, Persistence);
    }

    void Clear();

    /** Get Event Header branch name */
    TString GetHeaderName() { return fHeaderName; }

    /** Get the event ID for this event */
    UInt_t GetEventId() { return fEventId; }

    /** Get the period ID for this event */
    UInt_t GetPeriodId() { return fPeriodId; }

    /** Get the time for this event */
    TTimeStamp GetEventTimeTS() { return fEventTimeTS; }
    SysPoint GetEventTimeTP() { return BmnFunctionSet::TimeStamp2TP(fEventTimeTS); }

    /** Get the time for this event */
    TTimeStamp GetVspTS() { return fEventVspTS; }

    /** Get the type of this event */
    BmnEventType GetEventType() { return fEventType; }

    /** Get the trip word for this event */
    Bool_t GetTripWord() { return fTripWord; }

    /** Get wether new spill is started */
    Bool_t GetSpillStart() { return fSpillStart; }

    Double_t GetStartSignalTime() { return fStartSignalTime; }
    Double_t GetStartSignalWidth() { return fStartSignalWidth; }

    /** Get the spill statistics */
    BmnTrigInfo& GetTrigInfo() { return fTrigInfo; }

    //    /** Get the trigger state */
    //    BmnTrigUnion GetTrigState()
    //    {
    //        BmnTrigUnion u;
    //        u.storage = fTrigUnion;
    //        return u;
    //    }

    //    UInt_t GetTrigStateWord() { return fTrigUnion; }

    /** Get Input signal states, after reduction */
    UInt_t GetInputSignalsAR() { return fInputsAR; }

    /** Get Input signal states, before reduction */
    UInt_t GetInputSignalsBR() { return fInputsBR; }

    /** Get Input signal vector, each word is a state of input trigger states in a TRC clock time slice */
    vector<uint32_t>& GetInputSignalsVector() { return fInputsVec; }

    unordered_map<UInt_t, Long64_t>& GetTimeShift() { return fTimeShift; }

    /** Set Event Header branch name */
    void SetHeaderName(TString header_name) { fHeaderName = header_name; }

    /** Set the event ID for this event
     * @param event_id : unique event id
     */
    void SetEventId(UInt_t event_id) { fEventId = event_id; }

    /** Set the period ID for this event
     * @param period_id : period id
     */
    void SetPeriodId(UInt_t period_id) { fPeriodId = period_id; }

    /** Set the time for this event in */
    void SetEventTimeTS(TTimeStamp event_time) { fEventTimeTS = event_time; }
    void SetEventTimeTP(SysPoint event_time) { fEventTimeTS = BmnFunctionSet::TimePoint2TS(event_time); }

    /** Set the time for this event in */
    void SetVspTS(TTimeStamp event_time) { fEventVspTS = event_time; }

    /** Set the type for this event
     * @param type : type (0 or 1)
     */
    void SetEventType(BmnEventType event_type) { fEventType = event_type; }

    void SetTripWord(Bool_t flag) { fTripWord = flag; }

    void SetSpillStart(Bool_t flag) { fSpillStart = flag; }

    void SetStartSignalInfo(Double_t time, Double_t width)
    {
        fStartSignalTime = time;
        fStartSignalWidth = width;
    }

    void SetTrigInfo(BmnTrigInfo& trig_info) { fTrigInfo.SetFrom(trig_info); }

    //    /** Set the trigger state */
    //    void SetTrigState(BmnTrigUnion& v) { fTrigUnion = v.storage; }

    /** Set the Input signal states, after reduction */
    void SetInputSignalsAR(UInt_t v) { fInputsAR = v; }

    /** Set the Input signal states, before reduction */
    void SetInputSignalsBR(UInt_t v) { fInputsBR = v; }

    /** Set the Input signals vector, each word is a state of input trigger states in a TRC clock time slice */
    void SetInputSignalsVector(vector<uint32_t> v) { fInputsVec = v; }

    void SetSpillStartTS(TTimeStamp ts) { fSpillStartTS = ts; }
    TTimeStamp GetSpillStartTS() { return fSpillStartTS; }
    Int_t& GetSpillId() { return fSpillId; }
    void SetSpillId(UInt_t sid) { fSpillId = sid; }

    void SetTimeShift(unordered_map<UInt_t, Long64_t> time_shift) { fTimeShift = time_shift; }

    ClassDef(BmnEventHeader, 5)
};

#endif /* BMNEVENTHEADER_H */
