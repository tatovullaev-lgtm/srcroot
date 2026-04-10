#include "BmnSpillHeader.h"

// -----   Default constructor   -------------------------------------------
BmnSpillHeader::BmnSpillHeader()
    : fEventIdEnd(0)
    , fEventCnt(0)
    , fPeriodId(0)
    , fSpillId(0)
    , fStartTS{SysPoint::min()}
    , fStopTS{SysPoint::min()}
    , fRawT0Cntrs{0}
    , fBeamTrigger(0)
    , fBTnBusy(0)
    , fL0(0)
    , fTrigProtection(0)
    , fBC1(0)
    , fBC2(0)
    , fBC3(0)
    , fAccepted(0)
    , fCand(0)
    , fBP(0)
    , fAP(0)
    , fRj(0)
    , fAll(0)
    , fAvail(0)
    , fBC1H(0)
    , fBC1BP(0)
    , fBC1xBC2(0)
    , fBC1nBusy(0)
    , fIntTrig(0)
    , fSRCTrig(0)
    , fTrignBusy(0)
{
    printf("SH %p created\n", this);
}

BmnSpillHeader::BmnSpillHeader(UInt_t run_id, UInt_t event_id, TTimeStamp event_time)
    : fEventIdEnd(event_id)
    , fEventCnt(0)
    , fPeriodId(run_id)
    , fSpillId(0)
    , fStartTS{SysPoint::min()}
    , fStopTS{SysPoint::min()}
    , fRawT0Cntrs{0}
    , fBeamTrigger(0)
    , fBTnBusy(0)
    , fL0(0)
    , fTrigProtection(0)
    , fBC1(0)
    , fBC2(0)
    , fBC3(0)
    , fAccepted(0)
    , fCand(0)
    , fBP(0)
    , fAP(0)
    , fRj(0)
    , fAll(0)
    , fAvail(0)
    , fBC1H(0)
    , fBC1BP(0)
    , fBC1xBC2(0)
    , fBC1nBusy(0)
    , fIntTrig(0)
    , fSRCTrig(0)
    , fTrignBusy(0)
{}

// -----   Destructor   ----------------------------------------------------
BmnSpillHeader::~BmnSpillHeader()
{
    printf("SH %p destroyed\n", this);
}

void BmnSpillHeader::Clear()
{
    fEventIdEnd = 0;
    fEventCnt = 0;
    fPeriodId = 0;
    fSpillId = 0;
    fStartTS = SysPoint::min();
    fStopTS = SysPoint::min();
    //    fScalers.clear();
    fScalersTotal.clear();
    fScalersVec.clear();
    fEventTrigCount.clear();
    fRawT0Cntrs.fill(0);
    fBeamTrigger = 0;
    fBTnBusy = 0;
    fL0 = 0;
    fTrigProtection = 0;
    fBC1 = 0;
    fBC2 = 0;
    fBC3 = 0;
    fAccepted = 0;
    fCand = 0;
    fBP = 0;
    fAP = 0;
    fRj = 0;
    fAll = 0;
    fAvail = 0;
    fBC1H = 0;
    fBC1BP = 0;
    fBC1xBC2 = 0;
    fBC1nBusy = 0;
    fIntTrig = 0;
    fSRCTrig = 0;
    fTrignBusy = 0;
}
