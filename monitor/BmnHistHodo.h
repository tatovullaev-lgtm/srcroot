#ifndef BMNHISTHodo_H
#define BMNHISTHodo_H 1

#include "BmnHist.h"
#include "BmnHodoDigi.h"
#include "BmnHodoRaw2Digit.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TClonesArray.h"
#include "TColor.h"
#include "TFile.h"
#include "TFolder.h"
#include "TH1F.h"
#include "TH2F.h"
#include "THttpServer.h"
#include "TString.h"

#include <TNamed.h>
#define Hodo_ROWS 2
#define Hodo_COLS 2
#define coefHodo 1

class BmnHistHodo : public BmnHist
{
  public:
    BmnHistHodo(TString title = "Hodo", TString path = "");
    virtual ~BmnHistHodo();
    void CreateHistos();
    void Reset();
    void Register(THttpServer* serv);
    void SetDir(TFile* outFile, TTree* recoTree);
    void DrawBoth();
    void ClearRefRun();
    void FillFromDigi(DigiArrays* fDigiArrays);
    BmnStatus SetRefRun(Int_t id);
    void SetSelection(Int_t Strip, Int_t Side, Int_t Gain, Int_t Trigger);

  private:
    static constexpr int kNstrips = 24;   // Hodo 2022. Strips numerated from 1 to 16 inclusive
    static constexpr int kNsides = 2;     // Hodo 2022. Strips have UP and DOWN readout sides
    static constexpr int kNgains = 2;     // Hodo 2022. Strips have HIGH and LOW gain readout channels

    long int fEventCounter = 1;
    Int_t fSelectedStrip;
    Int_t fSelectedSide;
    Int_t fSelectedGain;
    Int_t fSelectedTrigger;

    TH2F* h2d_EstripUH;     // amplitude in strips. Side UP gain HIGH
    TH2F* h2d_EstripDH;     // amplitude in strips. Side DOWN gain HIGH
    TH2F* h2d_EstripUL;     // amplitude in strips. Side UP gain LOW
    TH2F* h2d_EstripDL;     // amplitude in strips. Side DOWN gain LOW
    TH1F* h1d_profileUH;    // profile amplitude in strips. Side UP gain HIGH
    TH1F* h1d_profileDH;    // profile amplitude in strips. Side DOWN gain HIGH
    TH1F* h1d_profileUL;    // profile amplitude in strips. Side UP gain LOW
    TH1F* h1d_profileDL;    // profile amplitude in strips. Side DOWN gain LOW
    TH1F* hStripSideGain;   // amplitude in selected StripSideGain
    TH1F* wfmStrip;         // waveform in selected StripSideGain
    TH1F* hZ2_hodo;         // Z2
    std::map<uint32_t, TH1*> vecProj{};

    TCanvas* canAmps;
    TCanvas* canProf;
    TCanvas* canZ2;
    TCanvas* canv_proj_DH;
    TCanvas* canv_proj_DL;
    TCanvas* canv_proj_UH;
    TCanvas* canv_proj_UL;
    TCanvas* canSelect;
    vector<PadInfo*> canAmpsPads;
    vector<PadInfo*> canProfPads;
    vector<PadInfo*> canZ2Pads;
    std::map<uint32_t, PadInfo*> canv_proj_Pads{};
    vector<PadInfo*> canSelectPads;
    vector<TString> NamesAmps;
    vector<TString> NamesProf;
    vector<TString> NamesZ2;
    std::map<uint32_t, TString> Namescanv_proj{};
    vector<TString> NamesSelect;

    ClassDef(BmnHistHodo, 1)
};

#endif /* BMNHISTHodo_H */
