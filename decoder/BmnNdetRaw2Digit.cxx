#include "BmnNdetRaw2Digit.h"

#include "BmnTrigRaw2Digit.h"
#include "TMath.h"
#include "TSystem.h"

void BmnNdetRaw2Digit::print()
{
    LOG(info) << Form("BmnNdetRaw2Digit");
}

BmnNdetRaw2Digit::BmnNdetRaw2Digit()
    : WfmProcessor()
{
    fPeriodId = 0;
    fRunId = 0;
}

BmnNdetRaw2Digit::BmnNdetRaw2Digit(int period, int run, TString mappingFile, TString CalibrationFile)
    : WfmProcessor()
{
    LOG(info) << Form("BmnNdetRaw2Digit : Config file: %s", mappingFile.Data());
    LOG(info) << Form("BmnNdetRaw2Digit : Calibration file: %s", CalibrationFile.Data());
    fPeriodId = period;
    fRunId = run;
    fmappingFileName = mappingFile;
    ParseConfig(fmappingFileName);
    fcalibrationFileName = CalibrationFile;
    ParseCalibration(fcalibrationFileName);
    ParseINLcorrections();
}

void BmnNdetRaw2Digit::ParseConfig(TString mappingFile)
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
        LOG(error) << Form("BmnNdetRaw2Digit : Loading Config from file: %s - file open error!", mappingFile.Data());
        return;
    }
    LOG(debug) << Form("BmnNdetRaw2Digit : Loading Config from file: %s", mappingFile.Data());
    po::store(po::parse_config_file(config_file, desc), vm);
    config_file.close();
    po::notify(vm);

    std::string board_serial;
    int board_channel;
    int det_id, layer_id, row_id, column_id;

    fuoChannelMap.clear();
    for (auto it : configuration) {
        istringstream ss(it);
        ss >> board_serial >> board_channel >> det_id >> layer_id >> row_id >> column_id;
        LOGF(debug, "config  =	%s    %d    %d    %d    %d    %d", board_serial.c_str(), board_channel, 0, layer_id,
             row_id, column_id);
        assert(board_channel < 16);

        auto key = std::make_pair(std::stoul(board_serial, nullptr, 16), board_channel);
        fuoChannelMap[key] = BmnNdetAddress::GetAddress(det_id, row_id, column_id, layer_id);
    }
}

