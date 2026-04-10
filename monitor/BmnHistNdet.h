#ifndef BMNHISTNdet_H
#define BMNHISTNdet_H 1

#include "BmnHist.h"
#include "BmnNdetDigi.h"
#include "BmnNdetGeo.h"
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
#define Ndet_ROWS 1
#define Ndet_COLS 2

class BmnHistNdet : public BmnHist
{
  public:
    BmnHistNdet(TString title = "Ndet", TString path = "", Int_t periodID = 8, BmnSetup setup = kBMNSETUP);
    virtual ~BmnHistNdet();
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
    static constexpr int kTOTAL_CELLS = 9;        // Ndet Prototype 2022. Cells numerated from 1 to 9 inclusive
    static constexpr int kTOTAL_COLS = 3;         // Ndet Prototype 2022. Cols numerated from 1 to 3 inclusive
    static constexpr int kTOTAL_ROWS = 3;         // Ndet Prototype 2022. Rows numerated from 1 to 3 inclusive
    static constexpr int kTOTAL_LAYRS = 16;       // Ndet Prototype 2022. Layers numerated from 1 to 15 inclusive
    static constexpr int kEM_LAYRS = 6;           // Ndet Prototype 2022. Layers numerated from 1 to 6 inclusive
    static constexpr int kHAD_LAYRS = 9;          // Ndet Prototype 2022. Layers numerated from 1 to 6 inclusive
    static constexpr int kTQDC_MAX_CHAN = 16;     // Ndet Prototype 2022. Mx tqdc channels
    static constexpr int kTQDC_MAX_MODULES = 9;   // Ndet Prototype 2022. Mx tqdc modules
    std::vector<unsigned int> MapCell2Ch = {0, 3, 6, 9, 2, 5, 8, 1, 4, 7};   // cells numerated from 1 to 9 inclusive

    long int fEventCounter = 1;
    float nEn;
    float timemin;
    BmnNdetGeo* fGeoHandler;   // stores map with detector address to 3D position and error
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
    TH2F* h2_hits;
    TH2F* h2_mean_amp;
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
    TCanvas* canv_hCellAmpEM;
    TCanvas* canv_hCellAmpH;
    TCanvas* canv_hCellTimeEM;
    TCanvas* canv_hCellTimeH;
    TCanvas* canv_hits;
    vector<PadInfo*> canAmpsPads;
    vector<PadInfo*> canv_hProfCellPads;
    vector<PadInfo*> canv_SlicePads;
    vector<PadInfo*> canv_CellsPads;
    vector<PadInfo*> canv_hitsPads;
    vector<PadInfo*> canSelectPads;
    vector<PadInfo*> canv_AmplPads;
    vector<PadInfo*> canv_TimePads;
    vector<PadInfo*> canv_hAmplCellPads;
    vector<PadInfo*> canv_hCellAmpEMPads;
    vector<PadInfo*> canv_hCellAmpHPads;
    vector<PadInfo*> canv_hCellTimeEMPads;
    vector<PadInfo*> canv_hCellTimeHPads;
    vector<TString> NamesAmps;
    vector<TString> NamesProfs;
    vector<TString> NamesSlice;
    vector<TString> NamesCells;
    vector<TString> NamesSelect;
    vector<TString> NamesAmpl;
    vector<TString> NamesTime;
    vector<TString> NamesAmplCell;
    vector<TString> NamesCellAmpEM;
    vector<TString> NamesCellAmpH;
    vector<TString> NamesCellTimeEM;
    vector<TString> NamesCellTimeH;
    vector<TString> NamesHits;

    std::vector<TH1*> hProfCell =
        std::vector<TH1*>(kTOTAL_CELLS + 1, nullptr);   // Cells numerated from 1 to 9 inclusive
    std::vector<TH1*> hCellAmpEM =
        std::vector<TH1*>(kEM_LAYRS * kTOTAL_CELLS + 1, nullptr);   // Cells numerated for EM channels
    std::vector<TH1*> hCellAmpH =
        std::vector<TH1*>(kHAD_LAYRS * kTOTAL_CELLS + 1, nullptr);   // Cells numerated for Hadron channels
    std::vector<TH1*> hCellTimeEM = std::vector<TH1*>(kEM_LAYRS * kTOTAL_CELLS + 1, nullptr);
    std::vector<TH1*> hCellTimeH = std::vector<TH1*>(kHAD_LAYRS * kTOTAL_CELLS + 1, nullptr);
    std::array<float, kTOTAL_CELLS * kTOTAL_LAYRS + 1> meanamp;   // Cells numerated for all channels
    std::array<std::array<float, kTQDC_MAX_CHAN + 1>, kTQDC_MAX_MODULES + 1>
        meanamp_test;   // Cells numerated for all channels
    std::array<float, kTOTAL_CELLS * kTOTAL_LAYRS + 1> meantime;
    std::array<float, kTOTAL_CELLS * kTOTAL_LAYRS + 1> ch_entries;
    std::array<std::array<int, kTQDC_MAX_CHAN + 1>, kTQDC_MAX_MODULES + 1> ch_entries_test;

    ClassDef(BmnHistNdet, 2)
};

#endif /* BMNHISTNdet_H */
