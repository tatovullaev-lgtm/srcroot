#include "DstEventHeader.h"

// -----   Default constructor   -------------------------------------------
DstEventHeader::DstEventHeader() : FairEventHeader(),
                                   fHeaderName("DstEventHeader."),
                                   fEventId(0),
                                   fEventTimeTS(TTimeStamp()),
                                   fTriggerType(kBMNMINBIAS),
                                   fB(0),
                                   ftrigger(-1),
                                   fBC1_12(-100.0), 
                                   fBC2_12(-100.0), 
                                   fBC3_12(-100.0), 
                                   fBC4_12(-100.0), 
                                   fBC5_12(-100.0),
                                   fBC1_cor(-100.0), //cor
                                   fBC2_cor(-100.0), 
                                   fBC3_cor(-100.0), 
                                   fBC4_cor(-100.0), 
                                   fBC5_cor(-100.0),  
                                   fBC3_12_X10(-100.0), 
                                   fBC4_12_X10(-100.0), 
                                   fBC5_12_X10(-100.0),
                                   fBC3_S(-100.0), 
                                   fBC4_S(-100.0), 
                                   fBC5_S(-100.0),
                                   fZinBC12(-100),
                                   fZoutBC34_12(-100), 
                                   fZoutBC35_12(-100), 
                                   fZoutBC45_12(-100),
                                   fZinBC12_cor(-100),  //cor
                                   fZoutBC34_12_cor(-100), 
                                   fZoutBC35_12_cor(-100), 
                                   fZoutBC45_12_cor(-100), 
                                   fZoutBC34_S(-100), 
                                   fZoutBC35_S(-100), 
                                   fZoutBC45_S(-100),
                                   fVeto(-1),
                                   fPq(0,0),
                                   fPq_MDF(0,0),
                                   fPq_MDF1(0,0),
                                   fPq_MDF2(0,0),
                                   fPq_MDF3(0,0),
                                   fTx_MDF(0,0),
                                   fB10(0,0),
                                   fB11(0,0),
                                   fDCH_Mult(-1),
                                   fMWPC_Mult(-1),
                                   fScWallELoss(0,0),
                                   fScWallCellId(0,0),
                                   fScWallFlag(0,0),
                                   fScWallTrackId(0,0),
                                   fScWallMult(0),
                                   fScWallNMatchTracks(0),
                                   fScWallOrderId(0,0),
                                   fVBCellId(0,0),
                                   fVBMult(0), 
                                   fBC1_Time0(-100),
                                   fBC1_Time1(-100),
                                   fBC2_Time0(-100),
                                   fBC2_Time1(-100),
                                   fTofCal_Plane(0,0),
                                   fTofCal_Bar(0,0),
                                   fTofCal_Time0(0,0),
                                   fTofCal_Time1(0,0),
                                   fTofCal_Amp0(0,0),
                                   fTofCal_Amp1(0,0),
                                   fTofCal_X(0,0),
                                   fTofCal_Y(0,0),
                                   fTofCal_nHits(-100),
                                   fTofCal_Arm(0,0),
                                   fTofCal_GlobalBar(0,0),
                                   fIs_laser(-1)
                                   {}

