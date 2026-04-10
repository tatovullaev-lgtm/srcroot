/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * Author: Andrei Driuk
 *
 * Created on August 14, 2022
 */
#ifndef BMNHISTStat_H
#define BMNHISTStat_H 1

#include "BmnHist.h"

#include <TH1.h>
#include <vector>

using namespace std;

class BmnHistStat : public BmnHist
{
  public:
    BmnHistStat(TString title, TString path = "");
    virtual ~BmnHistStat();
    void Reset();
    void Register(THttpServer* serv);
    void SetDir(TFile* outFile = NULL, TTree* recoTree = NULL);
    void DrawBoth();
    void FillFromDigi(DigiArrays* fDigiArrays);
    void ClearRefRun();
    BmnStatus SetRefRun(Int_t id);

  private:
    void initHistDet();
    void initHistFD();
    void initHistBD();
    void initHistCorr();
    Int_t getPositionInArray(vector<TClonesArray*>* trigDigits, TString name);

    vector<TString> NamesStatDet;
    vector<TString> NamesStatFD;
    vector<TString> NamesStatBD;
    vector<TString> NamesStatCorr;

    const int32_t Max_ToF_Cnt = 301;
    const int32_t Max_GEM_Cnt = 5001;
    const int32_t Max_FSD_Cnt = 5001;
    const int32_t Max_CSC_Cnt = 2001;
    const int32_t Max_FHC_Cnt = 1001;

    const int32_t Max_BD_Cnt = 51;

    const int32_t Max_FD_Sig = 8000;

    TH1I* histGemStat;
    TH1I* histSiliconStat;
    TH1I* histTof400Stat;
    TH1I* histTof700Stat;
    TH1I* histCscStat;
    TH1I* histFHCalStat;
    //===FD vs Det======
    TH2F* histFDGemStat;
    TH2F* histFDSiliconStat;
    TH2F* histFDTof400Stat;
    TH2F* histFDTof700Stat;
    TH2F* histFDCscStat;
    TH2F* histFDFHCalStat;
    //===BD vs Det======
    TH2F* histBDGemStat;
    TH2F* histBDSiliconStat;
    TH2F* histBDTof400Stat;
    TH2F* histBDTof700Stat;
    TH2F* histBDCscStat;
    TH2F* histBDFHCalStat;

    //===Correlations=====
    TH2F* histGemSilicon;
    TH2F* histGemTof400;
    TH2F* histGemTof700;
    TH2F* histGemCsc;
    TH2F* histGemFHCal;
    TH2F* histCscFHCal;

    TCanvas* canStatDet;
    TCanvas* canStatFD;
    TCanvas* canStatBD;
    TCanvas* canStatCorr;
    vector<PadInfo*> canStatDetPads;
    vector<PadInfo*> canStatFDPads;
    vector<PadInfo*> canStatBDPads;
    vector<PadInfo*> canStatCorrPads;

    ClassDef(BmnHistStat, 1)
};

#endif /* BMNMONHISTS_H */
