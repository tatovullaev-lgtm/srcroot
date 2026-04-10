#include "BmnHodoRaw2Digit.h"

#include "BmnADCDigit.h"
#include "BmnSyncDigit.h"
#include "BmnTDCDigit.h"
#include "BmnTQDCADCDigit.h"
#include "BmnTrigRaw2Digit.h"
#include "FairLogger.h"
#include "RLDeconvolutor.h"
#include "TMath.h"
#include "TSystem.h"

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <nlohmann/json.hpp>

void BmnHodoRaw2Digit::print()
{
    LOG(info) << Form("%s", GetName().Data());
}

BmnHodoRaw2Digit::BmnHodoRaw2Digit()
    : WfmProcessor()
    , fSaturation(std::numeric_limits<int16_t>::max())
    , fDoRLdeconvolve(false)
    , fRLthresold(0)
    , fR2converge(0.01)
    , fMaxIters(10)
    , fMaxPeaks(2)
    , fworkTime_real(0.0)
    , fworkTime_cpu(0.0)
{}

BmnHodoRaw2Digit::BmnHodoRaw2Digit(int period,
                                   int run,
                                   const std::string& mappingFile,
                                   const std::string& calibrationFile)
    : WfmProcessor()
    , fSaturation(std::numeric_limits<int16_t>::max())
    , fDoRLdeconvolve(false)
    , fRLthresold(0)
    , fR2converge(0.01)
    , fMaxIters(10)
    , fMaxPeaks(2)
    , fworkTime_real(0.0)
    , fworkTime_cpu(0.0)
{
    LOG(info) << Form("%s : Config file: %s", GetName().Data(), mappingFile.c_str());
    LOG(info) << Form("%s : Calibration file: %s", GetName().Data(), calibrationFile.c_str());
    ParseConfig(mappingFile);
    ParseCalibration(calibrationFile);
}

void BmnHodoRaw2Digit::ParseConfig(const std::string& file)
{
    std::string fullPath = std::string(getenv("VMCWORKDIR")) + "/input/" + file;
    std::ifstream config_file(fullPath);
    if (!config_file.is_open()) {
        LOG(error) << GetName() << "::ParseConfig: Cannot open config file: " << fullPath;
        return;
    }

    nlohmann::json j;
    try {
        config_file >> j;
    } catch (const std::exception& e) {
        LOG(error) << GetName() << "::ParseConfig: Failed to parse JSON: " << e.what();
        return;
    }

    LOG(debug) << GetName() << "::ParseConfig. Version: " << j.value("version", "unknown");
    LOG(debug) << GetName() << "::ParseConfig. Comment: " << j.value("comment", "none");

    fuoChannelMap.clear();
    for (const auto& entry : j["mapping"]) {
        std::string serial = entry["serial"];
        int channel = entry["channel"];
        int strip = entry["strip"];
        int side = entry["side"];
        int gain = entry["gain"];

        auto key = std::make_pair(std::stoul(serial, nullptr, 16), channel);
        fuoChannelMap[key] = BmnHodoAddress::GetAddress(strip, side, gain);
    }
}

