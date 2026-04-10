#ifndef BMNSIBTRAW2DIGIT_H
#define BMNSIBTRAW2DIGIT_H

#include "BmnADCDigit.h"
#include "BmnAdcProcessor.h"
#include "BmnEnums.h"
#include "BmnSiBTDigit.h"
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
using std::vector;
using namespace TMath;

struct BmnSiBTMapping
{
    Short_t layer;   // X = 0 or X' = 1
    UInt_t serial;
    Short_t module;
    Short_t channel;
    Int_t strip_shift;
    Char_t channel_name;
    Short_t station;
    StripMap* strips;
};
typedef map<Int_t, BmnSiBTMapping*> InChanMapSiBT;

class BmnSiBTRaw2Digit : public BmnAdcProcessor
{
  public:
    BmnSiBTRaw2Digit(Int_t period, Int_t run, TString MapFileName, BmnSetup bmnSetup = kBMNSETUP);
    virtual ~BmnSiBTRaw2Digit();

    BmnStatus FillEvent(TClonesArray* adc, TClonesArray* sts);
    BmnStatus FillProfiles(TClonesArray* adc);
    BmnStatus FillNoisyChannels();

    vector<BmnSiBTMapping*>& GetMap() { return fMap; };

  private:
    vector<BmnSiBTMapping*> fMap;
    map<UInt_t, InChanMapSiBT> fOuterMap;   // serial map
    map<PlMapKey, BmnSiBTMapping> fGlobalMap;
    map<Char_t, StripMap> fLocalMaps;
    TString fMapFileName;

    unique_ptr<BmnSiBTStationSet> fSiBTStationSet;

    BmnStatus ReadLocalMap(string& FileName, StripMap& Strips);
    BmnStatus ReadLocalMaps();
    BmnStatus ReadMapFile();

    inline Int_t MapStrip(BmnSiBTMapping& v, Short_t& iSmpl)
    {
        return v.strips->at(iSmpl) + v.strip_shift;
        //            return (iCh - v->channel) * GetNSamples() + iSmpl;
    }
    void ProcessAdc(TClonesArray* silicon, Bool_t doFill);

    ClassDef(BmnSiBTRaw2Digit, 1);
};

#endif /* BMNSIBTRAW2DIGIT_H */
