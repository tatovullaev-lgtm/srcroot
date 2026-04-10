#ifndef BMNSILICONRAW2DIGIT_H
#define BMNSILICONRAW2DIGIT_H

#include "BmnADCDigit.h"
#include "BmnAdcProcessor.h"
#include "BmnEnums.h"
#include "BmnSiliconDigit.h"
#include "TClonesArray.h"
#include "TColor.h"
#include "TMath.h"
#include "TString.h"
#include "TTree.h"

#include <BmnEventHeader.h>
#include <BmnFunctionSet.h>
#include <TSystem.h>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <vector>

using std::map;
using std::string;
using std::unordered_map;
using std::vector;
using namespace TMath;

struct BmnSiliconMapping
{
    Short_t layer;   // X = 0 or X' = 1
    UInt_t serial;
    Short_t module;
    Short_t channel_low;
    Short_t channel_high;
    Short_t station;
    bool inverted;
};
typedef map<Int_t, BmnSiliconMapping*> InChanMapSil;

class BmnSiliconRaw2Digit : public BmnAdcProcessor
{
  public:
    BmnSiliconRaw2Digit(Int_t period, Int_t run, TString MapFileName, BmnSetup bmnSetup = kBMNSETUP);
    virtual ~BmnSiliconRaw2Digit();

    BmnStatus FillEvent(TClonesArray* adc, TClonesArray* sts);
    BmnStatus FillProfiles(TClonesArray* adc);
    BmnStatus FillNoisyChannels();
    void CreatePedestalReport();

    vector<BmnSiliconMapping*>& GetMap() { return fMap; };

  private:
    vector<BmnSiliconMapping*> fMap;
    unordered_map<UInt_t, InChanMapSil> fOuterMap;   // serial map
    Int_t fEventId;
    TString fMapFileName;

    TCanvas* canStrip = nullptr;
    TH2F* hraw = nullptr;
    TH2F* hrms = nullptr;
    TH2F* hcorrp = nullptr;
    TH2F* hcorr = nullptr;
    TH2F* hfilter = nullptr;
    TH2F* hped = nullptr;
    TH2F* hcms = nullptr;
    //    TH2F* hscms;
    TH2F* hscms = nullptr;
    TH1F* hscms1 = nullptr;
    TH1F* hscms1full = nullptr;
    TH1F* hped1 = nullptr;
    TH1F* hsig = nullptr;
    Int_t nev = -1;
    //    Int_t nradc = 0;
    Int_t niterped;
    Int_t nchip;
    Int_t nchmin;
    Int_t npevents;

    Int_t zeroCnt = 0;
    Int_t allCnt = 0;

    unique_ptr<BmnSiliconStationSet> fSilStationSet;

    Double_t thresh;

    Double_t thrnoise;

    BmnStatus ReadMapFile();
    inline Int_t MapStrip(BmnSiliconMapping* v, Int_t& iCh, Short_t& iSmpl)
    {
        if (v->inverted)
            return (v->channel_high - iCh) * GetNSamples() + iSmpl;
        else
            return (iCh - v->channel_low) * GetNSamples() + iSmpl;
    }
    void ProcessAdc(TClonesArray* silicon, Bool_t doFill);

    ClassDef(BmnSiliconRaw2Digit, 1);
};

#endif /* BMNSILICONRAW2DIGIT_H */
