#include "BmnNdet.h"

#include "BmnNdetCell.h"
#include "BmnNdetSurfacePoint.h"
#include "CbmStack.h"
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairVolume.h"
#include "TGeoManager.h"
#include "TLorentzVector.h"
#include "TParticle.h"
#include "TVirtualMC.h"

#include <iostream>

BmnNdet::BmnNdet()
    : FairDetector("BmnNdet", true)
    , fGeoHandler(new BmnNdetGeo())
    , fCollection(new TClonesArray("BmnNdetPoint"))
    , fSurfaceCollection(new TClonesArray("BmnNdetSurfacePoint"))
{
    LOG(debug4) << "BmnNdet default constructor called";
}

BmnNdet::BmnNdet(const char* name, Bool_t active)
    : FairDetector(name, active)
    , fGeoHandler(new BmnNdetGeo())
    , fCollection(new TClonesArray("BmnNdetPoint"))
    , fSurfaceCollection(new TClonesArray("BmnNdetSurfacePoint"))
{
    LOG(debug4) << "BmnNdet constructor called with name: " << name << " and active: " << active;
}

BmnNdet::~BmnNdet()
{
    LOG(debug4) << "BmnNdet destructor called";
    if (fCollection) {
        fCollection->Delete();
        delete fCollection;
    }
    if (fSurfaceCollection) {
        fSurfaceCollection->Delete();
        delete fSurfaceCollection;
    }
    delete fGeoHandler;
}

void BmnNdet::Initialize()
{
    LOG(debug4) << "BmnNdet Initialize method called";
    FairDetector::Initialize();
    // FairRun* sim = FairRun::Instance();
    // FairRuntimeDb* rtdb = sim->GetRuntimeDb();
}

void BmnNdet::BeginEvent()
{
    LOG(debug4) << "BmnNdet BeginEvent method called";
}

Int_t BmnNdet::FindHitIndex(int trackID, uint32_t address)
{
    LOG(debug4) << "BmnNdet FindHitIndex method called for (trackID, address): (" << trackID << ", " << address << ")";
    // almost guaranteed that each time we need last entry, therefore search backwards
    for (Int_t i = fCollection->GetEntriesFast() - 1; i >= 0; i--) {
        auto* ThisPoint = static_cast<BmnNdetPoint*>(fCollection->At(i));
        if (ThisPoint->GetTrackID() == trackID && ThisPoint->GetAddress() == address) {
            return i;   // Return the index if a match is found
        }
    }

    return -1;   // Return -1 if no match is found
}

BmnNdetPoint* BmnNdet::GetHit(int trackID, uint32_t address)
{
    auto index = FindHitIndex(trackID, address);
    if (index >= 0)
        return static_cast<BmnNdetPoint*>(fCollection->At(index));
    else
        return nullptr;
}

double GetBirkQCF()
{
    LOG(debug4) << "GetBirkQCF function called";
    const double BirkConst = 12.6;   // (12.6 cm/GeV - from Wikipedia, 0.07943mm/MeV in Geant4)
    double QCF = 1.0 + (BirkConst / gMC->TrackStep()) * gMC->Edep();
    return QCF;
}

