#ifndef BMNHISTPROF_H
#define BMNHISTPROF_H
#include "BmnHist.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TClonesArray.h"
#include "TFile.h"
#include "TFolder.h"
#include "TH1D.h"
#include "TH1F.h"
#include "TH2F.h"
#include "THttpServer.h"
#include "TString.h"

#include <TNamed.h>
#include <exception>
#include <stdlib.h>
#include <vector>

using namespace std;

class BmnHistProf : public BmnHist
{
  public:
    BmnHistProf(TString title, TString path = "", Int_t periodID = 7, BmnSetup setup = kBMNSETUP);
    virtual ~BmnHistProf();
    void Reset();
    void Register(THttpServer* serv);
    void SetDir(TFile* outFile = NULL, TTree* recoTree = NULL);
    void DrawBoth();
    void FillFromDigi(DigiArrays* fDigiArrays);
    void ClearRefRun();
    BmnStatus SetRefRun(Int_t id);

  private:
    TCanvas* canStrip;
    vector<PadInfo*> canStripPads;
    //    BmnGemStripConfiguration::GEM_CONFIG gemStationConfig;
    //    BmnGemStripStationSet *gemStationSet;
    vector<TString> Names;
    vector<vector<vector<TH1F*>>> histSiliconStrip;
    UInt_t nStations;
    UInt_t nMods;
    UInt_t nLayers;
    UInt_t sumMods;
    UInt_t maxLayers;

    ClassDef(BmnHistProf, 1)
};

#endif /* BMNHISTPROF_H */
