#include "BmnScWallRaw2Digit.h"

#include "TMath.h"
#include "TSystem.h"

#include <bitset>

void BmnScWallRaw2Digit::print()
{
    LOG(info) << Form("BmnScWallRaw2Digit");
}

BmnScWallRaw2Digit::BmnScWallRaw2Digit()
    : WfmProcessor()
{
    fPeriodId = 0;
    fRunId = 0;
}

BmnScWallRaw2Digit::BmnScWallRaw2Digit(int period, int run, TString mappingFile, TString CalibrationFile)
    : WfmProcessor()
{
    LOG(info) << Form("BmnScWallRaw2Digit : Config file: %s", mappingFile.Data());
    LOG(info) << Form("BmnScWallRaw2Digit : Calibration file: %s", CalibrationFile.Data());
    fPeriodId = period;
    fRunId = run;
    fmappingFileName = mappingFile;
    ParseConfig(fmappingFileName);
    fcalibrationFileName = CalibrationFile;
    ParseCalibration(fcalibrationFileName);
}

void BmnScWallRaw2Digit::ParseConfig(TString mappingFile)
{

    namespace po = boost::program_options;

    TString dir = getenv("VMCWORKDIR");
    TString path = dir + "/input/";

    float version;
    std::string comment;
    std::vector<std::string> configuration;

    // Setup options.
    po::options_description desc("Options");
    desc.add_options()("VERSION.id", po::value<float>(&version),
                       "version identificator")("COMMENT.str", po::value<std::string>(&comment), "comment")(
        "CONFIGURATION.config", po::value<std::vector<std::string>>(&configuration)->multitoken(), "configuration");

    // Load config file.
    po::variables_map vm;
    std::ifstream config_file((path + mappingFile).Data(), std::ifstream::in);
    if (!config_file.is_open()) {
        LOG(error) << Form("BmnScWallRaw2Digit : Loading Config from file: %s - file open error!", mappingFile.Data());
        return;
    }
    LOG(debug) << Form("BmnScWallRaw2Digit : Loading Config from file: %s", mappingFile.Data());
    po::store(po::parse_config_file(config_file, desc), vm);
    config_file.close();
    po::notify(vm);

    std::string board_serial;
    size_t board_channel;
    int cell_id;
    std::string zone;

    fuoChannelMap.clear();
    for (auto it : configuration) {
        istringstream ss(it);
        ss >> board_serial >> board_channel >> cell_id >> zone;
        if (zone == 'Z')
            continue;

        auto key = std::make_pair(std::stoul(board_serial, nullptr, 16), board_channel);
        fuoChannelMap[key] = BmnScWallAddress::GetAddress(cell_id);
    }
    // std::LOG(debug) << "COMMENT.str: " << comment;
}

void BmnScWallRaw2Digit::ParseCalibration(TString calibrationFile)
{

    namespace po = boost::program_options;

    TString dir = getenv("VMCWORKDIR");
    TString path = dir + "/parameters/scwall/";

    float version;
    std::string comment;
    std::vector<std::string> harmonics;
    std::vector<std::string> calibrations;

    // Setup options.
    po::options_description desc("Options");
    desc.add_options()("VERSION.id", po::value<float>(&version),
                       "version identificator")("COMMENT.str", po::value<std::string>(&comment), "comment")(
        "CHECKER.isWriteWfm", po::value<bool>(&fdigiPars.isWriteWfm),
        "writing waveforms")("PARAMETERS.gateBegin", po::value<int>(&fdigiPars.gateBegin), "digi parameters")(
        "PARAMETERS.gateEnd", po::value<int>(&fdigiPars.gateEnd),
        "digi parameters")("PARAMETERS.threshold", po::value<float>(&fdigiPars.threshold), "digi parameters")(
        "PARAMETERS.signalType", po::value<int>(&fdigiPars.signalType),
        "digi parameters")("PARAMETERS.doInvert", po::value<bool>(&fdigiPars.doInvert), "digi parameters")(
        "FITPARAMETERS.isfit", po::value<bool>(&fdigiPars.isfit), "digi parameters")(
        "FITPARAMETERS.harmonic", po::value<std::vector<std::string>>(&harmonics)->multitoken(), "fit harmonics")(
        "CALIBRATION.calib", po::value<std::vector<std::string>>(&calibrations)->multitoken(), "calibrations");

    // Load config file.
    po::variables_map vm;
    std::ifstream calib_file((path + calibrationFile).Data(), std::ifstream::in);
    if (!calib_file.is_open()) {
        LOG(error) << Form("BmnScWallRaw2Digit : Loading Calibration from file: %s - file open error!",
                           calibrationFile.Data());
        return;
    }
    LOG(debug) << Form("BmnScWallRaw2Digit : Loading Calibration from file: %s", calibrationFile.Data());
    po::store(po::parse_config_file(calib_file, desc), vm);
    calib_file.close();
    po::notify(vm);

    for (auto str : harmonics) {
        if (str.find(',') != std::string::npos) {
            // Split the string into real and imaginary parts
            std::string real_part_str, imaginary_part_str;
            std::istringstream iss(str);
            std::getline(iss, real_part_str, ',');
            std::getline(iss, imaginary_part_str);

            // Convert the real and imaginary parts to floats
            float real_part = std::stof(real_part_str);
            float imaginary_part = std::stof(imaginary_part_str);

            // Set the value to the complex number
            fdigiPars.harmonics.push_back(std::complex<float>(real_part, imaginary_part));
        } else
            fdigiPars.harmonics.push_back(std::stof(str));
    }

    int cell_id;
    float calibration;
    float calibError;
    fuoCalibMap.clear();
    for (auto it : calibrations) {
        istringstream ss(it);
        ss >> cell_id >> calibration >> calibError;
        uint32_t address = BmnScWallAddress::GetAddress(cell_id);
        fuoCalibMap[address] = std::make_pair(calibration, calibError);
    }

    if (fdigiPars.isfit) {
        int model_order = fdigiPars.harmonics.size() + 1;
        fSignalLength = fdigiPars.gateEnd - fdigiPars.gateBegin + 1;
        auto maxElement = std::max_element(
            fdigiPars.harmonics.begin(), fdigiPars.harmonics.end(),
            [](const std::complex<float>& a, const std::complex<float>& b) { return std::abs(a) < std::abs(b); });
        if (maxElement != fdigiPars.harmonics.end()) {
            // set signal length as 5 * harmonic length
            int maxElementLength = floor(5. / (-log(real(*maxElement))));
            if (maxElementLength < fSignalLength)
                fSignalLength = maxElementLength;
        }

        fAZik = new std::complex<float>*[model_order];
        for (int i = 0; i < model_order; i++) {
            fAZik[i] = new std::complex<float>[model_order];
            for (int j = 0; j < model_order; j++)
                fAZik[i][j] = {0., 0.};
        }
        PsdSignalFitting::PronyFitter Pfitter;
        Pfitter.Initialize(fdigiPars.harmonics.size(), fdigiPars.harmonics.size(), fdigiPars.gateBegin,
                           fdigiPars.gateEnd);
        Pfitter.SetExternalHarmonics(fdigiPars.harmonics);
        Pfitter.MakeInvHarmoMatrix(fSignalLength, fAZik);
    }
}

