//
// Base class for processing data from ADC strip detectors
// It's used for pedestal calculation
//

#ifndef BMNADCPROCESSOR_H
#define BMNADCPROCESSOR_H

// STL
#include <bitset>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>
// ROOT
#include <TCanvas.h>
#include <TClonesArray.h>
#include <TColor.h>
#include <TFile.h>
#include <TH2F.h>
#include <TMath.h>
#include <TStopwatch.h>
#include <TString.h>
#include <TStyle.h>
#include <TTree.h>
// FairRoot
#include <FairLogger.h>
// BmnRoot
#include "BmnADCDigit.h"
#include "BmnAliases.h"
#include "BmnCalibData.h"
#include "BmnEnums.h"

#include <BmnCSCStationSet.h>
#include <BmnGemStripStationSet.h>
#include <BmnSiBTStationSet.h>
#include <BmnSiProfStationSet.h>
#include <BmnSiliconStationSet.h>

#define N_EV_FOR_PEDESTALS 500
#define ADC_N_CHANNELS 64       // number of ADC channels
#define ADC_PROF_N_CHANNELS 2   // number of ADC channels
#define ADC_SIBT_N_SAMPLES 64   // number of samples in one SiBT digit
#define ADC128_N_SAMPLES 128    // number of samples in one ADC digit //silicon
#define ADC32_N_SAMPLES 32      // number of samples in one ADC digit //gem

typedef vector<Int_t> StripMap;

using std::map;
using std::unordered_map;
using std::vector;
using namespace TMath;

class BmnAdcProcessor
{
  public:
    BmnAdcProcessor(Int_t period, Int_t run, TString det, Int_t nCh, const Int_t nSmpl);
    virtual ~BmnAdcProcessor();

    BmnStatus RecalculatePedestals();
    BmnStatus RecalculatePedestalsAugmented();
    void PrecalcEventModsOld(TClonesArray* adc);
    void PrecalcEventMods(TClonesArray* adc);
    void PrecalcEventMods_simd(TClonesArray* adc);
    void (BmnAdcProcessor::*PrecalcEventModsImp)(TClonesArray* adc);
    void CalcEventMods();
    void CalcEventMods_simd();
    //    BmnStatus FillProfiles(TClonesArray *adc);
    Double_t CalcCMS(Double_t* samples, Int_t size);
    BmnStatus SaveFilterInfo(TFile* cTalibFile);
    BmnStatus LoadFilterInfo(TFile* cTalibFile);
    static BmnStatus DrawFilterInfo(BmnCalibData* data);

    Double_t**** GetPedData() { return fPedDat; }

    void SetNSerials(Int_t n) { fNSerials = n; }

    void SetNChannels(Int_t n) { fNChannels = n; }

    void SetApplyThreshold(bool b) { fApplyThreshold = b; }

    bool GetApplyThreshold() { return fApplyThreshold; }

    Int_t GetNSerials() { return fNSerials; }

    Int_t GetNChannels() { return fNChannels; }

    Int_t GetNSamples() { return fNSamples; }

    Float_t GetPedestal(Int_t iSer, Int_t ch, Int_t smpl) { return fPedVal[iSer][ch][smpl]; }

    Float_t*** GetPedestals() { return fPedVal; }

    Double_t*** GetPedestalsRMS() { return fPedCMod2; }

    Bool_t*** GetNoisyChipChannels() { return fNoisyChipChannels; }

    Int_t GetPeriod() { return fPeriod; }

    Int_t GetRun() { return fRun; }

    Int_t GetVerbose() { return fVerbose; }

    void SetVerbose(Int_t v) { fVerbose = v; }

    vector<UInt_t>& GetSerials() { return fAdcSerials; }

    unordered_map<UInt_t, Int_t>& GetSerialMap() { return fSerMap; }

    inline UInt_t GetBoundaryRun(UInt_t nSmpl)
    {
        // format for SILICON data was changed during March 2017 seance (run 1542)
        // format for GEM was changed after March 2017 seance (run 1992)
        // so we have to use this crutch.
        return (nSmpl == 128) ? 1542 : 1992;
    }

    void SetEvForPedestals(UInt_t v) { fEvForPedestals = v; }

    UInt_t GetEvForPedestals() { return fEvForPedestals; }

    void DrawDebugHists();
    void DrawDebugHists2D();
    void ClearDebugHists();
    void CopyData2PedMap(TClonesArray* adc, UInt_t ev);
    void SetThreshold(Double_t final_thr, Double_t thr_dif = -1, Double_t n_iters = -1, Double_t cmod_cut = -1);

  protected:
    Int_t fVerbose;
    Int_t drawCnt;
    Int_t drawCnt2d;
    Double_t thrDif;
    Double_t FinalThr;
    Int_t fNIters;
    Double_t cmodcut;
    Double_t thrMax;
    Int_t fNSerials;
    vector<UInt_t> fAdcSerials;   ///< list of serial id for ADC-detector
    const Int_t fNSamples;
    Int_t fNChannels;
    UInt_t fEvForPedestals;
    Double_t**** fPedDat;   ///< data set to calculate pedestals
    BmnSetup fSetup;
    bool fApplyThreshold;
    map<PlMapKey, Double_t> fSpecThreshMap;   // for special cases signal amplitude

    void InitArrays();
    void GrabNewSerial(UInt_t serial);

