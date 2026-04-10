#include "BmnTOF701.h"

using namespace std;

//--------------------------------------------------------------------------------------------------------------------------------------
BmnTOF701::BmnTOF701()
    : fPosIndex(0)
    , nan(-10000000)
{
    fTofCollection = new TClonesArray("BmnTOF1Point");
    fVerboseLevel = 1;
}

//--------------------------------------------------------------------------------------------------------------------------------------
BmnTOF701::BmnTOF701(const char* name, Bool_t active)
    : FairDetector(name, active)
    , fPosIndex(0)
    , nan(-10000000)
{
    fTofCollection = new TClonesArray("BmnTOF1Point");
    fVerboseLevel = 1;
}

//--------------------------------------------------------------------------------------------------------------------------------------
BmnTOF701::~BmnTOF701()
{
    fTofCollection->Delete();
    delete fTofCollection;
}

//--------------------------------------------------------------------------------------------------------------------------------------
void BmnTOF701::Initialize()
{
    FairDetector::Initialize();
}

//--------------------------------------------------------------------------------------------------------------------------------------
void BmnTOF701::BeginEvent() {}

//--------------------------------------------------------------------------------------------------------------------------------------
Bool_t BmnTOF701::ProcessHits(FairVolume* vol)
{
    // Set parameters at entrance of volume. Reset ELoss.
    if (gMC->IsTrackEntering()) {
        fELoss = 0.;
        fTime = gMC->TrackTime() * 1.0e09;
        fLength = gMC->TrackLength();
        gMC->TrackPosition(fPos);
        gMC->TrackMomentum(fMom);
    }

    // Sum energy loss for all steps in the active volume
    fELoss += gMC->Edep();

    // Create TofPoint at exit of active volume
    if (fELoss > 0 && (gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared())) {
        Int_t fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
        assert(fTrackID >= 0);

        Int_t region = 0, detector, strip;
        gMC->CurrentVolOffID(0, strip);
        gMC->CurrentVolOffID(2, detector);

        Int_t fVolumeID = BmnTOF1Point::GetVolumeUID(region, detector, strip);

        AddPoint(fTrackID, kTOF701, TVector3(fPos.X(), fPos.Y(), fPos.Z()), TVector3(fMom.Px(), fMom.Py(), fMom.Pz()),
                 fTime, fLength, fELoss, fVolumeID);
        ((CbmStack*)gMC->GetStack())->AddPoint(kTOF701);

        ResetParameters();
    }

    return kTRUE;
}

//--------------------------------------------------------------------------------------------------------------------------------------
void BmnTOF701::EndOfEvent()
{
    if (fVerboseLevel)
        Print("");
    Reset();
}

//--------------------------------------------------------------------------------------------------------------------------------------
void BmnTOF701::Register()
{
    FairRootManager::Instance()->Register("TOF700Point", "TOF700", fTofCollection, kTRUE);
}

//--------------------------------------------------------------------------------------------------------------------------------------
TClonesArray* BmnTOF701::GetCollection(Int_t iColl) const
{
    if (iColl == 0)
        return fTofCollection;

    return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------
void BmnTOF701::Print(Option_t*) const
{
    Int_t nHits = fTofCollection->GetEntriesFast();
    cout << "-I- BmnTOF700: " << nHits << " points registered in this event.\n";

    if (fVerboseLevel > 1)
        for (Int_t i = 0; i < nHits; i++)
            (*fTofCollection)[i]->Print();
}

//--------------------------------------------------------------------------------------------------------------------------------------
void BmnTOF701::Reset()
{
    fTofCollection->Delete();
    fPosIndex = 0;
}

//--------------------------------------------------------------------------------------------------------------------------------------
// guarda in FairRootManager::CopyClones
void BmnTOF701::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset)
{
    Int_t nEntries = cl1->GetEntriesFast();
    // cout << "-I- BmnTOF700: " << nEntries << " entries to add." << endl;
    TClonesArray& clref = *cl2;
    BmnTOF1Point* oldpoint = NULL;

    for (Int_t i = 0; i < nEntries; i++) {
        oldpoint = (BmnTOF1Point*)cl1->At(i);
        Int_t index = oldpoint->GetTrackID() + offset;
        oldpoint->SetTrackID(index);
        new (clref[fPosIndex]) BmnTOF1Point(*oldpoint);
        fPosIndex++;
    }

    cout << " -I- BmnTOF701: " << cl2->GetEntriesFast() << " merged entries." << endl;
}

//--------------------------------------------------------------------------------------------------------------------------------------
void BmnTOF701::ConstructGeometry()
{
    TString fileName = GetGeometryFileName();
    if (fileName.EndsWith(".root")) {
        LOG(info) << "Constructing TOF700 geometry from ROOT file " << fileName.Data();
        ConstructRootGeometry();
    } else if (fileName.EndsWith(".geo")) {
        LOG(info) << "Constructing TOF700 geometry from ASCII file " << fileName.Data();
        ConstructAsciiGeometry();
    } else
        LOG(fatal) << "Geometry format of TOF700 file " << fileName.Data() << " not supported.";
}

//--------------------------------------------------------------------------------------------------------------------------------------
void BmnTOF701::ConstructAsciiGeometry()
{
    FairGeoLoader* geoLoad = FairGeoLoader::Instance();
    FairGeoInterface* geoFace = geoLoad->getGeoInterface();
    BmnTOF1Geo* TOF701Geo = new BmnTOF1Geo();
    TOF701Geo->setGeomFile(GetGeometryFileName());
    geoFace->addGeoModule(TOF701Geo);

    Bool_t rc = geoFace->readSet(TOF701Geo);
    if (rc)
        TOF701Geo->create(geoLoad->getGeoBuilder());
    TList* volList = TOF701Geo->getListOfVolumes();

    // store geo parameter
    FairRun* fRun = FairRun::Instance();
    FairRuntimeDb* rtdb = FairRun::Instance()->GetRuntimeDb();
    BmnTOF1GeoPar* par = (BmnTOF1GeoPar*)(rtdb->getContainer("BmnTOF1GeoPar"));
    TObjArray* fSensNodes = par->GetGeoSensitiveNodes();
    TObjArray* fPassNodes = par->GetGeoPassiveNodes();

    TListIter iter(volList);
    FairGeoNode* node = nullptr;
    FairGeoVolume* aVol = nullptr;

    while ((node = (FairGeoNode*)iter.Next())) {
        aVol = dynamic_cast<FairGeoVolume*>(node);

        if (node->isSensitive())
            fSensNodes->AddLast(aVol);
        else
            fPassNodes->AddLast(aVol);
    }

    par->setChanged();
    par->setInputVersion(fRun->GetRunId(), 1);
    ProcessNodes(volList);
}

//--------------------------------------------------------------------------------------------------------------------------------------
Bool_t BmnTOF701::CheckIfSensitive(std::string name)
{
    TString tsname = name;
    if (tsname.Contains("ActiveGasStrip"))
        return kTRUE;

    return kFALSE;
}

//--------------------------------------------------------------------------------------------------------------------------------------
BmnTOF1Point* BmnTOF701::AddPoint(Int_t trackID,
                                  Int_t detID,
                                  TVector3 pos,
                                  TVector3 mom,
                                  Double_t time,
                                  Double_t length,
                                  Double_t eLoss,
                                  Int_t volUID)
{
    return new ((*fTofCollection)[fTofCollection->GetEntriesFast()])
        BmnTOF1Point(trackID, detID, pos, mom, time, length, eLoss, volUID);
}