void BmnHodoRaw2Digit::ParseCalibration(const std::string& file)
{
    std::string fullPath = std::string(getenv("VMCWORKDIR")) + "/parameters/hodo/" + file;
    std::ifstream config_file(fullPath);
    if (!config_file.is_open()) {
        LOG(error) << GetName() << "::ParseCalibration: Cannot open config file: " << fullPath;
        return;
    }

    nlohmann::json j;
    try {
        config_file >> j;
    } catch (const std::exception& e) {
        LOG(error) << GetName() << "::ParseCalibration: Failed to parse JSON: " << e.what();
        return;
    }

    LOG(debug) << GetName() << "::ParseCalibration. Version: " << j.value("version", "unknown");
    LOG(debug) << GetName() << "::ParseCalibration. Comment: " << j.value("comment", "none");

    const auto& par = j.at("parameters");

    fdigiPars.isWriteWfm = par.value("isWriteWfm", false);
    fdigiPars.gateBegin = par.at("gateBegin");
    fdigiPars.gateEnd = par.at("gateEnd");
    fdigiPars.threshold = par.at("threshold");
    fSaturation = par.at("saturation");
    fdigiPars.doInvert = par.value("doInvert", false);
    bool applyINL = par.value("applyINLcorr", true);
    if (applyINL)
        ParseINLcorrections();

    fuoCalibMap.clear();
    for (const auto& ch : j.at("calibration")) {
        int strip = ch.at("strip");
        int side = ch.at("side");
        int gain = ch.at("gain");
        float calib = ch.at("calib");
        float integ = ch.at("calibIntegral");

        fuoCalibMap[BmnHodoAddress::GetAddress(strip, side, gain)] = std::make_pair(calib, integ);
    }

    if (j.contains("deconvParameters")) {
        const auto& decpar = j.at("deconvParameters");
        fEthalon.clear();
        fDoRLdeconvolve = decpar.value("isdec", false);
        fRLthresold = decpar.value("threshold", 0);
        fR2converge = decpar.value("R2converge", 0.f);
        fMaxIters = decpar.value("iterations", 0);
        fMaxPeaks = decpar.value("peaks", 0);
        if (decpar.contains("ethalon") && decpar["ethalon"].is_array()) {
            auto tmp = decpar["ethalon"].get<std::vector<float>>();
            fEthalon.insert(fEthalon.end(), tmp.begin(), tmp.end());
        }
    }

    if (j.contains("fitParameters")) {
        const auto& fitpar = j.at("fitParameters");
        fdigiPars.isfit = fitpar.value("isfit", false);
        fdigiPars.harmonics.clear();
        for (const auto& pair : fitpar.at("harmonics")) {
            float re = pair.at(0).get<float>();
            float im = pair.at(1).get<float>();
            fdigiPars.harmonics.emplace_back(re, im);
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
                for (int k = 0; k < model_order; k++)
                    fAZik[i][k] = {0., 0.};
            }
            PsdSignalFitting::PronyFitter Pfitter;
            Pfitter.Initialize(fdigiPars.harmonics.size(), fdigiPars.harmonics.size(), fdigiPars.gateBegin,
                               fdigiPars.gateEnd);
            Pfitter.SetExternalHarmonics(fdigiPars.harmonics);
            Pfitter.MakeInvHarmoMatrix(fSignalLength, fAZik);
        }
    }
}

