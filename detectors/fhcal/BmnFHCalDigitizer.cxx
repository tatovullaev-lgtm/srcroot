#include "BmnFHCalDigitizer.h"

#include "BmnFHCalAddress.h"
#include "FairRootManager.h"
#include "TStopwatch.h"
#include "TVirtualMC.h"

#include <FairRun.h>
#include <FairRunSim.h>

// Constructor
BmnFHCalDigitizer::BmnFHCalDigitizer(const char* name)
    : FairTask(name)
    , fScale(1.0)
    , fThreshold(0.0)
    , fTimeCut(1000.0)
    , fSiPM()
    , fPointArray(nullptr)
    , fDigiArray(nullptr)
{}

// Destructor
BmnFHCalDigitizer::~BmnFHCalDigitizer()
{
    if (fDigiArray) {
        fDigiArray->Delete();
        delete fDigiArray;
    }
}

// Initialization method
InitStatus BmnFHCalDigitizer::Init()
{
    fworkTime = 0.0;
    LOG(detail) << "-I- BmnFHCalDigitizer: Init started..." << std::endl;

    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman) {
        LOG(error) << "-E- BmnFHCalDigitizer::Init: RootManager not instantiated!" << std::endl;
        return kFATAL;
    }

    fPointArray = static_cast<TClonesArray*>(ioman->GetObject("FHCalPoint"));
    if (!fPointArray) {
        LOG(error) << "-W- BmnFHCalDigitizer::Init: No FHCalPoint array!" << std::endl;
        return kERROR;
    }

    fDigiArray = new TClonesArray("BmnFHCalDigit");
    ioman->Register("FHCalDigit", "FHCal", fDigiArray, kTRUE);

    LOG(detail) << "-I- BmnFHCalDigitizer: Initialization successful" << std::endl;
    return kSUCCESS;
}

void BmnFHCalDigitizer::Exec(Option_t* opt)
{
    if (!IsActive())
        return;
    TStopwatch sw;
    sw.Start();

    LOG(debug2) << "BmnFHCalDigitizer::Exec() started..." << std::endl;
    if (!fDigiArray)
        Fatal("Exec", "No DigiArray");

    fDigiArray->Clear();
    FillHitMap();   // One address (key) is now associated with a sorted vector of tracks which fired it

    for (auto& it : fuoHitMap) {
        uint32_t address = it.first;
        const auto& tracks_contributions = it.second;

        double totalEnergy = 0.0;
        double crossTime = 0.0;   // time when threshold is crossed
        bool reachedThreshold = false;
        for (const auto* point : tracks_contributions) {
            if (point->GetTime() > fTimeCut)
                break;
            if (FairLogger::GetLogger()->IsLogNeeded(fair::Severity::debug2))
                point->Print();

            totalEnergy += point->GetEnergyLoss();
            if (!reachedThreshold && totalEnergy * fScale > fThreshold) {
                crossTime = point->GetTime();
                reachedThreshold = true;
                LOG(debug2) << Form("Reached threshold. Energy %.4f Weighted time %.4f", totalEnergy, crossTime);
            }
        }
        totalEnergy = fSiPM.ModelResponse(totalEnergy);
        totalEnergy *= fScale;

        if (!reachedThreshold || totalEnergy < std::numeric_limits<double>::epsilon()) {
            LOG(debug2) << "BmnFHCalDigitizer::Skip Digit " << BmnFHCalAddress::GetInfoString(address).c_str();
            continue;
        } else {
            TClonesArray& ar = *fDigiArray;
            long entries = fDigiArray->GetEntriesFast();
            new (ar[entries]) BmnFHCalDigit(address, crossTime, totalEnergy);
            if (FairLogger::GetLogger()->IsLogNeeded(fair::Severity::debug2))
                static_cast<BmnFHCalDigit*>(ar.At(entries))->Print();
        }
    }

    sw.Stop();
    fworkTime += sw.RealTime();
}

void BmnFHCalDigitizer::FillHitMap()
{
    fuoHitMap.clear();

    // Fill the hit map with points from the TClonesArray
    for (int iPoint = 0; iPoint < fPointArray->GetEntriesFast(); ++iPoint) {
        auto* point = static_cast<BmnFHCalPoint*>(fPointArray->At(iPoint));
        uint32_t address = BmnFHCalAddress::GetPhysicalAddress(point->GetAddress());
        fuoHitMap[address].push_back(point);
    }

    // Sort each vector of points by their time
    for (auto& pair : fuoHitMap) {
        std::vector<BmnFHCalPoint*>& points = pair.second;
        std::sort(points.begin(), points.end(),
                  [](BmnFHCalPoint* a, BmnFHCalPoint* b) { return a->GetTime() < b->GetTime(); });
    }

    LOG(debug2) << "BmnFHCalDigitizer::FillHitMap() event " << gMC->CurrentEvent() << " fired sections "
                << fuoHitMap.size();
}

void BmnFHCalDigitizer::Finish()
{
    printf("Work time of the FHCal digitizer: %4.4f sec.\n", fworkTime);
}