Bool_t BmnNdet::ProcessHits(FairVolume* vol)
{
    LOG(debug4) << "BmnNdet ProcessHits method called for event " << gMC->CurrentEvent();
    Int_t trackID = gMC->GetStack()->GetCurrentTrackNumber();

    if (CheckIfVacuum()) {
        if (gMC->IsTrackEntering()) {
            LOG(debug4) << "BmnNdet::ProcessHits: vacuum" << gMC->CurrentVolName() << " " << gMC->CurrentVolPath();
            TLorentzVector tPos, tMom;
            gMC->TrackPosition(tPos);
            gMC->TrackMomentum(tMom);
            TVector3 pos = TVector3(tPos.X(), tPos.Y(), tPos.Z());
            TVector3 mom = TVector3(tMom.X(), tMom.Y(), tMom.Z());
            auto [planeId, direction] = fGeoHandler->GetEnteredFace(pos, mom);
            uint8_t armID = fGeoHandler->GetArmFromPath(gMC->CurrentVolPath());

            new ((*fSurfaceCollection)[fSurfaceCollection->GetEntriesFast()])
                BmnNdetSurfacePoint(gMC->GetStack()->GetCurrentTrackNumber(), pos, mom, gMC->TrackTime() * 1.0e09,
                                    gMC->TrackLength(), gMC->TrackPid(), gMC->CurrentEvent(), armID,
                                    static_cast<uint8_t>(planeId), static_cast<uint8_t>(direction));

            ((CbmStack*)gMC->GetStack())->AddPoint(kNDET);
        }
        return kTRUE;
    }

    if (!CheckIfSensitive(gMC->CurrentVolName()) || gMC->Edep() < 0) {
        return kFALSE;
    }

    uint32_t address = fGeoHandler->GetAddressFromPath(gMC->CurrentVolPath());

    // new track is entering sensitive volume for the first time
    if (GetHit(trackID, address) == nullptr) {
        LOG(debug4) << "trackID = " << trackID << " entering. " << BmnNdetAddress::GetInfoString(address)
                    << ". Edep = " << gMC->Edep() << " Time = " << gMC->TrackTime() * 1.0e09;
        if (!gMC->IsTrackEntering())
            LOG(warning) << "trackID = " << trackID << " is not marked as gMC->IsTrackEntering() ";

        // Initializing new hit
        BmnNdetPoint* newPoint = new BmnNdetPoint();
        newPoint->ResetLinks();
        newPoint->SetEventID(gMC->CurrentEvent());
        newPoint->SetTrackID(trackID);
        newPoint->SetAddress(address);
        TLorentzVector tPos, tMom;
        gMC->TrackPosition(tPos);   // Position at entrance
        gMC->TrackMomentum(tMom);   // Momentum at entrance
        newPoint->SetPosition(TVector3(tPos.X(), tPos.Y(), tPos.Z()));
        newPoint->SetMomentum(TVector3(tMom.X(), tMom.Y(), tMom.Z()));

        double dEdep = (gMC->TrackCharge() != 0 && gMC->TrackStep() > 0) ? gMC->Edep() / GetBirkQCF()
                                                                         : gMC->Edep();   // Energy loss at current step
        newPoint->SetEnergyLoss(dEdep);
        newPoint->SetTime(gMC->TrackTime() * 1.0e09);   // Time of flight
        newPoint->SetLength(gMC->TrackLength());        // Length of the current track from its origin (in cm)

        // Inserting new hit
        AddHit(newPoint);
    } else {   // Increment hit properties of this track
        double dEdep = (gMC->TrackCharge() != 0 && gMC->TrackStep() > 0) ? gMC->Edep() / GetBirkQCF()
                                                                         : gMC->Edep();   // Energy loss at current step
        if (dEdep > std::numeric_limits<double>::epsilon()) {
            LOG(debug4) << "BmnNdet::ProcessHits. trackID = " << trackID << " inside or exiting. "
                        << BmnNdetAddress::GetInfoString(address) << ". Edep = " << gMC->Edep()
                        << " Time = " << gMC->TrackTime() * 1.0e09;

            BmnNdetPoint* point = GetHit(trackID, address);
            double weighted_time = point->GetEnergyLoss() * point->GetTime() + dEdep * gMC->TrackTime() * 1.0e09;
            point->SetEnergyLoss(point->GetEnergyLoss() + dEdep);
            point->SetTime(weighted_time / point->GetEnergyLoss());   // Reweighted time of flight
        }
    }
    // TODO handle reentering case differently if needed.
    // gMC->IsTrackEntering() && GetHit(trackID, address) != nullptr

    // last MC step for this track in this sens volume. If hit is zero-energy remove it from hits collection
    if (gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared()) {
        BmnNdetPoint* point = GetHit(trackID, address);
        if (FinaliseTrackProcessing(point)) {
            ((CbmStack*)gMC->GetStack())->AddPoint(kNDET);
            auto surf_point = GetSurfacePoint(trackID);
            if (surf_point >= 0)
                point->AddLink(FairLink(static_cast<int>(BmnNdetCell::LinkType::SurfaceMCPoint), surf_point));
        }
    }

    return kTRUE;
}

bool BmnNdet::FinaliseTrackProcessing(BmnNdetPoint* point)
{
    if (!point)
        return false;
    if (point->GetEnergyLoss() < std::numeric_limits<double>::epsilon()) {
        LOG(debug4) << "BmnNdet ProcessHits: remove hit. trackID = " << point->GetTrackID() << " "
                    << BmnNdetAddress::GetInfoString(point->GetAddress());
        Int_t index = FindHitIndex(point->GetTrackID(), point->GetAddress());
        RemoveHit(index);
        return false;
    }
    return true;
}

void BmnNdet::EndOfEvent()
{
    LOG(debug4) << "BmnNdet EndOfEvent method called";
    Print("");
    Reset();
}

void BmnNdet::Register()
{
    LOG(debug4) << "BmnNdet Register method called";
    if (!FairRootManager::Instance()->GetObject("NdetPoint")) {
        FairRootManager::Instance()->Register("NdetPoint", "Ndet", fCollection, kTRUE);
        FairRootManager::Instance()->Register("NdetSurfacePoint", "Ndet", fSurfaceCollection, kTRUE);
    } else
        LOG(debug4) << "NdetPoint Already registered";
}

TClonesArray* BmnNdet::GetCollection(Int_t iColl) const
{
    LOG(debug4) << "BmnNdet GetCollection iColl=" << iColl;
    if (iColl == 0)
        return fCollection;
    if (iColl == 1)
        return fSurfaceCollection;
    return nullptr;
}

void BmnNdet::Print(Option_t*) const
{
    LOG(debug4) << "BmnNdet Print method called";
    Int_t nHits = fCollection->GetEntriesFast();
    std::cout << "-I- BmnNdet: " << nHits << " points registered in event " << gMC->CurrentEvent() << std::endl;

    if (FairLogger::GetLogger()->IsLogNeeded(fair::Severity::debug4)) {
        for (Int_t i = 0; i < nHits; i++) {
            (*fCollection)[i]->Print();
        }
    }
}

