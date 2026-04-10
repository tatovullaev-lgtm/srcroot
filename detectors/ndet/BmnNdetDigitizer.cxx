/*************************************************************************************
 *            BmnNdetDigitizer
 *    Class to create digital data taken from BmnNdet detector
 *  Author:   Elena Litvinenko
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  18-11-2015
 *  Modified by M.Golubeva July 2022
 ************************************************************************************/

#include "BmnNdetDigitizer.h"

#include "BmnNdetCell.h"
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "TVirtualMC.h"

#include <TRandom3.h>

BmnNdetDigitizer::BmnNdetDigitizer(const char* name)
    : FairTask(name)
    , fTofCut(100.)
    , fThreshold(0.0025)
    , fSiPM()
    , fPointArray(nullptr)
    , fDigiArray(nullptr)
{}

BmnNdetDigitizer::~BmnNdetDigitizer()
{
    if (fDigiArray) {
        fDigiArray->Delete();
        delete fDigiArray;
    }
}

InitStatus BmnNdetDigitizer::Init()
{
    LOG(detail) << "-I- BmnNdetDigitizer: Init started..." << std::endl;

    // Get RootManager
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman) {
        LOG(error) << "-E- BmnNdetDigitizer::Init: RootManager not instantiated!" << std::endl;
        return kFATAL;
    }

    // Get input array
    fPointArray = static_cast<TClonesArray*>(ioman->GetObject("NdetPoint"));
    if (!fPointArray) {
        LOG(error) << "-W- BmnNdetDigitizer::Init: No NdetPoint array!" << std::endl;
        return kERROR;
    }

    // Create and register output array
    fDigiArray = new TClonesArray("BmnNdetDigit");
    ioman->Register("NdetDigit", "Ndet", fDigiArray, kTRUE);

    LOG(detail) << "-I- BmnNdetDigitizer: Initialization successful" << std::endl;
    return kSUCCESS;
}

void BmnNdetDigitizer::Exec(Option_t* opt)
{
    LOG(debug) << "BmnNdetDigitizer::Exec() started..." << std::endl;
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
        for (auto* point : tracks_contributions) {
            if (FairLogger::GetLogger()->IsLogNeeded(fair::Severity::debug))
                point->Print();

            if (point->GetTime() > fTofCut)
                continue;
            double sipmEnergy = fSiPM.ModelSiPM(point->GetEnergyLoss());
            LOG(debug) << "Modeled SiPM energy " << sipmEnergy;
            if (sipmEnergy < 0.)
                continue;
            totalEnergy += sipmEnergy;

            if (!reachedThreshold && totalEnergy > fThreshold) {
                crossTime = point->GetTime();
                reachedThreshold = true;
                point->AddLink(FairLink(static_cast<int>(BmnNdetCell::LinkType::SurfaceMCPointCrossThr),
                                        point->GetLinksWithType(static_cast<int>(BmnNdetCell::LinkType::SurfaceMCPoint))
                                            .GetLink(0)
                                            .GetIndex(),
                                        sipmEnergy));
                LOG(debug) << Form("Reached threshold. Energy %.4f Weighted time %.4f", totalEnergy, crossTime);
            }
        }

        if (totalEnergy < fThreshold) {
            LOG(debug) << "BmnNdetDigitizer::Skip Digit " << BmnNdetAddress::GetInfoString(address).c_str();
            continue;
        } else {
            TClonesArray& ar = *fDigiArray;
            long entries = fDigiArray->GetEntriesFast();
            new (ar[entries]) BmnNdetDigit(address, crossTime, totalEnergy);
            if (FairLogger::GetLogger()->IsLogNeeded(fair::Severity::debug))
                static_cast<BmnNdetDigit*>(ar.At(entries))->Print();
        }
    }
}

void BmnNdetDigitizer::FillHitMap()
{
    fuoHitMap.clear();

    // Fill the hit map with points from the TClonesArray
    for (int iPoint = 0; iPoint < fPointArray->GetEntriesFast(); ++iPoint) {
        auto* point = static_cast<BmnNdetPoint*>(fPointArray->At(iPoint));
        uint32_t address = point->GetAddress();
        fuoHitMap[address].push_back(point);
    }

    // Sort each vector of points by their time
    for (auto& pair : fuoHitMap) {
        std::vector<BmnNdetPoint*>& points = pair.second;
        std::sort(points.begin(), points.end(),
                  [](BmnNdetPoint* a, BmnNdetPoint* b) { return a->GetTime() < b->GetTime(); });
    }

    LOG(debug) << "BmnNdetDigitizer::FillHitMap() event " << gMC->CurrentEvent() << " fired cells " << fuoHitMap.size();
}
