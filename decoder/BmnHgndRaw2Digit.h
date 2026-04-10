#ifndef BmnHgndRaw2Digit_H
#define BmnHgndRaw2Digit_H

#include "BmnAbstractTDCDigit.h"
#include "BmnHgndCalibration.h"
#include "BmnHgndDigi.h"
#include "BmnSyncDigit.h"
#include "FairLogger.h"
#include "TClonesArray.h"
#include "TString.h"

#include <boost/math/special_functions/lambert_w.hpp>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <optional>
#include <unordered_map>

using BmnHgndCalibration::QdcFromTot;
using BmnHgndCalibration::TdcFromTot;

class BmnHgndRaw2Digit
{
  public:
    BmnHgndRaw2Digit() = default;
    BmnHgndRaw2Digit(int period, int run, const std::string& mappingFile = "", const std::string& calibrationFile = "");
    virtual ~BmnHgndRaw2Digit();

    void ParseConfig(const std::string& mappingFile);
    void ParseCalibration(const std::string& calibrationFile);
    void ParseINLcorrections();
    void fillEvent(TClonesArray* tdc_data, Long64_t T0Sync, TClonesArray* digits);
    void print();

    const std::unordered_map<uint32_t, uint32_t>& GetChannelMap() const { return fuoChannelMap; }
    std::optional<std::pair<TdcFromTot, QdcFromTot>> GetCalibPairFromAddress(uint32_t address) const;
    std::optional<uint32_t> GetAddressFromBoard(uint32_t key) const;

  private:
    std::unordered_map<uint32_t, uint32_t> fuoChannelMap;   // physical global channel to digi address
    std::unordered_map<uint32_t, std::pair<TdcFromTot, QdcFromTot>> fuoCalibMap;   //! digi address to pair<tdc, qdc>

    ClassDef(BmnHgndRaw2Digit, 1);
};
#endif /* BmnHgndRaw2Digit_H */