    template<typename cl>
    void GrabSerialsFromMap(unordered_map<UInt_t, cl> m)
    {
        for (auto& it : m) {
            UInt_t serial = it.first;
            auto serIter = fSerMap.find(serial);
            if (serIter == fSerMap.end()) {
                if (fVerbose > 1)
                    printf("Adding %08X to the map, iSer = %d\n", serial, fNSerials);
                fSerMap.insert(pair<UInt_t, Int_t>(serial, fNSerials++));
                fAdcSerials.push_back(serial);
            }
        }
    }

    template<typename SST>
    void InitNoiseArrays(SST& ss)
    {
        Int_t kNStations = ss->GetNStations();
        fSigProf = new TH1F***[kNStations];
        fNoisyChannels = new Bool_t***[kNStations];
        for (Int_t iSt = 0; iSt < kNStations; ++iSt) {
            auto* st = ss->GetStation(iSt);
            UInt_t nModules = st->GetNModules();
            fSigProf[iSt] = new TH1F**[nModules];
            fNoisyChannels[iSt] = new Bool_t**[nModules];
            for (UInt_t iMod = 0; iMod < nModules; ++iMod) {
                auto* mod = st->GetModule(iMod);
                Int_t nLayers = mod->GetNStripLayers();
                fSigProf[iSt][iMod] = new TH1F*[nLayers];
                fNoisyChannels[iSt][iMod] = new Bool_t*[nLayers];
                for (Int_t iLay = 0; iLay < nLayers; ++iLay) {
                    auto& lay = mod->GetStripLayer(iLay);
                    Int_t nStrips = lay.GetNStrips();
                    TString histName;
                    histName.Form("%s_%d_%d_%d", fDetName.Data(), iSt, iMod, iLay);
                    if (fVerbose)
                        printf("SigProf %s\n", histName.Data());
                    fSigProf[iSt][iMod][iLay] = new TH1F(histName, histName, nStrips, 0, nStrips);
                    fSigProf[iSt][iMod][iLay]->SetDirectory(0);
                    fNoisyChannels[iSt][iMod][iLay] = new Bool_t[nStrips];
                    for (Int_t iStrip = 0; iStrip < nStrips; ++iStrip)
                        fNoisyChannels[iSt][iMod][iLay][iStrip] = kFALSE;
                }
            }
        }
    }

    template<typename SST>
    void DeleteNoiseArrays(SST& ss)
    {
        Int_t kNStations = ss->GetNStations();
        for (Int_t iSt = 0; iSt < kNStations; ++iSt) {
            auto* st = ss->GetStation(iSt);
            for (Int_t iMod = 0; iMod < st->GetNModules(); ++iMod) {
                auto* mod = st->GetModule(iMod);
                for (Int_t iLay = 0; iLay < mod->GetNStripLayers(); ++iLay) {
                    delete fSigProf[iSt][iMod][iLay];
                    delete[] fNoisyChannels[iSt][iMod][iLay];
                }
                delete[] fSigProf[iSt][iMod];
                delete[] fNoisyChannels[iSt][iMod];
            }
            delete[] fSigProf[iSt];
            delete[] fNoisyChannels[iSt];
        }
        delete[] fNoisyChannels;
        delete[] fSigProf;
    }
    Double_t workTime_cpu = 0.0;
    Double_t workTime_real = 0.0;
    Double_t workTime_cpu_p = 0.0;
    Double_t workTime_real_p = 0.0;

    unordered_map<UInt_t, Int_t> fSerMap;   ///< ADC serials map

    TH1F**** fSigProf;
    Bool_t**** fNoisyChannels;
    // Double_t*** fAdc;
    Float_t*** fAdc;
    Double_t*** fPedRms;   // set of calculated pedestal errors
    // Double_t*** fPedVal; //set of calculated pedestals
    Float_t*** fPedVal;
    Double_t*** fPedValTemp;   // set of calculated pedestals
    // Double_t** fCMode; //set of calculated pedestal CMSs
    Float_t** fCMode;
    // Double_t** fCMode0; //set of calculated pedestal CMSs
    Float_t** fCMode0;
    // Double_t** fSMode; //set of calculated signal CMSs
    Float_t** fSMode;
    // Double_t** fSMode0; //set of calculated signal CMSs
    Float_t** fSMode0;

    vector<vector<TH1*>> hPedLine;
    vector<vector<TH1*>> hCMode;
    vector<vector<TH1*>> hSMode;
    vector<vector<TH1*>> hPedLineSi;
    vector<TH1*> hPedSi;
    vector<TH1*> hCModeSi;
    vector<TH1*> hSModeSi;

    Bool_t*** fNoisyChipChannels;   // set of noisy channel flags

    Float_t** fNvals;
    // UInt_t** fNvals; // number of valid (under threshold) pedestals
    UInt_t*** fNvalsCMod;   // number of valid (under threshold) pedestals
    UInt_t*** fNvalsADC;    // number of valid (under threshold) ADC signals
    Double_t*** fPedCMod;
    Double_t*** fPedCMod2;
    Double_t** fSumRmsV;

    TString fDetName;   // it's used for .txt files name
    Int_t fPeriod;
    Int_t fRun;

  private:
    UInt_t*** fAdcProfiles;
    TH2F* hcms;
    TH2F* hscms_adc;
    TH1F* hcms1;
    TH2F* hscms1_adc;
    TH1F* hcms1p;
    TH1F* hscms1p_adc;
    TH2F* hp;
    TH2F* hfilter;
    TH2F* hfilterMK;
    TCanvas* canStrip;

    ClassDef(BmnAdcProcessor, 1);
};

#endif /* BMNADCPROCESSOR_H */
