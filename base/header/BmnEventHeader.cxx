#include "BmnEventHeader.h"

// -----   Default constructor   -------------------------------------------
BmnEventHeader::BmnEventHeader()
    : FairEventHeader()
    , fHeaderName("BmnEventHeader.")
    , fEventId(0)
    , fPeriodId(0)
    , fEventTimeTS(0, 0)
    , fEventVspTS(0, 0)
    , fEventType(kBMNPAYLOAD)
    , fTripWord(kFALSE)
    , fSpillStart(kFALSE)
    , fStartSignalTime(0)
    , fStartSignalWidth(0)
    , fInputsAR(0)
    , fInputsBR(0)
    , fSpillStartTS(0, 0)
    , fSpillId(-1)
{}

BmnEventHeader::BmnEventHeader(UInt_t run_id,
                               UInt_t event_id,
                               TTimeStamp event_time,
                               BmnEventType event_type,
                               Bool_t trip,
                               BmnTrigInfo& trig_info)
    : FairEventHeader()
    , fHeaderName("BmnEventHeader.")
    , fEventId(event_id)
    , fEventTimeTS(event_time)
    , fEventVspTS(0, 0)
    , fEventType(event_type)
    , fTripWord(trip)
    , fSpillStart(kFALSE)
    , fStartSignalTime(0)
    , fStartSignalWidth(0)
    , fTrigInfo(trig_info)
    , fInputsAR(0)
    , fInputsBR(0)
    , fSpillStartTS(0, 0)
    , fSpillId(-1)
{
    SetRunId(run_id);
    SetEventTime(fEventTimeTS.AsDouble());
}

BmnEventHeader::BmnEventHeader(UInt_t run_id,
                               UInt_t event_id,
                               TTimeStamp event_time,
                               BmnEventType event_type,
                               Bool_t trip,
                               BmnTrigInfo& trig_info,
                               unordered_map<UInt_t, Long64_t> time_shift)
    : FairEventHeader()
    , fHeaderName("BmnEventHeader.")
    , fEventId(event_id)
    , fEventTimeTS(event_time)
    , fEventVspTS(0, 0)
    , fEventType(event_type)
    , fTripWord(trip)
    , fSpillStart(kFALSE)
    , fStartSignalTime(0)
    , fStartSignalWidth(0)
    , fTrigInfo(trig_info)
    , fInputsAR(0)
    , fInputsBR(0)
    , fSpillStartTS(0, 0)
    , fSpillId(-1)
    , fTimeShift(time_shift)
{
    SetRunId(run_id);
    SetEventTime(event_time.AsDouble());
}

// -----   Destructor   ----------------------------------------------------
BmnEventHeader::~BmnEventHeader()
{
    // if (fTrigInfo) delete fTrigInfo;
}

void BmnEventHeader::Clear()
{
    fHeaderName = "";
    fEventId = 0;
    fPeriodId = 0;
    fEventTimeTS = TTimeStamp(0, 0);
    fEventVspTS = TTimeStamp(0, 0);
    fEventType = (BmnEventType)0;
    fTripWord = kFALSE;
    fSpillStart = kFALSE;
    fStartSignalTime = 0.0;
    fStartSignalWidth = 0.0;
    fInputsAR = 0;
    fInputsBR = 0;
    fInputsVec.clear();
    fSpillStartTS = TTimeStamp(0, 0);
    fSpillId = -1;
    fTimeShift.clear();
}
