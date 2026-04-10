#include "BmnFHCal.h"

#include "CbmStack.h"
#include "FairLinkManager.h"
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairVolume.h"
#include "TGeoManager.h"
#include "TLorentzVector.h"
#include "TParticle.h"
#include "TVirtualMC.h"

#include <cassert>
#include <iostream>

BmnFHCal::BmnFHCal()
    : FairDetector("BmnFHCal", true)
    , fCollection(new TClonesArray("BmnFHCalPoint"))
    , fGeoHandler(std::make_unique<BmnFHCalGeo>())
    , fPoint(nullptr)
{
    LOG(debug3) << "BmnFHCal default constructor called";
}

BmnFHCal::BmnFHCal(const char* name, Bool_t active)
    : FairDetector(name, active)
    , fCollection(new TClonesArray("BmnFHCalPoint"))
    , fGeoHandler(std::make_unique<BmnFHCalGeo>())
    , fPoint(nullptr)
{
    LOG(debug3) << "BmnFHCal constructor called with name: " << name << " and active: " << active;
}

BmnFHCal::~BmnFHCal()
{
    LOG(debug3) << "BmnFHCal destructor called";
    if (fCollection) {
        fCollection->Delete();
        delete fCollection;
        fCollection = nullptr;
    }

    fMultiLinkMap.clear();
    fGeoHandler.reset();
    fPoint.reset();
}

void BmnFHCal::Initialize()
{
    LOG(debug3) << "BmnFHCal Initialize method called";
    FairDetector::Initialize();
}

void BmnFHCal::BeginEvent()
{
    LOG(debug3) << "BmnFHCal BeginEvent method called";
    fMultiLinkMap.clear();
}

BmnFHCalPoint* BmnFHCal::GetHit(uint32_t address)
{
    LOG(debug4) << "BmnFHCal GetHit method called";
    Int_t nEntries = fCollection->GetEntriesFast();
    for (Int_t i = nEntries - 1; i >= 0; i--) {
        auto* hit = static_cast<BmnFHCalPoint*>(fCollection->At(i));
        if (hit->GetAddress() == address) {
            return hit;
        }
    }
    return nullptr;
}

static double CalculateEnergyLoss(double step, double edep)
{
    constexpr double BirkConst = 12.6;   // Static constant.
    return (step > 0) ? edep / (1.0 + (BirkConst / step) * edep) : edep;
}

Bool_t BmnFHCal::ProcessHits(FairVolume* vol)
{
    LOG(debug4) << "BmnFHCal ProcessHits method called for event " << gMC->CurrentEvent();

    // Skip non-sensitive volumes or invalid energies
    if (!CheckIfSensitive(gMC->CurrentVolName()) || gMC->Edep() < 0) {
        return kFALSE;
    }

    LOG(debug4) << "BmnFHCal::ProcessHits. trackID = " << gMC->GetStack()->GetCurrentTrackNumber() << " "
                << BmnFHCalAddress::GetInfoString(fGeoHandler->GetAddressFromPath(gMC->CurrentVolPath()))
                << ". Edep = " << gMC->Edep() << " Time = " << gMC->TrackTime() * 1.0e09;

    if (gMC->IsTrackEntering() || !fPoint) {
        fPoint.reset(new BmnFHCalPoint());
        fPoint->SetEventID(gMC->CurrentEvent());
        fPoint->SetAddress(fGeoHandler->GetAddressFromPath(gMC->CurrentVolPath()));
        TLorentzVector tPos, tMom;
        gMC->TrackPosition(tPos);
        gMC->TrackMomentum(tMom);
        fPoint->SetPosition(tPos.Vect());
        fPoint->SetMomentum(tMom.Vect());
        fPoint->SetEnergyLoss(CalculateEnergyLoss(gMC->TrackStep(), gMC->Edep()));
        fPoint->SetTime(gMC->TrackTime() * 1.0e09);
        fPoint->SetLength(gMC->TrackLength());
    }

    assert(fPoint);
    assert(fPoint->GetAddress() == fGeoHandler->GetAddressFromPath(gMC->CurrentVolPath()));
    double dEdep = CalculateEnergyLoss(gMC->TrackStep(), gMC->Edep());
    double weighted_time = fPoint->GetEnergyLoss() * fPoint->GetTime() + dEdep * gMC->TrackTime() * 1.0e09;
    fPoint->SetEnergyLoss(fPoint->GetEnergyLoss() + dEdep);
    if (fPoint->GetEnergyLoss() > 0.)
        fPoint->SetTime(weighted_time / fPoint->GetEnergyLoss());

    if (gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared()) {
        if (fPoint->GetEnergyLoss() > std::numeric_limits<double>::epsilon()) {
            int surface_track = GetSurfaceMCTrack(gMC->GetStack()->GetCurrentTrackNumber());
            if (surface_track >= 0) {
                fPoint->SetTrackID(surface_track);
                fMultiLinkMap[fPoint->GetAddress()].AddLink(
                    FairLink("MCTrack", surface_track, fPoint->GetEnergyLoss()));
                ((CbmStack*)gMC->GetStack())->AddPoint(kFHCAL, surface_track);
            }
            auto* existing = GetHit(fPoint->GetAddress());
            if (existing)
                UpdateHit(*existing, *fPoint);
            else
                AddHit(fPoint.get());
        }
        fPoint.reset();
    }

    return kTRUE;
}

