#include "BmnVacWall.h"

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

BmnVacWall::BmnVacWall()
    : FairDetector("BmnVacWall", true)
    , fGeoHandler(new BmnZdcGeo())
    , fCollection(new TClonesArray("FairMCPoint"))
{
    LOG(debug4) << "BmnVacWall default constructor called";
}

BmnVacWall::BmnVacWall(const char* name, Bool_t active)
    : FairDetector(name, active)
    , fGeoHandler(new BmnZdcGeo())
    , fCollection(new TClonesArray("FairMCPoint"))
{
    LOG(debug4) << "BmnVacWall constructor called with name: " << name << " and active: " << active;
}

BmnVacWall::~BmnVacWall()
{
    LOG(debug4) << "BmnVacWall destructor called";
    if (fCollection) {
        fCollection->Delete();
        delete fCollection;
    }
    delete fGeoHandler;
}

void BmnVacWall::Initialize()
{
    LOG(debug4) << "BmnVacWall Initialize method called";
    FairDetector::Initialize();
    // FairRun* sim = FairRun::Instance();
    // FairRuntimeDb* rtdb = sim->GetRuntimeDb();
}

void BmnVacWall::BeginEvent()
{
    LOG(debug4) << "BmnVacWall BeginEvent method called";
}

Bool_t BmnVacWall::ProcessHits(FairVolume* vol)
{
    LOG(debug2) << "BmnVacWall ProcessHits method called for event " << gMC->CurrentEvent();
    if (!CheckIfSensitive(gMC->CurrentVolName())) {
        return kFALSE;
    }

    Int_t trackID = gMC->GetStack()->GetCurrentTrackNumber();
    int plane_id;
    gMC->CurrentVolOffID(0, plane_id);   // planeNb 1-6
    if (gMC->IsTrackEntering()) {
        // Initializing new hit
        FairMCPoint* newPoint = new FairMCPoint();
        newPoint->SetEventID(gMC->CurrentEvent());
        newPoint->SetTrackID(trackID);
        newPoint->SetDetectorID(plane_id);
        TLorentzVector tPos, tMom;
        gMC->TrackPosition(tPos);   // Position at entrance
        gMC->TrackMomentum(tMom);   // Momentum at entrance
        newPoint->SetPosition(TVector3(tPos.X(), tPos.Y(), tPos.Z()));
        newPoint->SetMomentum(TVector3(tMom.X(), tMom.Y(), tMom.Z()));
        newPoint->SetTime(gMC->TrackTime() * 1.0e09);   // Time of flight
        newPoint->SetLength(gMC->TrackLength());        // Length of the current track from its origin (in cm)

        // Inserting new hit
        AddHit(newPoint);
        ((CbmStack*)gMC->GetStack())->AddPoint(kVACWALL);
    }
    return kTRUE;
}

void BmnVacWall::EndOfEvent()
{
    LOG(debug4) << "BmnVacWall EndOfEvent method called";
    Print("");
    Reset();
}

void BmnVacWall::Register()
{
    LOG(debug4) << "BmnVacWall Register method called";
    if (!FairRootManager::Instance()->GetObject("VacWallPoint"))
        FairRootManager::Instance()->Register("VacWallPoint", "VacWall", fCollection, kTRUE);
    else
        LOG(debug4) << "VacWallPoint Already registered";
}

TClonesArray* BmnVacWall::GetCollection(Int_t iColl) const
{
    LOG(debug4) << "BmnVacWall GetCollection method called with iColl: " << iColl;
    return (iColl == 0) ? fCollection : nullptr;
}

void BmnVacWall::Print(Option_t*) const
{
    LOG(debug4) << "BmnVacWall Print method called";
    Int_t nHits = fCollection->GetEntriesFast();
    std::cout << "-I- BmnVacWall: " << nHits << " points registered in event " << gMC->CurrentEvent() << std::endl;

    if (FairLogger::GetLogger()->IsLogNeeded(fair::Severity::debug2)) {
        for (Int_t i = 0; i < nHits; i++) {
            (*fCollection)[i]->Print();
        }
    }
}

void BmnVacWall::Reset()
{
    LOG(debug4) << "BmnVacWall Reset method called";
    fCollection->Delete();
}

void BmnVacWall::ConstructGeometry()
{
    LOG(debug4) << "BmnVacWall ConstructGeometry method called";
    TString fileName = GetGeometryFileName();
    if (fileName.EndsWith(".root")) {
        LOG(info) << Form("Constructing VacWall geometry from ROOT file %s", fileName.Data());
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

    ProcessNodes(volList);
}

Bool_t BmnVacWall::CheckIfSensitive(std::string name)
{
    LOG(debug4) << "BmnVacWall CheckIfSensitive method called with name: " << name;
    TString tsname = name.c_str();
    // zdc legacy
    return tsname.Contains("zdc01s") || tsname.Contains("zdc01s_after") || tsname.Contains("ScH");
}

FairMCPoint* BmnVacWall::AddHit(FairMCPoint* point)
{
    TClonesArray& clref = *fCollection;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size]) FairMCPoint(*point);
}

void BmnVacWall::RemoveHit(Int_t index)
{
    if (index < 0 || index >= fCollection->GetEntriesFast()) {
        LOG(debug4) << "BmnVacWall RemoveHit method index: " << index << " out of range.";
        return;
    }
    fCollection->RemoveAt(index);
    fCollection->Compress();   // This method will remove NULL pointers from the array
}
