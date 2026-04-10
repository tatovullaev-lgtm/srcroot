/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   BmnHistSilicon.h
 * Author: ilnur
 *
 * Created on March 9, 2017, 10:39 AM
 */

#ifndef BMNHISTSILICON_H
#define BMNHISTSILICON_H
#include "BmnAdcProcessor.h"
#include "BmnHist.h"
#include "BmnSiliconDigit.h"
#include "BmnSiliconStationSet.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TClonesArray.h"
#include "TFile.h"
#include "TFolder.h"
#include "TGraph.h"
#include "TH1I.h"
#include "THttpServer.h"
#include "TString.h"

#include <TNamed.h>
#include <exception>
#include <stdlib.h>
#include <vector>

using std::vector;

class BmnHistSilicon : public BmnHist
{
  public:
    BmnHistSilicon(TString title, TString path = "", Int_t periodID = 7, BmnSetup setup = kBMNSETUP);
    virtual ~BmnHistSilicon();
    void Reset();
    void Register(THttpServer* serv);
    void SetDir(TFile* outFile = NULL, TTree* recoTree = NULL);
    void DrawBoth();
    void FillFromDigi(DigiArrays* fDigiArrays);
    void ClearRefRun();
    void ChangeStation(Int_t stationNumber);
    void ResetCanvases(Int_t numberOfModules);
    BmnStatus SetRefRun(Int_t id);

  private:
    vector<vector<TString>> NamesStrip;
    vector<vector<TString>> NamesSignal;
    vector<vector<TString>> NamesStripSignal;
    vector<vector<TString>> NamesStripNoise;
    vector<vector<TString>> NamesChipSignal;

    vector<vector<vector<TH1I*>>> histSiliconStrip;                // 1-dim hist for signal in each layer
    vector<vector<vector<vector<TH1F*>>>> histSiliconChipSignal;   // 1-dim hist for signal in each chip in each layer
    vector<vector<vector<TH2F*>>> histSiliconStripSignal;          // 2-dim hist for number of strip and signal
    vector<vector<vector<TH1F*>>> histSiliconStripNoise;           // 1-dim hist for noise in each layer
    vector<vector<TH2F*>> histSiliconSignal2D;   // 2-dim hist for corrrelation between signals in up and low layers

    vector<vector<vector<Double_t>>> signalSilicon;   // vector of signals for 2dim Hists

    TCanvas* canSiliconStrip;
    TCanvas* canSiliconSignal;
    TCanvas* canSiliconStripSignal;
    TCanvas* canSiliconStripNoise;
    TCanvas* canSiliconChipSignal;

    vector<vector<PadInfo*>> canStripPads;
    vector<vector<PadInfo*>> canSignalPads;
    vector<vector<PadInfo*>> canStripSignalPads;
    vector<vector<PadInfo*>> canStripNoisePads;
    vector<vector<PadInfo*>> canChipSignalPads;

    unique_ptr<BmnSiliconStationSet> stationSet;
    Int_t nStations;
    UInt_t nMods;
    UInt_t nLayers;
    UInt_t sumMods;
    UInt_t maxLayers;
    UInt_t maxMods;
    UInt_t numberOfChips;
    UInt_t numberOfStrips;
    Int_t fStation;

    ClassDef(BmnHistSilicon, 1)
};

#endif /* BMNHISTSILICON_H */