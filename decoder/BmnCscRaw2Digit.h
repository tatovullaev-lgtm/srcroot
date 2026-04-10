#ifndef BMNCSCRAW2DIGIT_H
#define BMNCSCRAW2DIGIT_H

#include "BmnADCDigit.h"
#include "BmnAdcProcessor.h"
#include "BmnCSCDigit.h"
#include "BmnEnums.h"
#include "BmnMath.h"
#include "TClonesArray.h"
#include "TH1F.h"
#include "TMath.h"
#include "TString.h"
#include "TTree.h"

#define N_CSC_MODULES 2
#define N_CSC_CHANNELS 2048

using std::map;
using std::string;
using std::unordered_map;
using std::vector;
using namespace TMath;

typedef pair<string, Short_t> LocalCSCMapKey;   // {prefix, module}

struct LocalCSC
{
    //    BmnCSCStation* station;
    map<pair<Short_t, Short_t>, Short_t> shift_map;   // {mod, layer} -> strip shift
    Int_t channel2layer[N_CSC_CHANNELS];              // {mod channel} -> layer
                                                      //    StripMap channel2layer; // {mod channel} -> layer
                                                      //    Int_t channel2layer[N_CSC_MODULES][N_CSC_CHANNELS];
    Int_t channel2strip[N_CSC_CHANNELS];              // {mod channel} -> strip
    //    StripMap channel2strip; // {mod channel} -> strip
    //    Int_t channel2strip[N_CSC_MODULES][N_CSC_CHANNELS];
};

struct BmnCscMapping
{
    Short_t layer;   // X = 0 or X' = 1
    UInt_t serial;
    Short_t chan_shift;
    Short_t module;
    Int_t channel_low;
    Int_t channel_high;
    Short_t station;
    string prefix;
    LocalCSC* local_map;
};

typedef map<Int_t, BmnCscMapping*> InChanMapCSC;   // channel ranges

class BmnCscRaw2Digit : public BmnAdcProcessor
{
  public:
    BmnCscRaw2Digit(Int_t period, Int_t run, TString MapFileName, BmnSetup bmnSetup = kBMNSETUP);
    ~BmnCscRaw2Digit();

    BmnStatus FillEvent(TClonesArray* adc, TClonesArray* csc);
    BmnStatus FillProfiles(TClonesArray* adc);
    BmnStatus FillNoisyChannels();

  private:
    TString fMapFileName;
    unordered_map<UInt_t, InChanMapCSC> fOuterMap;   // serial map
                                                     //    map<UInt_t, > fPrefixMap; // local map prefix map
                                                     //    vector<UInt_t> fSerials;
    map<LocalCSCMapKey, LocalCSC> fLocalMapsMap;
    Int_t fEventId;

    unique_ptr<BmnCSCStationSet> fCscStationSet;

    void ProcessDigit(BmnADCDigit* adcDig, BmnCscMapping* cscM, TClonesArray* csc, Bool_t doFill);
    void ProcessAdc(TClonesArray* csc, Bool_t doFill);
    BmnStatus ReadMapFile();
    BmnStatus ReadLocalMap(string& FileName, Int_t* Layers, Int_t* Strips, Int_t& iLay, Int_t& ChanShift);
    BmnStatus ReadLocalMaps();
    void MatchMaps();
    inline void MapStrip(BmnCscMapping* cscM,
                         UInt_t iCh,
                         Int_t iSmpl,
                         Int_t& station,
                         Int_t& module,
                         Int_t& layer,
                         Int_t& strip);

    ClassDef(BmnCscRaw2Digit, 2);
};

#endif /* BMNCSCRAW2DIGIT_H */
