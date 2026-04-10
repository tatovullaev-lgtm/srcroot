#ifndef BMNTOF701RAW2DIGIT_H
#define BMNTOF701RAW2DIGIT_H

#include "BmnMath.h"
#include "BmnTDCDigit.h"
#include "BmnTof701Digit.h"
#include "Riostream.h"
#include "TClonesArray.h"
#include "TString.h"

#include <UniDetectorParameter.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>

// Side of the strip is stored as a bool variable
#define TOF701_LEFT true
#define TOF701_RIGHT false

#define TOF701_CHANNEL_NUMBER 64
#define TOF701_BIN_NUMBER 1024
#define TOF701_MAX_TIME (24.)       // In ns
#define TOF701_TDC_TYPE (0xD3)      // TDC64VHL
#define TOF701_TDCVXS_TYPE (0xD0)   // TDC72VXS
// const UInt_t kTDC64VHLE = 0x53;

// A simple class to compare the TDCDigits. See .cxx code
struct _Tof701TDCCompare
{
    bool operator()(const BmnTDCDigit& a, const BmnTDCDigit& b) const;
};

// Map element
struct BmnTof701Map2
{
    Short_t plane;
    Short_t strip;
    Bool_t side;
    BmnTof701Map2(Short_t, Short_t, Bool_t);
    BmnTof701Map2();
};

// TDC parameters
struct BmnTof701TDCParameters
{
    double INL[TOF701_CHANNEL_NUMBER][TOF701_BIN_NUMBER];   // INL
    BmnTof701Map2 ChannelMap[TOF701_CHANNEL_NUMBER];        // A BmnTof701Map2 for every channel
    double t[TOF701_CHANNEL_NUMBER];                        // To store the value temporarily. See .cxx code
    BmnTof701TDCParameters();                               // Simple constructor
};

class BmnTof701Raw2Digit
{
  public:
    BmnTof701Raw2Digit();                                           // BmnTof701Raw2Digit main constructor
    BmnTof701Raw2Digit(Int_t nPeriod, Int_t nRun, Int_t verbose);   // Calls setRun(...)
    virtual ~BmnTof701Raw2Digit();                                  // Destructor

    Bool_t setRun(Int_t nPerion, Int_t nRun);   // Load mapping and INL from the DB for run #nRun in period #nPeriod
    Bool_t setMapFromFile(TString placementMapFile, TString mapFile);        // Load mapping from two files
    void saveMapToFile(std::string placementMapFile, std::string mapFile);   // Save the mapping to two files

    void setINLFromFile(std::string INLFile);                          // Load INL from an INI file
    void saveINLToFile(std::string INLFile, unsigned int TDCSerial);   // Save INL for TDCSerial to an INI file

    void print() {}   // Prints some info
    void setVerbose(Int_t verbose) { fVerbose = verbose; }

    void FillEvent(TClonesArray* data, unordered_map<UInt_t, Long64_t>* mapTS, TClonesArray* Tof701digit);   //

    static UShort_t ToGlobalChannel(UChar_t HptdcId, UChar_t channel);

  private:
    ClassDef(BmnTof701Raw2Digit, 1);
    void init();   // BmnTof701Raw2Digit init function (called in BmnTof701Raw2Digit constructors)
    Int_t RunIndex, PeriodIndex, fVerbose;   // To store the RunIndex and PeriodIndex and print level
    UInt_t fSerialForTS;
    std::map<std::pair<UInt_t, UChar_t>, UInt_t> PlacementMap;   // Stores the placement map
    std::map<UInt_t, BmnTof701TDCParameters>
        TDCMap;   // Stores the TDC mapping (TDC's channel -> plane/strip/side) and INL
    void plmap_insert(UInt_t Serial, UChar_t Slot, UInt_t TDCSerial);   // See .cxx code

    // std::map provides a way to find TDC by Serial and Slot really fast (O(logN))
    // BmnTof701Parameters could also be found so fast (O(logN))
    // All other operations (extracting the INL, for example) are already fast because of the structure - O(1)
};
#endif /* BMNTOF701RAW2DIGIT_H */
