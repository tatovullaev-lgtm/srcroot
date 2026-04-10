#ifndef BMNHISTHgnd_H
#define BMNHISTHgnd_H 1

#include "BmnHgndDigi.h"
#include "BmnHist.h"
// #include "BmnHgndGeo.h"
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
#define Hgnd_ROWS 1
#define Hgnd_COLS 2

class BmnHistHgnd : public BmnHist
{
  public:
    BmnHistHgnd(TString title = "Hgnd", TString path = "", Int_t periodID = 8, BmnSetup setup = kBMNSETUP);
    virtual ~BmnHistHgnd();
    void CreateHistos();
    void Reset();
    void Register(THttpServer* serv);
    void SetDir(TFile* outFile, TTree* recoTree);
    void DrawBoth();
    void ClearRefRun();
    void FillFromDigi(DigiArrays* fDigiArrays);
    BmnStatus SetRefRun(Int_t id);
    void SetSelection(Int_t Cell, Int_t Layer, Int_t Trigger);
    void SetGeometry(const std::string& path);

  private:
    static constexpr int kTOTAL_CELLS = 121;   // Hgnd Prototype 2025. Cells numerated from 1 to 121 inclusive
    static constexpr int kTOTAL_COLS = 11;     // Hgnd Prototype 2025. Cols numerated from 1 to 11 inclusive
    static constexpr int kTOTAL_ROWS = 11;     // Hgnd Prototype 2025. Rows numerated from 1 to 11 inclusive
    static constexpr int kTOTAL_LAYRS = 1;     // Hgnd Prototype 2025. Layers numerated from 1 to 1 inclusive
    std::vector<unsigned int> MapCell2Ch = {0, 3, 6, 9, 2, 5, 8, 1, 4, 7};   // cells numerated from 1 to 9 inclusive

    long int fEventCounter = 1;
    float nEn;
    float timemin;
    //    BmnHgndGeo* fGeoHandler;   // stores map with detector address to 3D position and error
    Int_t fSelectedCell;
    Int_t fSelectedLayer;
    Int_t fSelectedTrigger;

    TH1F* h1d_ECells;
    TH2F* h2_grid;
    TH1F* h1d_ProfileSlice;   // profile over slices of detector (sum over cells in current layer)
    TH1F* h1d_timemin;        // minimum timestamp in event
    TH1F* hChE;               // amplitude in selected channel
    TH1F* wfmCh;              // waveform in selected channel
    TH2F* h2_Layer_Cell;
    TH1F* h1_Cells;
    TH1F* h1_CellsAmpl;
    TH1F* h1_CellsTime;
    TH2F* h2_lrampl;
    TH2F* h2_lrtime;

    TCanvas* canAmps;
    TCanvas* canv_hProfCell;
    TCanvas* canv_Slice;
    TCanvas* canv_Cells;
    TCanvas* canSelect;
    TCanvas* canv_Ampl;
    TCanvas* canv_Time;
    TCanvas* canv_hAmplCell;
    TCanvas* canv_hCellAmp;
    TCanvas* canv_hCellTime;
    vector<PadInfo*> canAmpsPads;
    vector<PadInfo*> canv_hProfCellPads;
    vector<PadInfo*> canv_SlicePads;
    vector<PadInfo*> canv_CellsPads;
    vector<PadInfo*> canSelectPads;
    vector<PadInfo*> canv_AmplPads;
    vector<PadInfo*> canv_TimePads;
    vector<PadInfo*> canv_hAmplCellPads;
    vector<PadInfo*> canv_hCellAmpPads;
    vector<PadInfo*> canv_hCellTimePads;
    vector<TString> NamesAmps;
    vector<TString> NamesProfs;
    vector<TString> NamesSlice;
    vector<TString> NamesCells;
    vector<TString> NamesSelect;
    vector<TString> NamesAmpl;
    vector<TString> NamesTime;
    vector<TString> NamesAmplCell;
    vector<TString> NamesCellAmp;
    vector<TString> NamesCellTime;

    std::vector<TH1*> hProfCell =
        std::vector<TH1*>(kTOTAL_CELLS + 1, nullptr);   // Cells numerated from 1 to 121 inclusive
    std::vector<TH1*> hCellAmp =
        std::vector<TH1*>(kTOTAL_LAYRS * kTOTAL_CELLS + 1, nullptr);   // Cells numerated for channels
    std::vector<TH1*> hCellTime = std::vector<TH1*>(kTOTAL_LAYRS * kTOTAL_CELLS + 1, nullptr);
    std::array<float, kTOTAL_CELLS * kTOTAL_LAYRS + 1> meanamp;   // Cells numerated for all channels
    std::array<float, kTOTAL_CELLS * kTOTAL_LAYRS + 1> meantime;
    std::array<float, kTOTAL_CELLS * kTOTAL_LAYRS + 1> ch_entries;

    ClassDef(BmnHistHgnd, 2)
};

#endif /* BMNHISTHgnd_H */
