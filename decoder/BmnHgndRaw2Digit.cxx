#include "BmnHgndRaw2Digit.h"

#include "TMath.h"
#include "TSystem.h"

#include <bitset>
#include <nlohmann/json.hpp>

#define TDCBIN 0.1   // 100 ps

void BmnHgndRaw2Digit::print()
{
    LOG(info) << Form("BmnHgndRaw2Digit");
}

BmnHgndRaw2Digit::BmnHgndRaw2Digit(int period, int run, const std::string& mappingFile, const std::string& calibFile)
{
    std::string resolvedMapFile = mappingFile.empty() ? Form("HGND_map_period%d.json", period) : mappingFile;
    std::string resolvedCalibFile = calibFile.empty() ? Form("HGND_calibration_period%d.json", period) : calibFile;

    LOG(info) << "BmnHgndRaw2Digit: Using mapping file: " << resolvedMapFile;
    LOG(info) << "BmnHgndRaw2Digit: Using calibration file: " << resolvedCalibFile;

    ParseConfig(resolvedMapFile);
    ParseCalibration(resolvedCalibFile);
}

void BmnHgndRaw2Digit::ParseConfig(const std::string& mappingFile)
{
    std::string fullPath = std::string(getenv("VMCWORKDIR")) + "/input/" + mappingFile;
    std::ifstream config_file(fullPath);
    if (!config_file.is_open()) {
        LOG(error) << Form("BmnHgndRaw2Digit::ParseConfig: Cannot open config file: %s", fullPath.c_str());
        return;
    }

    nlohmann::json j;
    try {
        config_file >> j;
    } catch (const std::exception& e) {
        LOG(error) << "BmnHgndRaw2Digit::ParseConfig: Failed to parse JSON: " << e.what();
        return;
    }

    std::string version = j.value("version", "unknown");
    std::string comment = j.value("comment", "none");

    LOG(debug) << "BmnHgndRaw2Digit::ParseConfig. Mapping version: " << version;
    LOG(debug) << "BmnHgndRaw2Digit::ParseConfig. Comment: " << comment;

    fuoChannelMap.clear();
    for (const auto& entry : j["mapping"]) {
        int gl_ch = entry["global_ch"];
        int det = entry["detector"];
        int lay = entry["layer"];
        int row = entry["row"];
        int col = entry["column"];

        fuoChannelMap[gl_ch] = BmnNdetAddress::GetAddress(det, row, col, lay);
    }
}

void BmnHgndRaw2Digit::ParseCalibration(const std::string& calibrationFile)
{
    std::string fullPath = std::string(getenv("VMCWORKDIR")) + "/parameters/hgnd/" + calibrationFile;
    std::ifstream input(fullPath);
    if (!input.is_open()) {
        LOG(error) << "BmnHgndRaw2Digit::ParseCalibration: Cannot open file " << fullPath;
        return;
    }

    nlohmann::json j;
    try {
        input >> j;
    } catch (const std::exception& e) {
        LOG(error) << "BmnHgndRaw2Digit::ParseCalibration: Failed to parse JSON: " << e.what();
        return;
    }

    std::string version = j.value("version", "unknown");
    std::string comment = j.value("comment", "none");
    double totThreshold = j.value("totThreshold", 20.0);   // default in mV
    double Rload = j.value("Rload", 33.0);                 // default in Ohm

    LOG(debug) << "BmnHgndRaw2Digit::ParseCalibration. Calibration version: " << version;
    LOG(debug) << "BmnHgndRaw2Digit::ParseCalibration. Comment: " << comment;

    fuoCalibMap.clear();
    for (const auto& entry : j["calibration"]) {
        int det = entry["detector"];
        int lay = entry["layer"];
        int row = entry["row"];
        int col = entry["column"];
        double off = entry["offset"];
        double p = entry["p"];
        double tau = entry["tau"];
        double RC = entry["RC"];

        uint32_t address = BmnNdetAddress::GetAddress(det, row, col, lay);
        TdcFromTot tdc(off, p, tau, RC);
        QdcFromTot qdc(p, tau, RC, totThreshold, Rload);
        fuoCalibMap.emplace(address, std::make_pair(tdc, qdc));
    }
}

std::optional<uint32_t> BmnHgndRaw2Digit::GetAddressFromBoard(uint32_t key) const
{
    try {
        return fuoChannelMap.at(key);
    } catch (const std::out_of_range& e) {
        LOG(debug) << "BmnHgndRaw2Digit::GetAddressFromBoard. Invalid key " << key;
        return std::nullopt;
    }
}

std::optional<std::pair<TdcFromTot, QdcFromTot>> BmnHgndRaw2Digit::GetCalibPairFromAddress(uint32_t address) const
{
    try {
        return fuoCalibMap.at(address);
    } catch (const std::out_of_range& e) {
        LOG(debug) << "BmnHgndRaw2Digit::GetCalibPairFromAddress " << std::bitset<32>(address)
                   << " is marked badly in calibration file.";
        return std::nullopt;
    }
}

void BmnHgndRaw2Digit::fillEvent(TClonesArray* tdc, Long64_t T0Sync, TClonesArray* digits)
{

    LOG(debug) << "BmnHgndRaw2Digit::fillEvent";
    for (Int_t iTdc = 0; iTdc < tdc->GetEntriesFast(); iTdc++) {
        BmnAbstractTDCDigit* tdcDig = (BmnAbstractTDCDigit*)tdc->At(iTdc);
        auto key = tdcDig->GetChannel();
        auto catch_address = GetAddressFromBoard(key);
        if (!catch_address.has_value())
            continue;   // Not connected lines
        auto address = catch_address.value();
        if (address == 0)
            continue;   // not connected lines

        auto catch_calib = GetCalibPairFromAddress(address);
        if (!catch_calib.has_value())
            continue;   // not involved in the analysis
        auto calib_pair = catch_calib.value();

        BmnHgndDigi ThisDigi;
        ThisDigi.reset();
        ThisDigi.SetAddress(address);
        uint64_t wr_sec = static_cast<uint64_t>(tdcDig->GetTimeSec()) * 1000000000ULL;
        uint64_t wr_nsec = static_cast<uint64_t>(tdcDig->GetTimeNsec());
        int64_t wr_tof_ns = static_cast<int64_t>(wr_sec + wr_nsec) - T0Sync;

        double time = wr_tof_ns + tdcDig->GetToa() * TDCBIN;
        double tot = tdcDig->GetTot() * TDCBIN;

        ThisDigi.SetToa(time);
        ThisDigi.SetTot(tot);
        ThisDigi.SetTime(time - calib_pair.first(tot));
        ThisDigi.SetSignal(calib_pair.second(tot));

        new ((*digits)[digits->GetEntriesFast()]) BmnHgndDigi(ThisDigi);
    }
}

BmnHgndRaw2Digit::~BmnHgndRaw2Digit() {}
