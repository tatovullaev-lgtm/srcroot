#ifndef DSTEVENTHEADER_H
#define DSTEVENTHEADER_H

#include "BmnEnums.h"

#include "FairEventHeader.h"
#include "FairRootManager.h"

#include "TTimeStamp.h"
#include <vector>
using namespace std;


class DstEventHeader : public FairEventHeader {
   private:
    /** Event Header branch name **/
    TString fHeaderName;  //!

    /** Event Identifier **/
    UInt_t fEventId;
    /** Event Time in TTimeStamp **/
    TTimeStamp fEventTimeTS;
    /** Trigger Type (beam = 6 or target = 1) **/
    BmnTriggerType fTriggerType;
    /** Impact parameter [fm] **/
    Double_t fB;
    /** Total charge before and after the target **/
    Int_t ftrigger;
    Double_t fBC1_12, fBC2_12, fBC3_12, fBC4_12, fBC5_12, fBC3_12_X10, fBC4_12_X10, fBC5_12_X10, fBC3_S, fBC4_S, fBC5_S;          
    vector <bool> fB10, fB11;
    vector <Double_t>  fPq, fPq_MDF, fPq_MDF1, fPq_MDF2, fPq_MDF3, fTx_MDF;
    Short_t fZinBC12, fZoutBC34_12, fZoutBC35_12, fZoutBC45_12, fZoutBC34_S, fZoutBC35_S, fZoutBC45_S, fVeto, fDCH_Mult, fMWPC_Mult;
    
    // Info from Scint. Wall
    vector <Double_t> fScWallELoss; 
    vector <UInt_t> fScWallCellId; 
    vector <UInt_t> fScWallFlag;
    vector <Int_t> fScWallTrackId;
    UInt_t fScWallMult;
    UInt_t fScWallNMatchTracks;
    vector <UInt_t> fScWallOrderId;
    
    // Info from Veto-Box
    UInt_t fVBMult;
    vector <UInt_t> fVBCellId;
    
    

   public:
    /** Default constructor */
    DstEventHeader();

    /** Constructor with parameters */
    DstEventHeader(UInt_t run_id, UInt_t event_id, TTimeStamp event_time, BmnTriggerType trigger_type, Double_t b);

    /** Destructor */
    virtual ~DstEventHeader();

    virtual void Register(Bool_t Persistence = kTRUE) {
        FairRootManager::Instance()->Register(fHeaderName.Data(), "EvtHeader", this, Persistence);
    }

    /** Get Event Header branch name */
    TString GetHeaderName() { return fHeaderName; }

    /** Get run ID for this run */
    UInt_t GetEventId() { return fEventId; }

    /** Get time for this event */
    TTimeStamp GetEventTimeTS() { return fEventTimeTS; }

    /** Get type of this event */
    BmnTriggerType GetTriggerType() { return fTriggerType; }

    /** Get impact parameter - b, fm */
    Double_t GetB() { return fB; }

Double_t f;
    /** Get total charge before and after the target */
    vector <bool> GetB10() { return fB10; }
    vector <bool> GetB11() { return fB11; }
    Int_t GetTriger() { return ftrigger; }
    Double_t GetBC1_12() { return fBC1_12; }
    Double_t GetBC2_12() { return fBC2_12; }
    Double_t GetBC3_12() { return fBC3_12; }
    Double_t GetBC4_12() { return fBC4_12; }
    Double_t GetBC5_12() { return fBC5_12; }
    Double_t GetBC3_12_X10() { return fBC3_12_X10; }
    Double_t GetBC4_12_X10() { return fBC4_12_X10; }
    Double_t GetBC5_12_X10() { return fBC5_12_X10; }
    Double_t GetBC3_S() { return fBC3_S; }
    Double_t GetBC4_S() { return fBC4_S; }
    Double_t GetBC5_S() { return fBC5_S; }
    vector <Double_t> GetPq() { return fPq; }
    vector <Double_t> GetPq_MDF() { return fPq_MDF; }
    vector <Double_t> GetPq_MDF1() { return fPq_MDF1; }
    vector <Double_t> GetPq_MDF2() { return fPq_MDF2; }
    vector <Double_t> GetPq_MDF3() { return fPq_MDF3; }
    vector <Double_t> GetTx_MDF() { return fTx_MDF; }
    Short_t GetZinBC12() { return fZinBC12; }
    Short_t GetZoutBC34_12() { return fZoutBC34_12; }
    Short_t GetZoutBC35_12() { return fZoutBC35_12; }
    Short_t GetZoutBC45_12() { return fZoutBC45_12; }
    Short_t GetZoutBC34_S() { return fZoutBC34_S; }
    Short_t GetZoutBC35_S() { return fZoutBC35_S; }
    Short_t GetZoutBC45_S() { return fZoutBC45_S; }
    Short_t GetVeto() { return fVeto; }
    Short_t GetDCH_Mult() { return fDCH_Mult; }
    Short_t GetMWPC_Mult() { return fMWPC_Mult; }
    
    // Info from Scint. Wall
    vector <Double_t> GetScWallELoss() { return fScWallELoss; }
    vector <UInt_t> GetScWallCellId() { return fScWallCellId; }
    vector <UInt_t> GetScWallFlag() { return fScWallFlag; }
    vector <Int_t> GetScWallTrackId() { return fScWallTrackId; }
    UInt_t GetScWallMult() { return fScWallMult; }
    UInt_t GetScWallNMatchTracks() { return fScWallNMatchTracks; }
    vector <UInt_t> GetScWallOrderId() { return fScWallOrderId; }
    
