#ifndef BMNHISTScWall_H
#define BMNHISTScWall_H 1

#include "BmnHist.h"
#include "BmnScWallDigi.h"
#include "BmnScWallGeo.h"
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
#define ScWall_ROWS 1
#define ScWall_COLS 2

class BmnHistScWall : public BmnHist
{
  public:
    BmnHistScWall(TString title = "ScWall", TString path = "", Int_t periodID = 8, BmnSetup setup = kBMNSETUP);
    virtual ~BmnHistScWall();
    void CreateHistos();
    void Reset();
    void Register(THttpServer* serv);
    void SetDir(TFile* outFile, TTree* recoTree);
    void DrawBoth();
    void ClearRefRun();
    void FillFromDigi(DigiArrays* fDigiArrays);
    BmnStatus SetRefRun(Int_t id);
    void SetSelection(Int_t Cell, Int_t Trigger);
    void SetGeometry(const std::string& path);

  private:
    static constexpr int kTOTAL_CELLS = 173;   // SsWall 2022. Cells numerated from 1 to 174 inclusive

    long int fEventCounter = 1;
    BmnScWallGeo* fGeoHandler;   // stores map with detector address to 3D position and error
    Int_t fSelectedCell;
    Int_t fSelectedTrigger;

    TH2F* h2d_grid;             // signals in outer cells. 2D X-Y
    TH2F* h2d_grid_center;      // signals in inner cells. 2D X-Y
    TH2F* h2d_profile;          // signals in all cells. 2D cell-signal
    TH2F* h2d_cell_id;          // outer cells indexes. 2D X-Y
    TH2F* h2d_cell_id_center;   // inner cells indexes. 2D X-Y
    TH1F* hCellE;               // amplitude in selected cell
    TH1F* hCellAmpl;            // dE/dx in selected cell
    TH1F* wfmCell;              // waveform in selected cell
    TH1F* hModEn_scwall_nt;     // signals in all cells. 1D cell-signal

    TCanvas* canAmps;
    TCanvas* canModEn;
    TCanvas* canSelect;
    vector<PadInfo*> canAmpsPads;
    vector<PadInfo*> canModEnPads;
    vector<PadInfo*> canSelectPads;
    vector<TString> NamesAmps;
    vector<TString> NamesModEn;
    vector<TString> NamesSelect;

    TCanvas* canScintWall;
    vector<PadInfo*> canScintWallPads;
    vector<TString> NamescanScintWall;

    ClassDef(BmnHistScWall, 2)
};

#endif /* BMNHISTScWall_H */
