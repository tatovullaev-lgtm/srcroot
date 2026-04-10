/*
 * File:   BmnHistTrigger.h
 * Author: ilnur
 *
 * Created on November 27, 2016, 4:18 PM
 */

#ifndef BMNHISTTRIGGER_H
#define BMNHISTTRIGGER_H 1
#include "BmnEventHeader.h"
#include "BmnHist.h"
#include "BmnRawDataDecoder.h"
#include "BmnTrigDigit.h"
#include "BmnTrigRaw2Digit.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TClonesArray.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH1F.h"
#include "TH2F.h"
#include "THttpServer.h"

#include <TNamed.h>
#include <algorithm>
#include <exception>
#include <memory>
#include <regex>
#include <stdlib.h>
#include <vector>
#define TRIG_TIME_WIN 6000
#define TRIG_TIME_BEAM 2000
#define TRIG_TIME_BINS 300
#define TRIG_AMP_BINS 100
#define TRIG_AMP_BINS_BD 25
#define SCALER_TIME_WIN 5.5
#define SCALER_TIME_BINS 1000
#define AMP_WIN 3000
#define TRIG_MULTIPL 40
#define BD_CHANNELS 40
#define SI_CHANNELS 64
#define ND_CHANNELS 4
#define TRIG_ROWS 3
#define TRIG_COLS 3
#define LIN_LOG_COLS 2

struct TrigStateDesc
{
    TString Name;
    Color_t Color;
};

class BmnHistTrigger : public BmnHist
{
  public:
    BmnHistTrigger(TString title = "Triggers", TString path = "", Int_t periodID = 9, BmnSetup = kBMNSETUP);
    virtual ~BmnHistTrigger();

    void Register(THttpServer* serv);
    void SetDir(TFile* outFile, TTree* recoTree);
    void FillFromDigi(DigiArrays* fDigiArrays);
    void Reset();
    void DrawBoth();
    void ClearRefRun();
    BmnStatus SetRefRun(Int_t id);

    void SetBDChannel(Int_t iSelChannel);

    Int_t GetBDChannel() { return fSelectedBDChannel; }

  private:
    void InitHistsFromArr(vector<TClonesArray*>* trigAr);
    void InitGrids();
    void InitTrigMasked();
    void InitScalers();
    void FillMSC(DigiArrays* fDigiArrays);
    void SetDir(TDirectory* Dir);
    const Int_t MaxTimeTDC = 6000;
    const Int_t MaxAmpTDC = 50;
    const Int_t MaxAmpTDC_BD = 25;
    const Int_t MaxTimeTQDC = 600;
    const Int_t MaxAmpTQDC = 15000;
    const Int_t MaxAmpTQDC_Corr = 10000;
    const Int_t MaxAmpTQDC_Beam = 35000;
    const Int_t MaxAmpTQDC_BC2 = 15000;
    const Int_t MaxAmpTQDC_Unit = 60000;
    const Int_t MaxAmpTQDC_x10 = 33000;
    const vector<string> ScalerNames{"BC1L_Busy", "pBT_Busy", "BT_Busy",       "NiT_Busy",
                                     "CCT1_Busy", "MBT_Busy", "BT&nBUSY_Busy", "CCT2_Busy"};
    const vector<TrigStateDesc> TrigState{{"BT", kCyan}, {"MBT", kGreen + 1}, {"nFD", kRed}};
    //    const vector<TrigStateDesc> TrigState{{"BT", kCyan}, {"MBT", kGreen + 1}, {"CCT1", kRed}, {"CCT2", kBlue}};
    vector<uint16_t> trig_state_bit_index;
    Int_t fRows = 0;
    Int_t fCols = 0;
    BmnTrigRaw2Digit* fTrigMapper;
    vector<TString> trigNames;
    vector<TString> shortNames;
    TH2I* histBDAmpByChannel;
    TH1I* histBDChannels;
    TH1I* histBDSimult;
    TH1I* histBDCircular;
    TH2I* histCorrBCBC;
    TH2I* histCorrBCVC;
    TH2F* hTDCTimes;
    TH2F* hTDCAmps;
    TH2F* hTQDCTimes;
    TH2F* hTQDCAmps;

    const vector<pair<string, string>> TrigNameCorrTQDC{{"BC1T", "BC1B"}, {"BC2T", "BC2B"}, {"VCT", "VCB"},
                                                        {"BC1", "BC2"},   {"VC", "BC2"},    {"FD", "BC2"}};
    vector<double> storeAmpTQDC;                    // store the TQDC amp in each event to use for correlation
    vector<pair<uint32_t, uint32_t>> idxCorrTQDC;   // indices in the store amp vector
    vector<TH2I*> histCorrTQDC;

    TCanvas* can2d;
    TCanvas* canProfile;
    TCanvas* canTimes;
    TCanvas* canAmp;
    bool canInitDone;
    UInt_t indexBD;
    unique_ptr<TCanvas> canGridBD;
    unique_ptr<TCanvas> canGridND;
    unique_ptr<TCanvas> canTrigStates;
    unique_ptr<TCanvas> canCorrTQDC;
    unique_ptr<TCanvas> canScalers;
    map<string, UInt_t> mapArrayNames;
    map<UInt_t, PadInfo*> mapGridND;
    map<UInt_t, PadInfo*> mapAmpLin;
    map<UInt_t, PadInfo*> mapAmpLog;
    map<UInt_t, PadInfo*> mapTBLin;
    map<UInt_t, PadInfo*> mapTBLog;
    map<UInt_t, TH1*> mapTime;
    vector<TH1*> histsGridBD;
    vector<TH1*> hists;
    vector<vector<TH1*>> histsAux;
    vector<PadInfo*> padsGridBD;
    vector<PadInfo*> padsGridND;
    vector<PadInfo*> padsTrigStates;
    vector<PadInfo*> padsCorrTQDC;
    vector<PadInfo*> padsScalers;
    vector<PadInfo*> canProfilePads;
    vector<PadInfo*> canTimesPads;
    vector<PadInfo*> can2dPads;
    vector<PadInfo*> padsCanAmp;
    Int_t fSelectedBDChannel;
    // scaler auxillary data
    vector<pair<uint16_t, PadInfo*>> fill_scaler_vec_profile;
    vector<pair<uint16_t, PadInfo*>> fill_scaler_vec_spill;
    Int_t fLastSpillId;
    TTimeStamp fLastSpillStartTS;

    ClassDef(BmnHistTrigger, 2)
};

#endif /* BMNHISTTRIGGER_H */