std::optional<uint32_t> BmnScWallRaw2Digit::GetAddressFromBoard(std::pair<size_t, size_t> key)
{
    try {
        return fuoChannelMap.at(key);
    } catch (const std::out_of_range& e) {
        LOG(debug) << "BmnScWallRaw2Digit:: GetAddressFromBoard. Board serial " << std::hex << key.first << std::dec
                   << " channel " << key.second << " is not connected. Skipping it";
        return std::nullopt;
    }
}

std::optional<std::pair<float, float>> BmnScWallRaw2Digit::GetCalibPairFromAddress(uint32_t address)
{
    try {
        return fuoCalibMap.at(address);
    } catch (const std::out_of_range& e) {
        LOG(debug) << "BmnScWallRaw2Digit:: GetCalibPairFromAddress " << std::bitset<32>(address)
                   << " is marked badly in calibration file.";
        return std::nullopt;
    }
}
void BmnScWallRaw2Digit::fillEvent(TClonesArray* data, TClonesArray* ScWalldigit)
{

    LOG(debug) << "BmnScWallRaw2Digit::fillEvent";

    for (int i = 0; i < data->GetEntriesFast(); i++) {
        BmnADCDigit* digit = (BmnADCDigit*)data->At(i);
        auto key = std::make_pair(digit->GetSerial(), digit->GetChannel());
        auto catch_address = GetAddressFromBoard(key);
        if (!catch_address.has_value())
            continue;   // Not connected lines
        auto address = catch_address.value();
        if (address == 0)
            continue;   // Not connected lines

        auto catch_calib = GetCalibPairFromAddress(address);
        if (!catch_calib.has_value())
            continue;   // not involved in the analysis
        auto calib_pair = catch_calib.value();

        std::vector<float> wfm((short*)digit->GetUShortValue(), (short*)digit->GetUShortValue() + digit->GetNSamples());
        BmnScWallDigi ThisDigi;
        ThisDigi.reset();
        ThisDigi.SetAddress(address);
        ProcessWfm(wfm, &ThisDigi);

        // Apply calibration
        LOG(debug4) << "BmnScWallRaw2Digit::ProcessWfm  Calibration";
        if (fdigiPars.signalType == 0)
            ThisDigi.SetSignal((float)ThisDigi.fAmpl * calib_pair.first);
        if (fdigiPars.signalType == 1)
            ThisDigi.SetSignal((float)ThisDigi.fIntegral * calib_pair.first);
        if (ThisDigi.GetSignal() < fdigiPars.threshold)
            continue;

        TClonesArray& ar_ScWall = *ScWalldigit;
        new (ar_ScWall[ScWalldigit->GetEntriesFast()]) BmnScWallDigi(ThisDigi);
    }
}

BmnScWallRaw2Digit::~BmnScWallRaw2Digit() {}
