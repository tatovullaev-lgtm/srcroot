#include "BmnHodoDigitizer.h"

#include "BmnHodoAddress.h"
#include "FairRootManager.h"
#include "TMath.h"
#include "TStopwatch.h"

#include <FairRun.h>
#include <FairRunSim.h>
#include <nlohmann/json.hpp>

// Constructor
BmnHodoDigitizer::BmnHodoDigitizer(const char* name)
    : FairTask(name)
    , fScale(1.0)   // Initialize scale to 1.0
    , fThreshold(0.0)
    , fSaturation(std::numeric_limits<double>::max())
    , fPointArray(nullptr)
    , fDigiArray(nullptr)
{}

// Destructor
BmnHodoDigitizer::~BmnHodoDigitizer()
{
    if (fDigiArray) {
        fDigiArray->Delete();
        delete fDigiArray;
    }
}

// Initialization method
InitStatus BmnHodoDigitizer::Init()
{
    fworkTime = 0.0;
    LOG(detail) << "-I- BmnHodoDigitizer: Init started..." << std::endl;

    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman) {
        LOG(error) << "-E- BmnHodoDigitizer::Init: RootManager not instantiated!" << std::endl;
        return kFATAL;
    }

    fPointArray = static_cast<TClonesArray*>(ioman->GetObject("HodoPoint"));
    if (!fPointArray) {
        LOG(error) << "-W- BmnHodoDigitizer::Init: No HodoPoint array!" << std::endl;
        return kERROR;
    }

    fDigiArray = new TClonesArray("BmnHodoDigit");
    ioman->Register("HodoDigit", "Hodo", fDigiArray, kTRUE);

    LOG(detail) << "-I- BmnHodoDigitizer: Initialization successful" << std::endl;
    return kSUCCESS;
}

// Setter for energy resolution configuration
void BmnHodoDigitizer::SetConfig(const std::string& config)
{
    return ParseConfig(config);
}

void BmnHodoDigitizer::ParseConfig(const std::string& inputFile)
{
    std::string fullPath = std::string(getenv("VMCWORKDIR")) + "/parameters/hodo/" + inputFile;
    std::ifstream input(fullPath);
    if (!input.is_open()) {
        LOG(error) << "BmnHodoDigitizer::ParseConfig: Cannot open file " << fullPath;
        return;
    }

    nlohmann::json j;
    try {
        input >> j;
    } catch (const std::exception& e) {
        LOG(error) << "BmnHodoDigitizer::ParseConfig: Failed to parse JSON: " << e.what();
        return;
    }

    std::string version = j.value("version", "unknown");
    std::string comment = j.value("comment", "none");

    LOG(debug) << "BmnHodoDigitizer::ParseConfig. Version: " << version;
    LOG(debug) << "BmnHodoDigitizer::ParseConfig. Comment: " << comment;

    fSaturation = j.at("parameters").at("saturation");

    fuoResolutionMap.clear();
    for (const auto& entry : j["calibration"]) {
        int strip = entry["strip"];
        int side = entry["side"];
        int gain = entry["gain"];
        double calib = entry["calib"];
        double stochastic = entry["stochastic"];
        double constant = entry["constant"];

        uint32_t address = BmnHodoAddress::GetAddress(strip, side, gain);
        Kernel resolution(stochastic, constant);
        fuoResolutionMap.emplace(address, std::make_pair(calib, resolution));
    }
}

// Execute method (digitization process)
void BmnHodoDigitizer::Exec(Option_t* opt)
{
    if (!IsActive())
        return;
    TStopwatch sw;
    sw.Start();

    fDigiArray->Clear();
    FillHitMap();

    for (const auto& it : fuoHitMap) {
        uint32_t strip = it.first;
        const auto& points = it.second;

        double energyLoss = 0.0;
        double time = 0.0;
        for (const auto* point : points) {
            if (FairLogger::GetLogger()->IsLogNeeded(fair::Severity::debug))
                point->Print("");
            energyLoss += point->GetEnergyLoss();
            time += point->GetTime();
        }
        time /= points.size();

        if (fuoResolutionMap.empty()) {
            uint32_t address = BmnHodoAddress::GetAddress(strip, 0, 0);
            double z2 = energyLoss * fScale;
            LOG(debug) << "BmnHodoDigitizer::Exec Empty resoultion map. Smoothed energy: " << z2;

            if (z2 < fThreshold)
                continue;
            else {
                TClonesArray& ar = *fDigiArray;
                new (ar[ar.GetEntriesFast()]) BmnHodoDigit(address, time, z2);
            }
        } else {
            for (const auto& jt : fuoResolutionMap) {
                uint32_t address = jt.first;
                if (BmnHodoAddress::GetStripId(address) != strip)
                    continue;
                double z2 = jt.second.second(energyLoss * fScale);
                double adc = z2 / jt.second.first;
                LOG(debug) << Form(
                    "BmnHodoDigitizer::Exec Resoultion map for strid %d side %d gain %d. Smoothed energy: %f",
                    BmnHodoAddress::GetStripId(address), BmnHodoAddress::GetStripSide(address),
                    BmnHodoAddress::GetGain(address), z2);

                if (z2 < fThreshold)
                    continue;
                else {
                    TClonesArray& ar = *fDigiArray;
                    BmnHodoDigit* digit = new (ar[ar.GetEntriesFast()]) BmnHodoDigit(address, time, z2);
                    if (adc >= fSaturation) {
                        digit->SetSignal(fSaturation * jt.second.first);
                        digit->SetIsSaturated(true);
                    }
                }
            }
        }
    }

    sw.Stop();
    fworkTime += sw.RealTime();
}

// Fill the hit map with points
void BmnHodoDigitizer::FillHitMap()
{
    fuoHitMap.clear();

    for (int i = 0; i < fPointArray->GetEntriesFast(); ++i) {
        auto* point = static_cast<BmnHodoPoint*>(fPointArray->At(i));
        int iStrip = point->GetCopyMother();
        fuoHitMap[iStrip].emplace_back(point);
    }

    LOG(debug) << "BmnHodoDigitizer::FillHitMap() - Number of fired cells: " << fuoHitMap.size();
}

// Finish method
void BmnHodoDigitizer::Finish()
{
    LOG(info) << "Work time of the HODO digitizer: " << fworkTime << " sec." << std::endl;
}
