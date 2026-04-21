#ifndef BMNSCWALLRAW2DIGIT_H
#define	BMNSCWALLRAW2DIGIT_H

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
#include "BmnScWallDigi.h"
#include "WfmProcessor.h"

class BmnScWallRaw2Digit : public WfmProcessor {

public:
    BmnScWallRaw2Digit(int period, int run, TString mappingFile, TString calibrationFile = "");
    BmnScWallRaw2Digit();

    ~BmnScWallRaw2Digit();

    void ParseConfig(TString mappingFile);
    void ParseCalibration(TString calibrationFile);
    void fillEvent(TClonesArray *data, TClonesArray *ScWalldigit);
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

    ClassDef(BmnScWallRaw2Digit, 2);
};
#endif	/* BMNSCWALLRAW2DIGIT_H */


