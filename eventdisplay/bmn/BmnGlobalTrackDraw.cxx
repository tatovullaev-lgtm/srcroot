// -------------------------------------------------------------------------
// -----                  BmnGlobalTrackDraw source file -----
// -----            created 10/12/13 by K. Gertsenberger               -----
// ----- class to visualize reconstructed GlobalTracks in EventDisplay -----
// -------------------------------------------------------------------------

#include "BmnGlobalTrackDraw.h"

#include "BmnDchTrack.h"
#include "BmnGemTrack.h"
#include "BmnGlobalTrack.h"
#include "BmnSiliconTrack.h"
#include "BmnTrack.h"
#include "CbmStsTrack.h"
#include "FairLogger.h"
#include "TDatabasePDG.h"
#include "TEveManager.h"
#include "TEvePathMark.h"
#include "TEveVector.h"

#include <iostream>
using namespace std;

enum Detectors
{
    SILICON,
    GEMS,
    CSC1,
    CSC2,
    TOF1,
    TOF2,
    DCH,
    DCH1,
    DCH2
};

// default constructor
BmnGlobalTrackDraw::BmnGlobalTrackDraw()
    : FairTask("BmnGlobalTrackDraw", 0)
    , fTrackList(nullptr)
    , fTrPr(nullptr)
    , fEveTrList(nullptr)
    , fTrList(nullptr)
{}

