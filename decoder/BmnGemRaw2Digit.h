#ifndef BMNGEMRAW2DIGIT_H
#define BMNGEMRAW2DIGIT_H

#include "BmnADCDigit.h"
#include "BmnAdcProcessor.h"
#include "BmnCSCDigit.h"
#include "BmnGemStripDigit.h"
#include "TClonesArray.h"
#include "TH1F.h"
#include "TMath.h"
#include "TString.h"
#include "TTree.h"

#include <BmnEventHeader.h>
#include <BmnFunctionSet.h>
#include <TSystem.h>
#include <UniDetector.h>
#include <UniDetectorParameter.h>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <vector>

#define N_CH_BUF 4096

using namespace std;
using namespace TMath;

struct BmnGemMap
{
    Int_t strip;
    Int_t lay;   // strip type: 0 - x, 1 - y
    Int_t mod;   // hot zones: 1 - inner zone, 0 - outer zone

    BmnGemMap(Int_t s, Int_t l, Int_t m)
        : strip(s)
        , lay(l)
        , mod(m)
    {}

    BmnGemMap()
        : strip(-1)
        , lay(-1)
        , mod(-1)
    {}
};

struct GemMapLine
{
    Int_t Serial;
    Int_t Ch_lo;
    Int_t Ch_hi;
    Int_t GEM_id;
    Int_t Side;
    Int_t Type;
    Int_t Station;
    Int_t Module;
    Int_t Zone;

    void Print()
    {
        cout << Serial << " " << Ch_lo << " " << Ch_hi << " " << GEM_id << " " << Side << " " << Type << " " << Station
             << " " << Module << " " << Zone << endl;
    }

    GemMapLine()
        : Serial(-1)
        , Ch_lo(-1)
        , Ch_hi(-1)
        , GEM_id(-1)
        , Side(-1)
        , Type(-1)
        , Station(-1)
        , Module(-1)
        , Zone(-1)
    {}
};

class BmnGemRaw2Digit : public BmnAdcProcessor
{
  public:
    BmnGemRaw2Digit(Int_t period, Int_t run, TString mapFileName, BmnSetup bmnSetup = kBMNSETUP);
    ~BmnGemRaw2Digit();

    BmnStatus FillEvent(TClonesArray* adc, TClonesArray* gem);
    BmnStatus FillProfiles(TClonesArray* adc);
    BmnStatus FillNoisyChannels();

  private:
    // starting thresholds, number of iterations
    Int_t niterped = 3;
    Float_t thrped = 35;
    Float_t thrpedcsc = 80;
    vector<TH1I*> hNhits;

    BmnGemMap* fSmall;
    BmnGemMap* fMid;
    vector<BmnGemMap*> fBigHot;
    vector<BmnGemMap*> fBig;

    unique_ptr<BmnGemStripStationSet> fGemStationSet;

    TString fMapFileName;

    vector<GemMapLine*> fMap;
    inline void
        MapStrip(GemMapLine* gemM, UInt_t ch, Int_t iSmpl, Int_t& station, Int_t& mod, Int_t& lay, Int_t& strip);
    void ProcessAdc(TClonesArray* silicon, Bool_t doFill);
    BmnStatus ReadMap(TString parName, BmnGemMap* m, Int_t lay, Int_t mod);
    BmnStatus ReadLocalMap(TString parName, BmnGemMap* m, Int_t lay, Int_t mod);
    BmnStatus ReadGlobalMap(TString FileName);

    ClassDef(BmnGemRaw2Digit, 1);
};

#endif /* BMNGEMRAW2DIGIT_H */
