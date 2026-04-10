#ifndef BMNHodoRAW2DIGIT_H
#define BMNHodoRAW2DIGIT_H

#include "BmnHodoDigi.h"
#include "Riostream.h"
#include "TClonesArray.h"
#include "TString.h"
#include "WfmProcessor.h"

#include <TStopwatch.h>
#include <UniRun.h>
#include <boost/functional/hash.hpp>
#include <boost/program_options.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <optional>
#include <unordered_map>

class BmnHodoRaw2Digit : public WfmProcessor
{

  public:
    BmnHodoRaw2Digit(int period, int run, const std::string& mappingFile = "", const std::string& calibrationFile = "");
    BmnHodoRaw2Digit();

    ~BmnHodoRaw2Digit();

    TString GetName() { return "BmnHodoRaw2Digit"; }
    void ParseConfig(const std::string& file);
    void ParseCalibration(const std::string& file);
    void ParseINLcorrections();
    uint32_t correctINL(uint32_t time, std::pair<size_t, size_t> key);
    void fillEvent(TClonesArray* tdc_data, TClonesArray* adc_data, TClonesArray* Hododigit);
    void print();

    digiPars GetDigiPars() { return fdigiPars; }
    auto GetChannelMap() { return fuoChannelMap; }
    std::optional<std::pair<float, float>> GetCalibPairFromAddress(uint32_t address);
    std::optional<uint32_t> GetAddressFromBoard(std::pair<size_t, size_t> key);

  private:
    std::unordered_map<std::pair<size_t, size_t>, uint32_t, boost::hash<std::pair<size_t, size_t>>>
        fuoChannelMap;   // physical channel <board, ch> to digi address
    std::unordered_map<std::pair<size_t, size_t>, std::vector<float>, boost::hash<std::pair<size_t, size_t>>>
        fINLcorrMap;   // physical channel <board, ch> to vector<corrections>
    std::unordered_map<uint32_t, std::pair<float, float>> fuoCalibMap;   // digi address to pair<calib, calibIntegral>
    int fSaturation;

    bool fDoRLdeconvolve;
    int fRLthresold;     // Threshold to start deconvolution
    float fR2converge;   // R2 threshold to converge
    size_t fMaxIters;    // maximum number of iterations
    size_t fMaxPeaks;    // maximum number of pileup peaks
    std::vector<float> fEthalon;

    double fworkTime_real;
    double fworkTime_cpu;

    ClassDef(BmnHodoRaw2Digit, 5);
};
#endif /* BMNHodoRAW2DIGIT_H */
