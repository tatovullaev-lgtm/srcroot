#ifndef BMNHISTFHCal_H
#define BMNHISTFHCal_H 1

#include "BmnFHCalDigi.h"
#include "BmnFHCalGeo.h"
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
#define Hodo_ROWS 2
#define Hodo_COLS 2
#define coefHodo 1

#include <TNamed.h>
#define FHCal_ROWS 1
#define FHCal_COLS 2

class BmnHistFHCal : public BmnHist
{
  public:
    BmnHistFHCal(TString title = "FHCal", TString path = "", Int_t periodID = 8, BmnSetup setup = kBMNSETUP);
    virtual ~BmnHistFHCal();
    void CreateHistos();
    void Reset();
    void Register(THttpServer* serv);
    void SetDir(TFile* outFile, TTree* recoTree);
    void DrawBoth();
    void ClearRefRun();
    void FillFromDigi(DigiArrays* fDigiArrays);
    BmnStatus SetRefRun(Int_t id);
    void SetSelection(Int_t Module, Int_t Section, Int_t Trigger);
    void SetGeometry(const std::string& path);

  private:
    TClonesArray* Events;
    static constexpr int kTOTAL_MODULES = 54;   // FHCAL 2022. Modules numerated from 1 to 54 inclusive

    static constexpr int kNstrips = 16;   // Hodo 2022. Strips numerated from 1 to 16 inclusive
    static constexpr int kNsides = 2;     // Hodo 2022. Strips have UP and DOWN readout sides
    static constexpr int kNgains = 2;     // Hodo 2022. Strips have HIGH and LOW gain readout channels

    long int fEventCounter = 1;
    long int fEventCounterZL = 1;
    BmnFHCalGeo* fGeoHandler;   // stores map with detector address to 3D position and error
    Int_t fSelectedModule;
    Int_t fSelectedSection;
    Int_t fSelectedTrigger;

    TH1F* h1d_Emodules;            // energy in modules. 1D
    TH1F* hSecE;                   // amplitude in selected section
    TH1F* wfmSec;                  // waveform in selected section
    TH2F* h2_calo;                 // energy in modules. 2D X-Y
    TH2F* h2_calo_text;            // modules indexes. 2D X-Y
    TH2F* h2_mod_sec;              // energy in modules sections. 2D sec-mod
    TH2F* h2_ZL_nEvent;            // fZl vs Event number. 2D fZl-nEvent
    TH1F* h1_totalE;               // total energy in FHCal. 1D
    TH2F* h2_totalE_hodoZ2_BT;     // total energy in FHCal vs Hodo Z^2, BT. 2D
    TH2F* h2_totalE_hodoZ2_CCT2;   // total energy in FHCal vs Hodo Z^2, CCT2. 2D

    TCanvas* canAmps;
    TCanvas* canProfsSmall;
    TCanvas* canProfsBig;
    TCanvas* canZLSmall;
    TCanvas* canZLBig;
    TCanvas* canSelect;
    TCanvas* can_mod_sec;
    TCanvas* can_ZL_nEvent;
    TCanvas* can_totalE;
    TCanvas* can_totalE_hodoZ2;

    std::vector<PadInfo*> canAmpsPads;
    std::vector<PadInfo*> canProfsPads;
    std::vector<PadInfo*> canZLPads;
    vector<PadInfo*> canSelectPads;
    std::vector<PadInfo*> can_mod_secPads;
    std::vector<PadInfo*> can_ZL_nEventPads;
    std::vector<PadInfo*> can_totalEPads;
    std::vector<PadInfo*> can_totalE_hodoZ2Pads;
    std::vector<TString> NamesAmps;
    std::vector<TString> NamesProfs;
    std::vector<TString> NamesZL;
    vector<TString> NamesSelect;
    std::vector<TString> Names_mod_sec;
    std::vector<TString> Names_ZL_nEvent;
    std::vector<TString> Names_totalE;
    std::vector<TString> Names_totalE_hodoZ2;

    std::vector<TH1*> hModuleProfile =
        std::vector<TH1*>(kTOTAL_MODULES + 1, nullptr);   // Modules numerated from 1 to 54 inclusive
    std::vector<TH1*> h2_mod_ZL_nEvent =
        std::vector<TH1*>(kTOTAL_MODULES + 1, nullptr);   // Modules numerated from 1 to 54 inclusive

    ClassDef(BmnHistFHCal, 2)
};

#endif /* BMNHISTFHCal_H */
