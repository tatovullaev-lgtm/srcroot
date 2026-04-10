#ifndef BMNHISTGEM_H
#define BMNHISTGEM_H 1
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

using std::vector;

class BmnHistGem : public BmnHist
{
  public:
    BmnHistGem(TString title, TString path = "", Int_t periodID = 7, BmnSetup setup = kBMNSETUP);
    virtual ~BmnHistGem();
    void Reset();
    void Register(THttpServer* serv);
    void SetDir(TFile* outFile = NULL, TTree* recoTree = NULL);
    void DrawBoth();
    void FillFromDigi(DigiArrays* fDigiArrays);
    void ClearRefRun();
    BmnStatus SetRefRun(Int_t id);

  private:
    vector<TString> Names;
    vector<vector<vector<TH1I*>>> histGemStrip;
    TCanvas* canGemStrip;
    //    vector<PadInfo<TH1>*> canGemStripPads;
    vector<PadInfo*> canStripPads;
    UInt_t sumMods;
    UInt_t maxLayers;

    ClassDef(BmnHistGem, 1)
};

#endif /* BMNMONHISTS_H */
