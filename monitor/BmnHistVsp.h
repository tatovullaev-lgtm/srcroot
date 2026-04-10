#pragma once
// C++
#include <exception>
#include <stdlib.h>
#include <vector>
// ROOT
#include <TCanvas.h>
#include <TChain.h>
#include <TClonesArray.h>
#include <TFile.h>
#include <TFolder.h>
#include <TH1D.h>
#include <TH1F.h>
#include <TH2F.h>
#include <THttpServer.h>
#include <TNamed.h>
#include <TString.h>
// BmnRoot
#include <BmnHist.h>

using std::vector;

class BmnHistVsp : public BmnHist
{
  public:
    BmnHistVsp(TString title, TString path = "", Int_t periodID = 9, BmnSetup setup = kBMNSETUP);
    virtual ~BmnHistVsp();
    void Reset();
    void Register(THttpServer* serv);
    void SetDir(TFile* outFile = NULL, TTree* recoTree = NULL);
    void DrawBoth();
    void FillFromDigi(DigiArrays* fDigiArrays);
    void ClearRefRun();
    BmnStatus SetRefRun(Int_t id);

  private:
    vector<TString> Names;
    vector<TString> NamesTimes;
    vector<vector<vector<TH1I*>>> histStripOccup;
    vector<vector<vector<TH1I*>>> histTimes;
    TCanvas* canStripOccup;
    TCanvas* canTimes;
    vector<PadInfo*> canStripPads;
    vector<PadInfo*> canTimesPads;
    UInt_t sumMods;
    UInt_t maxLayers;

    ClassDef(BmnHistVsp, 1)
};
