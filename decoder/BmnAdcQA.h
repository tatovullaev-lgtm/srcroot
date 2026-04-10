#ifndef BMNADCQA_H
#define BMNADCQA_H

#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <vector>
#include <memory>

#include <TString.h>
#include <TH2F.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TFile.h>

#include "BmnEnums.h"

using std::vector;
using std::map;
using std::tuple;
using std::unique_ptr;

class BmnAdcQA {
public:
    BmnAdcQA(Int_t period, Int_t run, TString det);
    virtual ~BmnAdcQA();

    Int_t GetPeriod() {
        return fPeriod;
    }

    Int_t GetRun() {
        return fRun;
    }

    Int_t GetVerbose() {
        return fVerbose;
    }

    void SetVerbose(Int_t v) {
        fVerbose = v;
    }
    void DrawDebugHists();
    void DrawDebugHists2D();
    void ClearDebugHists();
    //    void FillAveragePedestal(Int_t i_ser, Int_t ch, Int_t smpl, Int_t value);
    void SetAveragePedestal(UInt_t ser, Int_t ch, Int_t smpl, Int_t spill_id, Double_t content);
    void SaveHists();


protected:
    Int_t fVerbose;
    BmnSetup fSetup;

    void InitArrays();

    vector< vector< TH1* > > hPedLine;
    vector< vector< TH1* > > hCMode;
    vector< vector< TH1* > > hSMode;
    vector< vector< TH1* > > hPedLineSi;
    vector< TH1* > hPedSi;
    vector< TH1* > hCModeSi;
    vector< TH1* > hSModeSi;

    typedef tuple<UInt_t, UInt_t, UInt_t> AdcKey;
    map<AdcKey, unique_ptr<TH1> > fAdcHists;

    TString fDetName;
    Int_t fPeriod;
    Int_t fRun;

  ClassDef(BmnAdcQA, 1);
};

#endif /* BMNADCQA_H */