void BmnNdetRaw2Digit::ParseCalibration(TString calibrationFile)
{

    namespace po = boost::program_options;

    TString dir = getenv("VMCWORKDIR");
    TString path = dir + "/parameters/ndet/";

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
        "PARAMETERS.applyINLcorr", po::value<bool>(&fApplyINL),
        "digi parameters")("PARAMETERS.applyAmplCalib", po::value<bool>(&fApplyAmplCalib), "digi parameters")(
        "PARAMETERS.applyTimeCalib", po::value<bool>(&fApplyTimeCalib),
        "digi parameters")("FITPARAMETERS.isfit", po::value<bool>(&fdigiPars.isfit), "digi parameters")(
        "FITPARAMETERS.harmonic", po::value<std::vector<std::string>>(&harmonics)->multitoken(), "fit harmonics")(
        "CALIBRATION.calib", po::value<std::vector<std::string>>(&calibrations)->multitoken(), "calibrations");

    // Load config file.
    po::variables_map vm;
    std::ifstream calib_file((path + calibrationFile).Data(), std::ifstream::in);
    if (!calib_file.is_open()) {
        LOG(error) << Form("BmnNdetRaw2Digit : Loading Calibration from file: %s - file open error!",
                           calibrationFile.Data());
        return;
    }
    LOG(debug) << Form("BmnNdetRaw2Digit : Loading Calibration from file: %s", calibrationFile.Data());
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

    int det_id, layer_id, row_id, column_id;
    float calibration;
    float calibError;
    float calibSlewP1;
    float calibSlewP2;
    float calibSlewP3;
    float calibTimeShift;
    fuoCalibMap.clear();
    fuoCalibSlewShiftMap.clear();
    for (auto it : calibrations) {
        istringstream ss(it);
        ss >> det_id >> layer_id >> row_id >> column_id >> calibration >> calibError >> calibSlewP1 >> calibSlewP2
            >> calibSlewP3 >> calibTimeShift;

        uint32_t address = BmnNdetAddress::GetAddress(det_id, row_id, column_id, layer_id);
        fuoCalibMap[address] = std::make_pair(calibration, calibError);
        std::vector<float> time_calib_vect{calibSlewP1, calibSlewP2, calibSlewP3, calibTimeShift};
        fuoCalibSlewShiftMap[address] = time_calib_vect;
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

void BmnNdetRaw2Digit::ParseINLcorrections()
{
    if (!fApplyINL)
        return;

    TString dir = getenv("VMCWORKDIR");
    TString path = dir + "/parameters/ndet/INLcorrections/";
    LOG(info) << Form("BmnNdetRaw2Digit : Loading INL corrections from directory: %s", path.Data());

    boost::property_tree::ptree pt;
    TString header = "TQDC16VS_E";
    TString postfix = "inl_corr";
    fINLcorrMap.clear();

    for (auto& it : fuoChannelMap) {
        auto boardSerial = (unsigned int)it.first.first;
        TString corrFileName = Form("%s%s-%04X-%04X.ini", path.Data(), header.Data(),
                                    ((boardSerial & 0xffff0000) >> 16), (boardSerial & 0xffff));
        boost::property_tree::ini_parser::read_ini(corrFileName.Data(), pt);
        TString thisHeader = Form("%s-%08x-%s", header.Data(), boardSerial, postfix.Data());
        for (UInt_t chan_iter = 0; chan_iter < TQDC16_CHANNEL_COUNT; chan_iter++) {
            auto correction_string = pt.get<std::string>(Form("%s.%d", thisHeader.Data(), chan_iter));
            istringstream ss(correction_string);

            std::string token;
            std::vector<float> corrV;
            while (std::getline(ss, token, ','))
                corrV.push_back(stof(token));
            assert(corrV.size() == 1024);

            auto key = std::make_pair(boardSerial, chan_iter);
            fINLcorrMap[key] = corrV;
        }   // channels loop
    }   // serials loop
}

std::optional<uint32_t> BmnNdetRaw2Digit::GetAddressFromBoard(std::pair<size_t, size_t> key)
{
    try {
        return fuoChannelMap.at(key);
    } catch (const std::out_of_range& e) {
        LOG(debug) << "BmnNdetRaw2Digit:: GetAddressFromBoard. Board serial " << std::hex << key.first << std::dec
                   << " channel " << key.second << " is not connected. Skipping it";
        return std::nullopt;
    }
}

std::optional<std::pair<float, float>> BmnNdetRaw2Digit::GetCalibPairFromAddress(uint32_t address)
{
    try {
        return fuoCalibMap.at(address);
    } catch (const std::out_of_range& e) {
        LOG(debug) << "BmnNdetRaw2Digit:: GetCalibPairFromAddress " << std::bitset<32>(address)
                   << " is marked badly in calibration file.";
        return std::nullopt;
    }
}

uint32_t BmnNdetRaw2Digit::correctINL(uint32_t time, std::pair<size_t, size_t> key)
{
    try {
        return time + fINLcorrMap.at(key).at(time & 1023);
    } catch (const std::out_of_range& e) {
        LOG(warning) << "BmnNdetRaw2Digit::correctINL. Board serial " << std::hex << key.first << std::dec
                     << " channel " << key.second << " is not connected. Leave without INL correction";
        return time;
    }
}

std::vector<float> BmnNdetRaw2Digit::GetCalibSlewShiftFromAddress(uint32_t address)
{
    if (fuoCalibSlewShiftMap.find(address) != fuoCalibSlewShiftMap.end())
        return fuoCalibSlewShiftMap.at(address);

    return {0.0, 0.0, 0.0, 0.0};
}

void BmnNdetRaw2Digit::fillEvent(TClonesArray* tdc,
                                 TClonesArray* adc,
                                 unordered_map<UInt_t, Long64_t>* mapTS,
                                 TClonesArray* Ndetdigit)
{

    LOG(debug) << "BmnNdetRaw2Digit::fillEvent";
    // double trigTime = findTriggerTime(tdc);
    // time is more important, it goes first
    for (Int_t iTdc = 0; iTdc < tdc->GetEntriesFast(); iTdc++) {
        BmnTDCDigit* tdcDig = (BmnTDCDigit*)tdc->At(iTdc);
        auto key = std::make_pair(tdcDig->GetSerial(), tdcDig->GetChannel());
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

        UInt_t ttvxs_ndet = 0x0CD9B727;
        unordered_map<UInt_t, Long64_t>::iterator itTS = mapTS->find(ttvxs_ndet);
        if (itTS == mapTS->end()) {
            continue;
        }
        double TimeShift = itTS->second;
        double time = (fApplyINL) ? correctINL(tdcDig->GetValue(), key) : tdcDig->GetValue();
        time *= 0.024 / 1.024;
        time += TimeShift;

        BmnNdetDigi ThisDigi;
        ThisDigi.reset();
        ThisDigi.SetAddress(address);
        ThisDigi.SetTime(time);
        LOG(debug) << "BmnNdetRaw2Digit::Switch to adc digit" << endl;
        for (int iAdc = 0; iAdc < adc->GetEntriesFast(); iAdc++) {
            BmnTQDCADCDigit* adcDig = (BmnTQDCADCDigit*)adc->At(iAdc);
            if (tdcDig->GetSerial() != adcDig->GetSerial() || tdcDig->GetSlot() != adcDig->GetSlot())
                continue;
            if (tdcDig->GetChannel() != adcDig->GetChannel())
                continue;

            std::vector<float> wfm(adcDig->GetShortValue(), adcDig->GetShortValue() + adcDig->GetNSamples());
            ProcessWfm(wfm, &ThisDigi);

            // Apply calibration
            LOG(debug) << "BmnNdetRaw2Digit::ProcessWfm  Calibration" << endl;
            if (fApplyAmplCalib == true) {
                if (fdigiPars.signalType == 0)
                    ThisDigi.SetSignal(ThisDigi.fAmpl * calib_pair.first);
                if (fdigiPars.signalType == 1)
                    ThisDigi.SetSignal(ThisDigi.fIntegral * calib_pair.first);
            } else {
                if (fdigiPars.signalType == 0)
                    ThisDigi.SetSignal(ThisDigi.fAmpl);
                if (fdigiPars.signalType == 1)
                    ThisDigi.SetSignal(ThisDigi.fIntegral);
            }
            if (fApplyTimeCalib == true) {
                auto calib_slew_shift = GetCalibSlewShiftFromAddress(ThisDigi.GetAddress());
                time = time
                       - ((calib_slew_shift.at(0)) / sqrt(ThisDigi.GetSignal() + calib_slew_shift.at(1))
                          + calib_slew_shift.at(2))
                       - calib_slew_shift.at(3);
                ThisDigi.SetTime(time);
            } else {
                ThisDigi.SetTime(time);
            }
        }

        if (ThisDigi.GetSignal() < fdigiPars.threshold)
            continue;

        TClonesArray& ar_Ndet = *Ndetdigit;
        new (ar_Ndet[Ndetdigit->GetEntriesFast()]) BmnNdetDigi(ThisDigi);
    }
}

BmnNdetRaw2Digit::~BmnNdetRaw2Digit() {}