void BmnHodoRaw2Digit::ParseINLcorrections()
{
    std::string path = std::string(getenv("VMCWORKDIR")) + "/parameters/hodo/INLcorrections/";
    LOG(info) << Form("%s : Loading INL corrections from directory: %s", GetName().Data(), path.c_str());

    boost::property_tree::ptree pt;
    std::string header = "TQDC16VS";
    std::string iniheader = header + "_E";
    std::string postfix = "inl_corr";
    fINLcorrMap.clear();

    for (auto& it : fuoChannelMap) {
        auto boardSerial = (unsigned int)it.first.first;
        std::string corrFileName = Form("%s%s-%04X-%04X.ini", path.c_str(), header.c_str(),
                                        ((boardSerial & 0xffff0000) >> 16), (boardSerial & 0xffff));
        boost::property_tree::ini_parser::read_ini(corrFileName.c_str(), pt);
        std::string thisHeader = Form("%s-%08x-%s", iniheader.c_str(), boardSerial, postfix.c_str());
        for (UInt_t chan_iter = 0; chan_iter < TQDC16_CHANNEL_COUNT; chan_iter++) {
            auto correction_string = pt.get<std::string>(Form("%s.%d", thisHeader.c_str(), chan_iter));
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

std::optional<uint32_t> BmnHodoRaw2Digit::GetAddressFromBoard(std::pair<size_t, size_t> key)
{
    try {
        return fuoChannelMap.at(key);
    } catch (const std::out_of_range& e) {
        LOG(debug) << GetName() << ":: GetAddressFromBoard. Board serial " << std::hex << key.first << std::dec
                   << " channel " << key.second << " is not connected. Skipping it";
        return std::nullopt;
    }
}

uint32_t BmnHodoRaw2Digit::correctINL(uint32_t time, std::pair<size_t, size_t> key)
{
    try {
        return time + fINLcorrMap.at(key).at(time & 1023);
    } catch (const std::out_of_range& e) {
        LOG(warning) << GetName() << "::correctINL. Board serial " << std::hex << key.first << std::dec << " channel "
                     << key.second << " is not connected. Leave without INL correction";
        return time;
    }
}

std::optional<std::pair<float, float>> BmnHodoRaw2Digit::GetCalibPairFromAddress(uint32_t address)
{
    try {
        return fuoCalibMap.at(address);
    } catch (const std::out_of_range& e) {
        LOG(debug) << GetName() << ":: GetCalibPairFromAddress " << std::bitset<32>(address)
                   << " is marked badly in calibration file.";
        return std::nullopt;
    }
}

void BmnHodoRaw2Digit::fillEvent(TClonesArray* tdc, TClonesArray* adc, TClonesArray* Hododigit)
{

    LOG(debug) << GetName() << "::fillEvent";
    TStopwatch timer;
    timer.Start();

    for (int iAdc = 0; iAdc < adc->GetEntriesFast(); iAdc++) {
        BmnTQDCADCDigit* adcDig = (BmnTQDCADCDigit*)adc->At(iAdc);
        //    Double_t adcTimestamp = adcDig->GetAdcTimestamp() * ADC_CLOCK_TQDC16VS;
        //    Double_t trgTimestamp = adcDig->GetTrigTimestamp() * ADC_CLOCK_TQDC16VS;
        auto key = std::make_pair(adcDig->GetSerial(), adcDig->GetChannel());
        auto catch_address = GetAddressFromBoard(key);
        if (!catch_address.has_value())
            continue;   // Not connected lines
        auto address = catch_address.value();

        auto catch_calib = GetCalibPairFromAddress(address);
        if (!catch_calib.has_value())
            continue;   // Strip side gain is not involved in the analysis
        auto calib_pair = catch_calib.value();

        double time = -1.0;
        for (int iTdc = 0; iTdc < tdc->GetEntriesFast(); iTdc++) {
            BmnTDCDigit* tdcDig = (BmnTDCDigit*)tdc->At(iTdc);
            if (tdcDig->GetSerial() != adcDig->GetSerial() || tdcDig->GetSlot() != adcDig->GetSlot())
                continue;
            if (tdcDig->GetChannel() != adcDig->GetChannel())
                continue;
            time = correctINL(tdcDig->GetValue(), key);
            time *= TDC_CLOCK / TDC_BIN_COUNT;
        }

        std::vector<float> wfm(adcDig->GetShortValue(), adcDig->GetShortValue() + adcDig->GetNSamples());
        std::vector<BmnHodoDigi> digi_vector;
        BmnHodoDigi ThisDigi;   // whole wfm as one digi
        ThisDigi.reset();
        ThisDigi.SetAddress(address);
        ProcessWfm(wfm, &ThisDigi);   // invert, remove ZL, precalc as only one signal per waveform
        ThisDigi.SetSignal((float)ThisDigi.fAmpl * calib_pair.first);
        if (ThisDigi.GetSignal() < fdigiPars.threshold)
            continue;
        ThisDigi.fIntegral = (int)std::accumulate(wfm.begin(), wfm.end(), 0.0);   // common for all recovered signals
        ThisDigi.fSignalIntegral = (float)ThisDigi.fIntegral * calib_pair.second;
        if (ThisDigi.fSignalIntegral < fdigiPars.threshold)
            continue;
        ThisDigi.SetTime(time + ThisDigi.fTimeMax * ADC_CLOCK_TQDC16VS);
        if (ThisDigi.fAmpl + ThisDigi.fZL >= fSaturation)
            ThisDigi.SetIsSaturated(true);
        ThisDigi.fCrosstalk = *std::min_element(wfm.begin() + fdigiPars.gateBegin, wfm.begin() + fdigiPars.gateEnd + 1);
        digi_vector.push_back(ThisDigi);

        if (fDoRLdeconvolve && ThisDigi.fSignalIntegral > fRLthresold) {
            digi_vector.clear();

            LOG(debug4) << GetName() << "::ProcessWfm  RLDeconvolutor";
            RLDeconvolutor deconvolution(wfm, fEthalon, fR2converge, fMaxIters);
            /*bool converged = */ deconvolution.runDeconvolution();
            const auto& decomposition = deconvolution.getSignalDecomposition();
            const auto& positions = deconvolution.getSignalInfo();
            assert(decomposition.size() == positions.size());
            auto fitR2 = deconvolution.getR2().back();   // common for all recovered signals

            // Create a vector of indices
            std::vector<size_t> indices(positions.size());
            std::iota(indices.begin(), indices.end(), 0);

            // Sort the indices based on the corresponding amplitudes and extract the top fMaxPeaks indices
            std::sort(indices.begin(), indices.end(),
                      [&positions](size_t a, size_t b) { return positions[a].second > positions[b].second; });
            std::vector<size_t> topIndices = (fMaxPeaks <= indices.size())
                                                 ? std::vector<size_t>(indices.begin(), indices.begin() + fMaxPeaks)
                                                 : indices;

            // Apply calibration
            LOG(debug4) << GetName() << "::ProcessWfm  Selection";
            for (auto it : topIndices) {
                auto time_max = positions.at(it).first;
                auto ampl = positions.at(it).second;
                auto sub_wfm = decomposition.at(it);

                BmnHodoDigi SubDigi = ThisDigi;   // common base for all
                SubDigi.fTimeMax = time_max;
                SubDigi.fFitTimeMax = time_max;
                SubDigi.fFitAmpl = ampl;
                SubDigi.fFitR2 = fitR2;
                SubDigi.SetSignal(SubDigi.fFitAmpl * calib_pair.first);
                if (SubDigi.GetSignal() < fdigiPars.threshold)
                    continue;

                SubDigi.SetTime(time + SubDigi.fFitTimeMax * ADC_CLOCK_TQDC16VS);
                if (SubDigi.fFitAmpl + SubDigi.fZL >= fSaturation)
                    SubDigi.SetIsSaturated(true);
                SubDigi.fCrosstalk =
                    *std::min_element(sub_wfm.begin() + fdigiPars.gateBegin, sub_wfm.begin() + fdigiPars.gateEnd + 1);

                if (fdigiPars.isWriteWfm) {
                    SubDigi.fWfm = wfm;
                    SubDigi.fFitWfm = sub_wfm;
                    SubDigi.fR2history = deconvolution.getR2();
                }

                digi_vector.push_back(SubDigi);
            }
        }

        TClonesArray& ar_Hodo = *Hododigit;
        for (auto& digi : digi_vector) {
            digi.fNpeaks = digi_vector.size();
            new (ar_Hodo[Hododigit->GetEntriesFast()]) BmnHodoDigi(digi);
        }
    }

    timer.Stop();
    fworkTime_cpu += timer.RealTime();
    fworkTime_real += timer.CpuTime();
}

BmnHodoRaw2Digit::~BmnHodoRaw2Digit()
{
    LOG(info) << Form("%10s: Real time %4.3f s, CPU time %4.3f s", GetName().Data(), fworkTime_real, fworkTime_cpu);
}