void BmnNdet::Reset()
{
    LOG(debug4) << "BmnNdet Reset method called";
    fCollection->Delete();
    fSurfaceCollection->Delete();
}

void BmnNdet::ConstructGeometry()
{
    LOG(debug4) << "BmnNdet ConstructGeometry method called";
    TString fileName = GetGeometryFileName();
    if (fileName.EndsWith(".root")) {
        LOG(info) << Form("Constructing NDET geometry from ROOT file %s", fileName.Data());
        ConstructRootGeometry();
    }

    FairGeoLoader* geoLoad = FairGeoLoader::Instance();
    FairGeoInterface* geoFace = geoLoad->getGeoInterface();
    fGeoHandler->setGeomFile(GetGeometryFileName());
    geoFace->addGeoModule(fGeoHandler);

    Bool_t rc = geoFace->readSet(fGeoHandler);
    if (rc)
        fGeoHandler->create(geoLoad->getGeoBuilder());
    TList* volList = fGeoHandler->getListOfVolumes();

    // Store geo parameter
    FairRun* fRun = FairRun::Instance();
    FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
    BmnNdetGeoPar* par = (BmnNdetGeoPar*)(rtdb->getContainer("BmnNdetGeoPar"));
    TObjArray* fSensNodes = par->GetGeoSensitiveNodes();
    TObjArray* fPassNodes = par->GetGeoPassiveNodes();

    TListIter iter(volList);
    FairGeoNode* node = nullptr;

    while ((node = (FairGeoNode*)iter.Next())) {
        FairGeoVolume* aVol = dynamic_cast<FairGeoVolume*>(node);
        if (node->isSensitive()) {
            fSensNodes->AddLast(aVol);
            LOG(debug4) << "BmnNdet ConstructGeometry add sensitive node " << aVol->GetName();
        } else {
            fPassNodes->AddLast(aVol);
            LOG(debug4) << "BmnNdet ConstructGeometry add passive node " << aVol->GetName();
        }
    }
    par->setChanged();
    par->setInputVersion(fRun->GetRunId(), 1);

    ProcessNodes(volList);
}

Bool_t BmnNdet::CheckIfSensitive(std::string name)
{
    LOG(debug4) << "BmnNdet CheckIfSensitive method called with name: " << name;
    TString tsname = name.c_str();
    return tsname == BmnNdetGeoPar::fEnvelopeVolumeName || tsname.Contains(BmnNdetGeoPar::SensitiveVolume_name_NICA)
           || tsname.Contains(BmnNdetGeoPar::SensitiveVolume_name_VETO);
}

Bool_t BmnNdet::CheckIfVacuum()
{
    return gMC->CurrentMedium() == gGeoManager->GetMedium("vacuum")->GetId();
}

BmnNdetPoint* BmnNdet::AddHit(BmnNdetPoint* point)
{
    TClonesArray& clref = *fCollection;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size]) BmnNdetPoint(*point);
}

void BmnNdet::RemoveHit(Int_t index)
{
    if (index < 0 || index >= fCollection->GetEntriesFast()) {
        LOG(debug4) << "BmnNdet RemoveHit method index: " << index << " out of range.";
        return;
    }
    fCollection->RemoveAt(index);
    fCollection->Compress();   // This method will remove NULL pointers from the array
}

int BmnNdet::GetSurfacePoint(int start_track_id)
{
    const auto* stack = static_cast<CbmStack*>(gMC->GetStack());
    int current_id = start_track_id;

    std::unordered_map<int, int> vacuum_map;
    for (int i = 0, n = fSurfaceCollection->GetEntriesFast(); i < n; ++i) {
        auto* pt = static_cast<BmnNdetSurfacePoint*>(fSurfaceCollection->UncheckedAt(i));
        if (pt->GetDirectionID() == static_cast<int>(BmnNdetGeo::Direction::FromOutside))
            vacuum_map[i] = pt->GetTrackID();
    }

    while (true) {
        const TParticle* particle = stack->GetParticle(current_id);
        if (!particle) {
            LOG(error) << "BmnNdet::GetSurfacePoint. No particle with ID: " << current_id;
            return -1;
        }

        for (const auto& [ipt, vac_track] : vacuum_map) {
            if (vac_track == current_id) {
                LOG(debug4) << "BmnNdet::GetSurfacePoint. Linked to " << vac_track;
                return ipt;
            }
        }

        if (particle->IsPrimary()) {
            LOG(debug4) << Form("BmnNdet::GetSurfacePoint. Reached primary particle %d, no vacuum point found",
                                current_id);
            return -1;
        }

        int mother_id = particle->GetFirstMother();
        if (mother_id == current_id || mother_id < 0) {
            LOG(error) << Form("BmnNdet::GetSurfacePoint. Invalid or circular mother link for track %d", current_id);
            return -1;
        }

        current_id = mother_id;
    }

    return -1;
}
