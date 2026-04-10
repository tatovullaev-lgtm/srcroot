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
#ifndef BMNHISTSiBT_H
#define BMNHISTSiBT_H 1

#include "BmnHist.h"
#include "BmnSiBTStationSet.h"

#include <TGraph.h>
#include <TH1.h>
#include <vector>

using namespace std;

class BmnHistSiBT : public BmnHist
{
  public:
    BmnHistSiBT(TString title, TString path = "", Int_t periodID = 8);
    virtual ~BmnHistSiBT();
    void Reset();
    void Register(THttpServer* serv);
    void SetDir(TFile* outFile = NULL, TTree* recoTree = NULL);
    void DrawBoth();
    void FillFromDigi(DigiArrays* fDigiArrays);
    void ClearRefRun();
    BmnStatus SetRefRun(Int_t id);

  private:
    unique_ptr<BmnSiBTStationSet> sibtStationSet;
    vector<TString> NamesStrip;
    vector<TString> NamesSignal;
    vector<TString> NamesStripSignal;
    vector<TString> NamesStripNoise;
    vector<TString> NamesChipSignal;

    vector<vector<vector<TH1F*>>> histSiBTStrip;                // 1-dim hist for signal in each layer
    vector<vector<vector<vector<TH1F*>>>> histSiBTChipSignal;   // 1-dim hist for signal in each chip in each layer
    vector<vector<vector<TH2F*>>> histSiBTStripSignal;          // 2-dim hist for number of strip and signal
    vector<vector<vector<TH1F*>>> histSiBTStripNoise;           // 1-dim hist for noise in each layer
    vector<vector<TH2F*>> histSiBTSignal2D;   // 2-dim hist for corrrelation between signals in up and low layers

    vector<vector<vector<Double_t>>> signalSiBT;   // vector of signals for 2dim Hists

    TCanvas* canSiBTStrip;
    TCanvas* canSiBTSignal;
    TCanvas* canSiBTStripSignal;
    TCanvas* canSiBTStripNoise;
    TCanvas* canSiBTChipSignal;

    vector<PadInfo*> canStripPads;
    vector<PadInfo*> canSignalPads;
    vector<PadInfo*> canStripSignalPads;
    vector<PadInfo*> canStripNoisePads;
    vector<PadInfo*> canChipSignalPads;

    Int_t sumMods;
    Int_t maxLayers;
    Int_t maxMods;
    Int_t numberOfStations;
    Int_t numberOfChips;
    Int_t numberOfStrips;

    ClassDef(BmnHistSiBT, 1)
};

#endif /* BMNMONHISTS_H */