    // Info from Veto-Box
    UInt_t GetVBMult() { return fVBMult; }
    vector <UInt_t> GetVBCellId() { return fVBCellId; }
    
  
    //Short_t GetZin() { return fZin; }
    /** Set Event Header branch name */
    void SetHeaderName(TString header_name) { fHeaderName = header_name; }

    /** Set event ID for this run */
    void SetEventId(UInt_t event_id) { fEventId = event_id; }

    /** Set time for this event in */
    void SetEventTimeTS(TTimeStamp event_time) { fEventTimeTS = event_time; }

    /** Set trigger type for this event
     * @param trigger_type : trigger type (beam = 6 or target = 1)
     */
    void SetTriggerType(BmnTriggerType trigger_type) { fTriggerType = trigger_type; }

    /** Set impact parameter - b, fm */
    void SetB(Double_t b) { fB = b; }


    /** Set total charge before and after the target */
     
    void SetB10(vector <bool> B10) { fB10 = B10; }
    void SetB11(vector <bool> B11) { fB11 = B11; }
    void SetTriger(Int_t trigger) { ftrigger = trigger; }
    void SetBC1_12(Double_t BC1_12) { fBC1_12 = BC1_12; }
    void SetBC2_12(Double_t BC2_12) { fBC2_12 = BC2_12; }
    void SetBC3_12(Double_t BC3_12) { fBC3_12 = BC3_12; }
    void SetBC4_12(Double_t BC4_12) { fBC4_12 = BC4_12; }
    void SetBC5_12(Double_t BC5_12) { fBC5_12 = BC5_12; }
    void SetBC3_12_X10(Double_t BC3_12_X10) { fBC3_12_X10 = BC3_12_X10; }
    void SetBC4_12_X10(Double_t BC4_12_X10) { fBC4_12_X10 = BC4_12_X10; }
    void SetBC5_12_X10(Double_t BC5_12_X10) { fBC5_12_X10 = BC5_12_X10; }
    void SetBC3_S(Double_t BC3_S) { fBC3_S = BC3_S; }
    void SetBC4_S(Double_t BC4_S) { fBC4_S = BC4_S; }
    void SetBC5_S(Double_t BC5_S) { fBC5_S = BC5_S; }
    void SetPq(vector <Double_t> Pq) { fPq = Pq; }
    void SetPq_MDF(vector <Double_t> Pq_MDF) { fPq_MDF = Pq_MDF; }
    void SetPq_MDF1(vector <Double_t> Pq_MDF1) { fPq_MDF1 = Pq_MDF1; }
    void SetPq_MDF2(vector <Double_t> Pq_MDF2) { fPq_MDF2 = Pq_MDF2; }
    void SetPq_MDF3(vector <Double_t> Pq_MDF3) { fPq_MDF3 = Pq_MDF3; }
    void SetTx_MDF(vector <Double_t> Tx_MDF) { fTx_MDF = Tx_MDF; }
    void SetZinBC12(Short_t ZinBC12) { fZinBC12 = ZinBC12; }
    void SetZoutBC34_12(Short_t ZoutBC34_12) { fZoutBC34_12 = ZoutBC34_12; }
    void SetZoutBC35_12(Short_t ZoutBC35_12) { fZoutBC35_12 = ZoutBC35_12; }
    void SetZoutBC45_12(Short_t ZoutBC45_12) { fZoutBC45_12 = ZoutBC45_12; }
    void SetZoutBC34_S(Short_t ZoutBC34_S) { fZoutBC34_S = ZoutBC34_S; }
    void SetZoutBC35_S(Short_t ZoutBC35_S) { fZoutBC35_S = ZoutBC35_S; }
    void SetZoutBC45_S(Short_t ZoutBC45_S) { fZoutBC45_S = ZoutBC45_S; }
    void SetVeto(Short_t Veto) { fVeto = Veto; }
    void SetDCH_Mult(Short_t DCH_Mult) { fDCH_Mult = DCH_Mult; }
    void SetMWPC_Mult(Short_t MWPC_Mult) { fMWPC_Mult = MWPC_Mult; }
    
    // Info from Scint. Wall
    void SetScWallELoss(vector <Double_t> ScWallELoss) { fScWallELoss = ScWallELoss; }
    void SetScWallCellId(vector <UInt_t> ScWallCellId) { fScWallCellId = ScWallCellId; }
    void SetScWallFlag(vector <UInt_t> ScWallFlag) { fScWallFlag = ScWallFlag; }
    void SetScWallTrackId(vector <Int_t> ScWallTrackId) { fScWallTrackId = ScWallTrackId; }
    void SetScWallMult(UInt_t ScWallMult) { fScWallMult = ScWallMult; }
    void SetScWallNMatchTracks(UInt_t ScWallNMatchTracks) { fScWallNMatchTracks = ScWallNMatchTracks; }
    void SetScWallOrderId(vector <UInt_t> ScWallOrderId) { fScWallOrderId = ScWallOrderId; }
    
    // Info from Veto-Box
    void SetVBMult(UInt_t VBMult) { fVBMult = VBMult; }
    void SetVBCellId(vector <UInt_t> VBCellId) { fVBCellId = VBCellId; }
  
    void CopyFrom(DstEventHeader * hdr);
    ClassDef(DstEventHeader, 2)
};

#endif /* DstEventHeader_H */