// -----   Constructor with parameters   -----------------------------------
DstEventHeader::DstEventHeader(UInt_t run_id, UInt_t event_id, TTimeStamp event_time, BmnTriggerType trigger_type, Double_t b) : FairEventHeader(),
                                                                                                                                 fHeaderName("DstEventHeader."),
                                                                                                                                 fEventId(event_id),
                                                                                                                                 fEventTimeTS(event_time),
                                                                                                                                 fTriggerType(trigger_type),
                                                                                                                                 fB(b),
                                                                                                                                 ftrigger(-1),
                                                                                                                                 fBC1_12(-100.0), 
                                                                                                                                 fBC2_12(-100.0), 
                                                                                                                                 fBC3_12(-100.0), 
                                                                                                                                 fBC4_12(-100.0), 
                                                                                                                                 fBC5_12(-100.0),
                                                                                                                                 fBC1_cor(-100.0),     //cor 
                                                                                                                                 fBC2_cor(-100.0), 
                                                                                                                                 fBC3_cor(-100.0), 
                                                                                                                                 fBC4_cor(-100.0), 
                                                                                                                                 fBC5_cor(-100.0),
                                                                                                                                 fBC3_12_X10(-100.0), 
                                                                                                                                 fBC4_12_X10(-100.0), 
                                                                                                                                 fBC5_12_X10(-100.0), 
                                                                                                                                 fBC3_S(-100.0), 
                                                                                                                                 fBC4_S(-100.0), 
                                                                                                                                 fBC5_S(-100.0),
                                                                                                                                 fZinBC12(-100),
                                                                                                                                 fZoutBC34_12(-100), 
                                                                                                                                 fZoutBC35_12(-100), 
                                                                                                                                 fZoutBC45_12(-100),
                                                                                                                                 fZinBC12_cor(-100),    //cor
                                                                                                                                 fZoutBC34_12_cor(-100), 
                                                                                                                                 fZoutBC35_12_cor(-100), 
                                                                                                                                 fZoutBC45_12_cor(-100), 
                                                                                                                                 fZoutBC34_S(-100), 
                                                                                                                                 fZoutBC35_S(-100), 
                                                                                                                                 fZoutBC45_S(-100),
                                                                                                                                 fVeto(-1),
                                                                                                                                 fPq(0,0),
                                                                                                                                 fPq_MDF(0,0),
                                                                                                                                 fPq_MDF1(0,0),
                                                                                                                                 fPq_MDF2(0,0),
                                                                                                                                 fPq_MDF3(0,0),
                                                                                                                                 fTx_MDF(0,0),
                                                                                                                                 fB10(0,0),
                                                                                                                                 fB11(0,0),
                                                                                                                                 fDCH_Mult(-1),
                                                                                                                                 fMWPC_Mult(-1),
                                                                                                                                 fScWallELoss(0,0),
                                                                                                                                 fScWallCellId(0,0),
                                                                                                                                 fScWallFlag(0,0),
                                                                                                                                 fScWallTrackId(0,0),
                                                                                                                                 fScWallMult(0),
                                                                                                                                 fScWallNMatchTracks(0),
                                                                                                                                 fScWallOrderId(0,0),
                                                                                                                                 fVBCellId(0,0),
                                                                                                                                 fVBMult(0),
                                                                                                                                 fBC1_Time0(-100),
                                                                                                                                 fBC1_Time1(-100),
                                                                                                                                 fBC2_Time0(-100),
                                                                                                                                 fBC2_Time1(-100),
                                                                                                                                 fTofCal_Plane(0,0),
                                                                                                                                 fTofCal_Bar(0,0),
                                                                                                                                 fTofCal_Time0(0,0),
                                                                                                                                 fTofCal_Time1(0,0),
                                                                                                                                 fTofCal_Amp0(0,0),
                                                                                                                                 fTofCal_Amp1(0,0),
                                                                                                                                 fTofCal_X(0,0),
                                                                                                                                 fTofCal_Y(0,0),
                                                                                                                                 fTofCal_nHits(-100),
                                                                                                                                 fTofCal_Arm(0,0),
                                                                                                                                 fTofCal_GlobalBar(0,0),
                                                                                                                                 fIs_laser(-1) {                                                                                                          
    SetRunId(run_id);
    SetEventTime(event_time.AsDouble());
}

// -----   Destructor   ----------------------------------------------------
DstEventHeader::~DstEventHeader() {}