void BmnFHCal::EndOfEvent()
{
    LOG(debug3) << "BmnFHCal EndOfEvent method called";
    Print("");
    Reset();
}

void BmnFHCal::Register()
{
    LOG(debug3) << "BmnFHCal Register method called";
    FairRootManager::Instance()->Register("FHCalPoint", "FHCal", fCollection, kTRUE);
}

TClonesArray* BmnFHCal::GetCollection(Int_t iColl) const
{
    LOG(debug3) << "BmnFHCal GetCollection method called with iColl: " << iColl;
    for (Int_t i = 0; i < fCollection->GetEntriesFast(); i++) {
        auto* hit = static_cast<BmnFHCalPoint*>(fCollection->At(i));
        hit->SetEntryNr(FairLink("FHCalPoint", i, hit->GetEnergyLoss()));
        uint32_t address = hit->GetAddress();
        if (fMultiLinkMap.find(address) != fMultiLinkMap.end()) {
            hit->SetLinks(fMultiLinkMap.at(address));   // Overwrite
            LOG(debug3) << "BmnFHCal::GetCollection Links: " << hit->GetNLinks();
            if (FairLogger::GetLogger()->IsLogNeeded(fair::Severity::debug3) && hit->GetNLinks()) {
                hit->PrintLinkInfo();
                printf("\n");
            }
        }
    }
    return (iColl == 0) ? fCollection : nullptr;
}

void BmnFHCal::Print(Option_t*) const
{
    LOG(debug3) << "BmnFHCal Print method called";
    Int_t nHits = fCollection->GetEntriesFast();
    std::cout << "-I- BmnFHCal: " << nHits << " points registered in event " << gMC->CurrentEvent() << std::endl;

    if (FairLogger::GetLogger()->IsLogNeeded(fair::Severity::debug4)) {
        for (Int_t i = 0; i < nHits; ++i) {
            auto hit = static_cast<BmnFHCalPoint*>((*fCollection)[i]);
            hit->Print();
        }
    }
}

void BmnFHCal::Reset()
{
    LOG(debug3) << "BmnFHCal Reset method called";
    fCollection->Delete();
}

void BmnFHCal::ConstructGeometry()
{
    TString fileName = GetGeometryFileName();
    if (fileName.EndsWith(".root")) {
        LOG(info) << "Constructing FHCal geometry from ROOT file %s" << fileName.Data();
        ConstructRootGeometry();
    }

    FairGeoLoader* geoLoad = FairGeoLoader::Instance();
    FairGeoInterface* geoFace = geoLoad->getGeoInterface();
    fGeoHandler->setGeomFile(GetGeometryFileName());
    geoFace->addGeoModule(fGeoHandler.get());

    Bool_t rc = geoFace->readSet(fGeoHandler.get());
    if (rc)
        fGeoHandler->create(geoLoad->getGeoBuilder());
    TList* volList = fGeoHandler->getListOfVolumes();

    // store geo parameter
    FairRun* fRun = FairRun::Instance();
    FairRuntimeDb* rtdb = FairRun::Instance()->GetRuntimeDb();
    BmnFHCalGeoPar* par = (BmnFHCalGeoPar*)(rtdb->getContainer("BmnFHCalGeoPar"));
    TObjArray* fSensNodes = par->GetGeoSensitiveNodes();
    TObjArray* fPassNodes = par->GetGeoPassiveNodes();

    TListIter iter(volList);
    FairGeoNode* node = NULL;
    FairGeoVolume* aVol = NULL;
    while ((node = (FairGeoNode*)iter.Next())) {
        aVol = dynamic_cast<FairGeoVolume*>(node);
        if (node->isSensitive()) {
            fSensNodes->AddLast(aVol);
        } else {
            fPassNodes->AddLast(aVol);
        }
    }
    par->setChanged();
    par->setInputVersion(fRun->GetRunId(), 1);

    ProcessNodes(volList);
}

