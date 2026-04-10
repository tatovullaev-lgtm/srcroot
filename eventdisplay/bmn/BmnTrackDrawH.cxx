// -------------------------------------------------------------------------
// -----                  BmnTrackDrawH source file                  -----
// -----            created 05/10/15 by K. Gertsenberger               -----
// ----- class to visualize GlobalTracks from *.root in EventDisplay   -----
// -------------------------------------------------------------------------

#include "BmnTrackDrawH.h"

#include "BmnTrack.h"
#include "CbmStsTrack.h"
#include "FairHit.h"
#include "FairLogger.h"
#include "TDatabasePDG.h"
#include "TEveManager.h"
#include "TEvePathMark.h"
#include "TEveVector.h"

#include <iostream>
using namespace std;

// default constructor
BmnTrackDrawH::BmnTrackDrawH()
    : FairTask("BmnTrackDrawH", 0)
    , fTrackList(nullptr)
    , fHitList(nullptr)
    , fTrPr(nullptr)
    , fEventManager(nullptr)
    , fEveTrList(nullptr)
    , fTrList(nullptr)
    , fHitsBranchName("")
    , fIsCbmStsTrack(kFALSE)
{}

// standard constructor
BmnTrackDrawH::BmnTrackDrawH(const char* name, TString hitsBranchName, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fTrackList(nullptr)
    , fHitList(nullptr)
    , fTrPr(nullptr)
    , fEventManager(nullptr)
    , fEveTrList(new TObjArray(16))
    , fTrList(nullptr)
    , fHitsBranchName(hitsBranchName)
    , fIsCbmStsTrack(kFALSE)
{}

// initialization of the track drawing task
InitStatus BmnTrackDrawH::Init()
{
    if (fVerbose > 0)
        LOG(info) << "BmnTrackDrawH::Init()";

    fEventManager = MpdEventManager::Instance();
    if (fVerbose > 1)
        LOG(debug) << "BmnTrackDrawH::Init() get instance of EventManager: " << fEventManager;

    FairRootManager* fManager = FairRootManager::Instance();
    if (fVerbose > 1)
        LOG(debug) << "BmnTrackDrawH::Init() get instance of FairRootManager: " << fManager;

    fTrackList = (TClonesArray*)fManager->GetObject(GetName());
    if (fTrackList == 0) {
        LOG(error) << "BmnTrackDrawH::Init() branch " << GetName() << " not found! Task will be deactivated";
        SetActive(kFALSE);
        return kERROR;
    }
    if (fVerbose > 1)
        LOG(debug) << "BmnTrackDrawH::Init() get track list " << fTrackList << " from branch '" << GetName() << "'";

    fHitList = (TClonesArray*)fManager->GetObject(fHitsBranchName);
    if (fHitList == 0) {
        LOG(error) << "BmnTrackDrawH::Init()  branch " << fHitsBranchName << " not found! Task will be deactivated";
        SetActive(kFALSE);
        return kERROR;
    }
    if (fVerbose > 1)
        LOG(debug) << "BmnTrackDrawH::Init() get list of hits " << fHitList << " from branch '" << fHitsBranchName
                   << "'";

    fIsCbmStsTrack = fTrackList->GetClass()->InheritsFrom("CbmStsTrack");

    return kSUCCESS;
}

// -------------------------------------------------------------------------
void BmnTrackDrawH::Exec(Option_t* option)
{
    if (fVerbose > 0)
        LOG(info) << "BmnTrackDrawH::Exec() ";

    if (!IsActive())
        return;

    Reset();

    if (fVerbose > 0)
        LOG(info) << "BmnTrackDrawH::Exec(): the number of tracks is " << fTrackList->GetEntriesFast();

    TObject* current_track;
    for (Int_t i = 0; i < fTrackList->GetEntriesFast(); i++) {
        if (fVerbose > 2)
            LOG(debug) << "BmnTrackDrawH::Exec() " << i;

        current_track = fTrackList->At(i);

        const FairTrackParam* pParamFirst;
        if (fIsCbmStsTrack) {
            pParamFirst = ((CbmStsTrack*)current_track)->GetParamFirst();
        } else {
            pParamFirst = ((BmnTrack*)current_track)->GetParamFirst();
        }

        // define whether track is primary
        bool isPrimary = ((TMath::Abs(pParamFirst->GetX()) < 10) && (TMath::Abs(pParamFirst->GetY()) < 10)
                          && (TMath::Abs(pParamFirst->GetZ()) < 10));

        // skip secondary tracks if primary flag is set
        if (fEventManager->IsPriOnly() && (!isPrimary))
            continue;

        // get PDG particle code, without identification - Rootino
        int particlePDG = 0;

        // get momentum
        TVector3 mom;
        pParamFirst->Momentum(mom);
        Double_t px = mom.X(), py = mom.Y(), pz = mom.Z();

        // create particle
        // TParticlePDG *fParticlePDG = TDatabasePDG::Instance()->GetParticle(particlePDG);
        TParticle* P = new TParticle(particlePDG, i, -1, -1, -1, -1, px, py, pz, 0, pParamFirst->GetX(),
                                     pParamFirst->GetY(), pParamFirst->GetZ(), 0);

        // get EVE track list for this particle
        fTrList = GetTrGroup(TString(GetName()), P);
        // create EVE track corresponding global track
        TEveTrack* track = new TEveTrack(P, particlePDG, fTrPr);
        // set line color corresponding PDG particle code
        track->SetLineColor(fTrList->GetMainColor());
        track->SetLineWidth(2);

        Int_t Np;
        if (fIsCbmStsTrack) {
            Np = ((CbmStsTrack*)current_track)->GetNStsHits();
        } else {
            Np = ((BmnTrack*)current_track)->GetNHits();
        }

        // cycle: add hits (points) to EVE path for this track
        if (fVerbose > 2)
            LOG(debug) << "BmnTrackDrawH::Exec(): number of track hits = " << Np;
        for (Int_t n = 0; n < Np; n++) {
            FairHit* pHit = nullptr;
            if (fIsCbmStsTrack) {
                pHit = (FairHit*)fHitList->UncheckedAt(((CbmStsTrack*)current_track)->GetStsHitIndex(n));
            } else {
                pHit = (FairHit*)fHitList->UncheckedAt(((BmnTrack*)current_track)->GetHitIndex(n));
            }

            track->SetPoint(n, pHit->GetX(), pHit->GetY(), pHit->GetZ());

            TEvePathMark* path = new TEvePathMark();
            TEveVector pos = TEveVector(pHit->GetX(), pHit->GetY(), pHit->GetZ());
            if (fVerbose > 3)
                LOG(debug) << "BmnTrackDrawH::Exec(): point " << n << ": X=" << pHit->GetX() << " Y=" << pHit->GetY()
                           << " Z=" << pHit->GetZ();
            path->fV = pos;
            path->fTime = pHit->GetTimeStamp();
            if (n == 0) {
                TEveVector Mom = TEveVector(px, py, pz);
                path->fP = Mom;
            }

            // add path marker for current EVE track
            track->AddPathMark(*path);

            if (fVerbose > 3)
                LOG(debug) << "BmnTrackDrawH::Exec: path marker added " << path;
        }

        // add track to EVE track list
        fTrList->AddElement(track);

        if (fVerbose > 2)
            LOG(debug) << "BmnTrackDrawH::Exec: track was added " << track->GetName();
    }

    // redraw EVE scenes
    // gEve->Redraw3D(kFALSE);
}

// destructor
BmnTrackDrawH::~BmnTrackDrawH() {}

void BmnTrackDrawH::SetParContainers() {}

void BmnTrackDrawH::Finish() {}

void BmnTrackDrawH::Reset()
{
    // clear EVE track lists (fEveTrList)
    for (Int_t i = 0; i < fEveTrList->GetEntriesFast(); i++) {
        TEveTrackList* ele = (TEveTrackList*)fEveTrList->At(i);
        gEve->RemoveElement(ele, fEventManager->EveRecoTracks);
    }
    fEveTrList->Clear();
}

// return pointer to EVE track list for given group_name and/or particle name. if list don't exist then create it
TEveTrackList* BmnTrackDrawH::GetTrGroup(const char* group_name, TParticle* P)
{
    fTrList = 0;
    TString track_group = "";
    if ((group_name != nullptr) && (strcmp(group_name, "")))
        track_group += group_name;
    if (P != nullptr) {
        if (track_group != "")
            track_group += "_";
        track_group += P->GetName();
    }
    if (track_group == "") {
        LOG(error) << "BmnTrackDrawH::GetTrGroup() function parameteres are empty (null)";
        return nullptr;
    }

    // search if there us existing track list for this particle (with given name)
    for (Int_t i = 0; i < fEveTrList->GetEntriesFast(); i++) {
        TEveTrackList* TrListIn = (TEveTrackList*)fEveTrList->At(i);
        if (strcmp(TrListIn->GetName(), track_group.Data()) == 0) {
            fTrList = TrListIn;
            break;
        }
    }

    // create new track list for new name
    if (fTrList == 0) {
        fTrPr = new TEveTrackPropagator();
        fTrList = new TEveTrackList(track_group, fTrPr);
        // set track color by particle PDG from MpdEventManager if group_name is not set
        Int_t new_color = 0;
        if (group_name == nullptr)
            new_color = fEventManager->Color(P->GetPdgCode());
        else
            new_color =
                track_group.Hash() % 9
                + 2;   // fEventManager->EveRecoTracks == nullptr ? 2 : fEventManager->EveRecoTracks->NumChildren() + 2;
        fTrList->SetMainColor(new_color);
        fEveTrList->Add(fTrList);
        fTrList->SetRnrLine(kTRUE);

        fEventManager->AddEventElement(fTrList, RecoTrackList);
    }

    return fTrList;
}
