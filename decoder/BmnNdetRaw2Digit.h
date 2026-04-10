#ifndef BmnNdetRaw2Digit_H
#define BmnNdetRaw2Digit_H

#include "BmnADCDigit.h"
#include "BmnNdetDigi.h"
#include "BmnSyncDigit.h"
#include "BmnTDCDigit.h"
#include "BmnTQDCADCDigit.h"
#include "FairLogger.h"
#include "TClonesArray.h"
#include "TString.h"
#include "WfmProcessor.h"

#include <boost/functional/hash.hpp>
#include <boost/program_options.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <optional>
#include <unordered_map>

class BmnNdetRaw2Digit : public WfmProcessor
{

  public:
    BmnNdetRaw2Digit(Int_t period, Int_t run, TString mappingFile, TString calibrationFile = "");
    BmnNdetRaw2Digit();

    ~BmnNdetRaw2Digit();

    void ParseConfig(TString mappingFile);
    void ParseCalibration(TString calibrationFile);
    void ParseINLcorrections();
    void fillEvent(TClonesArray* tdc_data,
                   TClonesArray* adc_data,
                   unordered_map<UInt_t, Long64_t>* mapTS,
                   TClonesArray* Ndetdigit);
    uint32_t correctINL(uint32_t time, std::pair<size_t, size_t> key);
    void print();

    digiPars GetDigiPars() { return fdigiPars; }
    auto GetChannelMap() { return fuoChannelMap; }

    std::optional<std::pair<float, float>> GetCalibPairFromAddress(uint32_t address);
    std::optional<uint32_t> GetAddressFromBoard(std::pair<size_t, size_t> key);
    std::vector<float> GetCalibSlewShiftFromAddress(uint32_t address);

  private:
    int fPeriodId;
    int fRunId;
    TString fmappingFileName;
    TString fcalibrationFileName;
    bool fApplyINL = false;
    bool fApplyAmplCalib = false;
    bool fApplyTimeCalib = false;

    std::unordered_map<std::pair<size_t, size_t>, uint32_t, boost::hash<std::pair<size_t, size_t>>>
        fuoChannelMap;   // physical channel <board, ch> to digi address
    std::unordered_map<std::pair<size_t, size_t>, std::vector<float>, boost::hash<pair<int, int>>>
        fINLcorrMap;   // physical channel <board, ch> to vector<corrections>
    std::unordered_map<uint32_t, std::pair<float, float>> fuoCalibMap;       // digi address to pair<calib, calibError>
    std::unordered_map<uint32_t, std::vector<float>> fuoCalibSlewShiftMap;   // digi address to vector<calib>

    ClassDef(BmnNdetRaw2Digit, 2);
};
#endif /* BmnNdetRaw2Digit_H */