Bool_t BmnFHCal::CheckIfSensitive(std::string name)
{
    LOG(debug4) << "BmnFHCal CheckIfSensitive method called with name: " << name;
    TString tsname = name.c_str();
    return tsname.Contains(BmnFHCalGeoPar::SensitiveVolume_name);
}

BmnFHCalPoint* BmnFHCal::AddHit(BmnFHCalPoint* point)
{
    TClonesArray& clref = *fCollection;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size]) BmnFHCalPoint(*point);
}

void BmnFHCal::UpdateHit(BmnFHCalPoint& existing, const BmnFHCalPoint& update)
{
    LOG(debug4) << "UpdateHit: Updating hit at address " << existing.GetAddress()
                << " with current E_loss = " << existing.GetEnergyLoss()
                << " update E_loss += " << update.GetEnergyLoss();

    const double eloss_existing = existing.GetEnergyLoss();
    const double eloss_update = update.GetEnergyLoss();
    const double eloss_total = eloss_existing + eloss_update;

    if (eloss_total == 0)
        return;

    const double weight_existing = eloss_existing / eloss_total;
    const double weight_update = eloss_update / eloss_total;

    TVector3 pos_existing, mom_existing;
    existing.Position(pos_existing);
    existing.Momentum(mom_existing);

    TVector3 pos_update, mom_update;
    update.Position(pos_update);
    update.Momentum(mom_update);

    existing.SetPosition(weight_existing * pos_existing + weight_update * pos_update);
    existing.SetMomentum(weight_existing * mom_existing + weight_update * mom_update);
    existing.SetTime(weight_existing * existing.GetTime() + weight_update * update.GetTime());
    existing.SetLength(weight_existing * existing.GetLength() + weight_update * update.GetLength());
    if (weight_update > weight_existing)
        existing.SetTrackID(update.GetTrackID());
    existing.SetEnergyLoss(eloss_total);
}

int BmnFHCal::GetSurfaceMCTrack(int start_track_id)
{
    int current_id = start_track_id;
    TParticle* current = ((CbmStack*)gMC->GetStack())->GetParticle(current_id);
    while (current) {
        if (current->IsPrimary()) {
            LOG(debug4) << Form("BmnFHCal::GetSurfaceMCTrack. Current particle %d is primary", current_id);
            return current_id;
        }
        int mother_id = current->GetFirstMother();
        TParticle* mother = ((CbmStack*)gMC->GetStack())->GetParticle(mother_id);
        if (!mother) {
            LOG(error) << Form("BmnFHCal::GetSurfaceMCTrack. Current track with id %d has mother but it is "
                               "not stored in MC stack",
                               current_id);
            return -1;
        }
        if (mother_id == current_id) {
            LOG(error) << "BmnFHCal::GetSurfaceMCTrack. Infinite loop detected for track ID: " << current_id;
            return -1;
        }

        TVector3 current_vtx(current->Vx(), current->Vy(), current->Vz());
        TVector3 mother_vtx(mother->Vx(), mother->Vy(), mother->Vz());
        if (fGeoHandler->IsPointInside(current_vtx) && !fGeoHandler->IsPointInside(mother_vtx))
        {   // daughter vtx is inside detector, mother vtx is not
            LOG(debug4) << Form("BmnFHCal::GetSurfaceMCTrack. Current track with id %d has vtx (%.2f %.2f "
                                "%.2f) inside detector. Its mother %d vtx (%.2f %.2f %.2f) is outside",
                                current_id, current_vtx.X(), current_vtx.Y(), current_vtx.Z(), mother_id,
                                mother_vtx.X(), mother_vtx.Y(), mother_vtx.Z());
            return mother_id;
        }
        current = mother;
        current_id = mother_id;
    }
    return -1;
}