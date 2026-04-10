#ifndef BmnFHCalRaw2Digit_H
#define	BmnFHCalRaw2Digit_H

#include <iostream>
#include <numeric>
#include <optional>

#include "FairLogger.h"
#include "TString.h"
#include "TClonesArray.h"
#include "BmnADCDigit.h"
#include "BmnSyncDigit.h"

#include "Riostream.h"
#include <cstdlib>
#include <UniRun.h>

#include <unordered_map>
#include <boost/functional/hash.hpp>
#include <boost/program_options.hpp>
#include "BmnFHCalDigi.h"
#include "WfmProcessor.h"

class BmnFHCalRaw2Digit : public WfmProcessor {

public:
    BmnFHCalRaw2Digit(Int_t period, Int_t run, TString mappingFile, TString calibrationFile = "");
    BmnFHCalRaw2Digit();

    ~BmnFHCalRaw2Digit();

    TString GetName() { return "BmnHodoRaw2Digit"; }
    void ParseConfig(TString mappingFile);
    void ParseCalibration(TString calibrationFile);
    void fillEvent(TClonesArray *data, TClonesArray *FHCaldigit);
    void print();

    digiPars GetDigiPars() {return fdigiPars;}
    auto GetChannelMap() {return fuoChannelMap;}
    std::optional<std::pair<float, float>> GetCalibPairFromAddress(uint32_t address);
    std::optional<uint32_t> GetAddressFromBoard(std::pair<size_t,size_t> key);
    
private:

    int fPeriodId; 
    int fRunId;
    TString fmappingFileName;
    TString fcalibrationFileName;

    std::unordered_map<std::pair<size_t,size_t>, uint32_t, boost::hash<std::pair<size_t,size_t>>> fuoChannelMap; // physical channel <board, ch> to digi address
    std::unordered_map<uint32_t, std::pair<float,float>> fuoCalibMap; // digi address to pair<calib, calibError>

    ClassDef(BmnFHCalRaw2Digit, 2);
};
#endif	/* BmnFHCalRaw2Digit_H */


