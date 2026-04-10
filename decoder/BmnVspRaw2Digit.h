#pragma once
// C++
// #include <fstream>
// #include <iostream>
// #include <sstream>
// ROOT
#include <TClonesArray.h>
// BmnRoot
#include "BmnEnums.h"
#include "BmnVSPStationSet.h"

using std::map;
using std::vector;

class BmnVspRaw2Digit
{
  public:
    BmnVspRaw2Digit(Int_t period, Int_t run, TString mapFileName = "", BmnSetup bmnSetup = kBMNSETUP);
    //    BmnVspRaw2Digit(TString mappingFile, TString INLFile, TTree* digiTree = NULL);

    virtual ~BmnVspRaw2Digit() {}

    BmnStatus FillEvent(TClonesArray* adc, TClonesArray* digits);
    //    BmnStatus ReadChannelMap(TString mappingFile);

  private:
    uint32_t fPeriod;
    uint32_t fRun;
    BmnSetup fSetup;

    unique_ptr<BmnVSPStationSet> station_set;

    ClassDef(BmnVspRaw2Digit, 1);
};