void DstEventHeader::CopyFrom(DstEventHeader * hdr) {
    fHeaderName = hdr->fHeaderName;
    fEventId = hdr->fEventId;
    fEventTimeTS = hdr->fEventTimeTS;
    fTriggerType = hdr->fTriggerType;
    fB = hdr->fB;
    ftrigger=hdr->ftrigger;
    fBC1_12 = hdr->fBC1_12;
    fBC2_12 = hdr->fBC2_12;
    fBC3_12 = hdr->fBC3_12;
    fBC4_12 = hdr->fBC4_12;
    fBC5_12 = hdr->fBC5_12;
    fBC1_cor = hdr->fBC1_cor;   //cor
    fBC2_cor = hdr->fBC2_cor;
    fBC3_cor = hdr->fBC3_cor;
    fBC4_cor = hdr->fBC4_cor;
    fBC5_cor = hdr->fBC5_cor;
    fBC3_12_X10 = hdr->fBC3_12_X10;
    fBC4_12_X10 = hdr->fBC4_12_X10;
    fBC5_12_X10 = hdr->fBC5_12_X10;
    fBC3_S = hdr->fBC3_S;
    fBC4_S = hdr->fBC4_S;
    fBC5_S = hdr->fBC5_S;
    fZinBC12 = hdr->fZinBC12;
    fZoutBC34_12 = hdr->fZoutBC34_12;
    fZoutBC35_12 = hdr->fZoutBC35_12;
    fZoutBC45_12 = hdr->fZoutBC45_12;
    fZinBC12_cor = hdr->fZinBC12_cor;   //cor
    fZoutBC34_12_cor = hdr->fZoutBC34_12_cor;
    fZoutBC35_12_cor = hdr->fZoutBC35_12_cor;
    fZoutBC45_12_cor = hdr->fZoutBC45_12_cor;
    fZoutBC34_S = hdr->fZoutBC34_S;
    fZoutBC35_S = hdr->fZoutBC35_S;
    fZoutBC45_S = hdr->fZoutBC45_S;
    fVeto = hdr->fVeto;
    fPq = hdr->fPq;
    fPq_MDF = hdr->fPq_MDF;
    fPq_MDF1 = hdr->fPq_MDF1;
    fPq_MDF2 = hdr->fPq_MDF2;
    fPq_MDF3 = hdr->fPq_MDF3;
    fTx_MDF = hdr->fTx_MDF;
    fB10 = hdr->fB10;
    fB11 = hdr->fB11;
    fDCH_Mult = hdr->fDCH_Mult;
    fMWPC_Mult = hdr->fMWPC_Mult;
    fScWallELoss = hdr->fScWallELoss;
    fScWallCellId = hdr->fScWallCellId;
    fScWallFlag = hdr->fScWallFlag;
    fScWallTrackId = hdr->fScWallTrackId;
    fScWallMult = hdr->fScWallMult;
    fScWallNMatchTracks = hdr->fScWallNMatchTracks;
    fScWallOrderId = hdr->fScWallOrderId;
    fVBCellId = hdr->fVBCellId;
    fVBMult = hdr->fVBMult;
    fBC1_Time0 = hdr->fBC1_Time0;
    fBC1_Time1 = hdr->fBC1_Time1;
    fBC2_Time0 = hdr->fBC2_Time0;
    fBC2_Time1 = hdr->fBC2_Time1;
    fTofCal_Plane = hdr->fTofCal_Plane;
    fTofCal_Bar = hdr->fTofCal_Bar;
    fTofCal_Time0 = hdr->fTofCal_Time0;
    fTofCal_Time1 = hdr->fTofCal_Time1;
    fTofCal_Amp0 = hdr->fTofCal_Amp0;
    fTofCal_Amp1 = hdr->fTofCal_Amp1;
    fTofCal_X = hdr->fTofCal_X;
    fTofCal_Y = hdr->fTofCal_Y;
    fTofCal_nHits = hdr->fTofCal_nHits;
    fTofCal_Arm = hdr->fTofCal_Arm;
    fTofCal_GlobalBar = hdr->fTofCal_GlobalBar;
    fIs_laser = hdr->fIs_laser;
    
}

ClassImp(DstEventHeader)