// standard constructor
BmnGlobalTrackDraw::BmnGlobalTrackDraw(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fTrackList(nullptr)
    , fTrPr(nullptr)
    , fEveTrList(new TObjArray(16))
    , fTrList(nullptr)
{}

// initialization of the track drawing task
InitStatus BmnGlobalTrackDraw::Init()
{
    if (fVerbose > 0)
        cout << "BmnGlobalTrackDraw::Init()" << endl;

    fTrackList.reset(static_cast<TClonesArray*>(FairRootManager::Instance()->GetObject(GetName())));
    if (!fTrackList) {
        LOG(error) << "BmnGlobalTrackDraw::Init() branch GlobalTrack not "
                      "found! Task will be deactivated";
        SetActive(kFALSE);
        return kERROR;
    }

    fEventManager = MpdEventManager::Instance();

    fUpstreamHitList.reset(static_cast<TClonesArray*>(FairRootManager::Instance()->GetObject("BmnUpstreamHit")));
    fUpstreamTrackList.reset(static_cast<TClonesArray*>(FairRootManager::Instance()->GetObject("BmnUpstreamTrack")));

    fSiliconHitList.reset(static_cast<TClonesArray*>(FairRootManager::Instance()->GetObject("BmnSiliconHit")));
    fSiliconTrackList.reset(static_cast<TClonesArray*>(FairRootManager::Instance()->GetObject("BmnSiliconTrack")));

    fGemHitList.reset(static_cast<TClonesArray*>(FairRootManager::Instance()->GetObject("BmnGemStripHit")));
    fGemTrackList.reset(static_cast<TClonesArray*>(FairRootManager::Instance()->GetObject("BmnGemTrack")));

    fStsHitList.reset(static_cast<TClonesArray*>(FairRootManager::Instance()->GetObject("StsHit")));
    fStsTrackList.reset(static_cast<TClonesArray*>(FairRootManager::Instance()->GetObject("StsTrack")));

    fStsVectorTrackList.reset(static_cast<TClonesArray*>(FairRootManager::Instance()->GetObject("StsVector")));

    fCscHitList.reset(static_cast<TClonesArray*>(FairRootManager::Instance()->GetObject("BmnCSCHit")));

    fTof1HitList.reset(static_cast<TClonesArray*>(FairRootManager::Instance()->GetObject("BmnTof400Hit")));
    fTof2HitList.reset(static_cast<TClonesArray*>(FairRootManager::Instance()->GetObject("BmnTof700Hit")));

    fDchHitList.reset(static_cast<TClonesArray*>(FairRootManager::Instance()->GetObject("BmnDchHit")));
    fDchTrackList.reset(static_cast<TClonesArray*>(FairRootManager::Instance()->GetObject("BmnDchTrack")));

    if (fVerbose > -10)
        cout << "BmnGlobalTrackDraw::Init() get instance of MpdEventManager " << endl;

    return kSUCCESS;
}

void BmnGlobalTrackDraw::Exec(Option_t* option)
{
    if (!IsActive())
        return;

    if (fVerbose > 1)
        cout << " BmnGlobalTrackDraw::Exec " << endl;

    Reset();

    struct PointCompare
    {
        bool operator()(const FairHit* a, const FairHit* b) const { return a->GetZ() < b->GetZ(); }
    };

    for (Int_t i = 0; i < fTrackList->GetEntriesFast(); i++) {
        if (fVerbose > 1)
            cout << "BmnGlobalTrackDraw::Exec " << i << endl;

        auto* tr = (BmnGlobalTrack*)fTrackList->At(i);
        const auto* pParamFirst = tr->GetParamFirst();

        // define whether track is primary
        bool isPrimary = ((TMath::Abs(pParamFirst->GetX()) < 10) && (TMath::Abs(pParamFirst->GetY()) < 10)
                          && (TMath::Abs(pParamFirst->GetZ()) < 10));

        // skip secondary tracks if primary flag is set
        if (MpdEventManager::Instance()->IsPriOnly() && (!isPrimary))
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
        auto* track = new TEveTrack(P, particlePDG, fTrPr);
        // set line color corresponding PDG particle code
        track->SetLineColor(fTrList->GetMainColor());
        track->SetLineWidth(2);

        multiset<FairHit*, PointCompare> track_points;

        // get upstream track for global track
        // if (tr->GetUpstreamTrackIndex() > -1)  // KG почему закомментировано (это только для src, да и то не
        // работает)
        // {
        //     auto *upstream_track = static_cast<BmnTrack *>(
        //         fUpstreamTrackList->UncheckedAt(tr->GetUpstreamTrackIndex()));
        //     for (Int_t k = 0; k < upstream_track->GetNHits(); k++)
        //         track_points.insert(
        //             static_cast<FairHit *>(fUpstreamTrackList->UncheckedAt(
        //                 upstream_track->GetHitIndex(k))));
        // }

        // get Silicon track for global track
        if (fSiliconTrackList != 0 && tr->GetSilTrackIndex() > -1) {
            auto* silicon_track = static_cast<BmnSiliconTrack*>(fSiliconTrackList->UncheckedAt(tr->GetSilTrackIndex()));
            for (Int_t k = 0; k < silicon_track->GetNHits(); k++)
                track_points.insert(static_cast<FairHit*>(fSiliconHitList->UncheckedAt(silicon_track->GetHitIndex(k))));
        }

        // get GEM track for global track
        if (fGemTrackList != 0 && tr->GetGemTrackIndex() > -1) {
            auto* gem_track = static_cast<BmnGemTrack*>(fGemTrackList->UncheckedAt(tr->GetGemTrackIndex()));
            for (Int_t k = 0; k < gem_track->GetNHits(); k++)
                track_points.insert(static_cast<FairHit*>(fGemHitList->UncheckedAt(gem_track->GetHitIndex(k))));
        }

        // get Sts track for global track
        if (fStsTrackList != 0 && tr->GetGemTrackIndex() > -1) {
            auto* sts_track = static_cast<CbmStsTrack*>(fStsTrackList->UncheckedAt(tr->GetGemTrackIndex()));
            for (Int_t k = 0; k < sts_track->GetNStsHits(); k++)
                track_points.insert(static_cast<FairHit*>(fStsHitList->UncheckedAt(sts_track->GetStsHitIndex(k))));
        }

        // get StsVector track for global track
        if (fStsVectorTrackList != 0 && tr->GetGemTrackIndex() > -1) {
            auto* sts_track = static_cast<CbmStsTrack*>(fStsVectorTrackList->UncheckedAt(tr->GetGemTrackIndex()));
            for (Int_t k = 0; k < sts_track->GetNStsHits(); k++)
                track_points.insert(static_cast<FairHit*>(fStsHitList->UncheckedAt(sts_track->GetStsHitIndex(k))));
        }

        // add CSC hit
        if (fCscHitList != 0 && tr->GetCscHitIndex(0) > -1) {
            track_points.insert(static_cast<FairHit*>(fCscHitList->UncheckedAt(tr->GetCscHitIndex(0))));
        }

        // add TOF1 hit
        if (fTof1HitList != 0 && tr->GetTof1HitIndex() > -1) {
            track_points.insert(static_cast<FairHit*>(fTof1HitList->UncheckedAt(tr->GetTof1HitIndex())));
        }

        // add TOF2 hit
        if (fTof2HitList != 0 && tr->GetTof2HitIndex() > -1) {
            track_points.insert(static_cast<FairHit*>(fTof2HitList->UncheckedAt(tr->GetTof2HitIndex())));
        }

        // add DCH hit
        if (fDchTrackList != 0 && tr->GetDchTrackIndex() > -1) {
            BmnDchTrack* dch_track = static_cast<BmnDchTrack*>(fDchTrackList->UncheckedAt(tr->GetDchTrackIndex()));
            for (Int_t k = 0; k < dch_track->GetNHits(); k++)
                track_points.insert((FairHit*)fDchTrackList->UncheckedAt(dch_track->GetHitIndex(k)));
        }

        Int_t n = 0;
        for (auto& hit : track_points) {
            track->SetPoint(n, hit->GetX(), hit->GetY(), hit->GetZ());

            auto* path = new TEvePathMark();
            TEveVector pos(hit->GetX(), hit->GetY(), hit->GetZ());
            path->fV = pos;
            path->fTime = hit->GetTimeStamp();
            if (n == 0) {
                TEveVector Mom(px, py, pz);
                path->fP = Mom;
            }

            // add path marker for current EVE track
            track->AddPathMark(*path);

            if (fVerbose > 3)
                cout << "Path marker added " << path << endl;
            n++;
        }

        // add track to EVE track list
        fTrList->AddElement(track);

        if (fVerbose > 3)
            cout << "Track added " << track->GetName() << endl;
    }

    // redraw EVE scenes
    gEve->Redraw3D(kFALSE);
}

// destructor
BmnGlobalTrackDraw::~BmnGlobalTrackDraw() {}

void BmnGlobalTrackDraw::SetParContainers() {}

void BmnGlobalTrackDraw::Finish() {}

void BmnGlobalTrackDraw::Reset()
{
    // clear EVE track lists (fEveTrList)
    for (Int_t i = 0; i < fEveTrList->GetEntriesFast(); i++) {
        auto* ele = static_cast<TEveTrackList*>(fEveTrList->At(i));
        gEve->RemoveElement(ele, MpdEventManager::Instance()->EveRecoTracks);
    }

    fEveTrList->Clear();
}

// return pointer to EVE track list for given group_name and/or particle name. if list don't exist then create it
TEveTrackList* BmnGlobalTrackDraw::GetTrGroup(const char* group_name, TParticle* P)
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

        MpdEventManager::Instance()->AddEventElement(fTrList, RecoTrackList);
    }

    return fTrList;
}
