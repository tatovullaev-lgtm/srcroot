

#ifndef BMNPAIRFINDER_H
#define BMNPAIRFINDER_H 

#include <iostream>
#include <vector>
#include "FairTask.h"
#include <TDatabasePDG.h>
#include <TGraph.h>
#include <TH1I.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TMath.h>
#include <TString.h>
#include <TClonesArray.h>
#include <TLorentzVector.h>
#include <TVector2.h>
#include <TVector3.h>
#include <TF1.h>
#include <TFitResult.h>
#include "BmnGlobalTrack.h"
#include "BmnFieldMap.h"
#include "BmnNewFieldMap.h"
#include "CbmMCTrack.h"
#include "CbmStsPoint.h"
#include "BmnSiliconPoint.h"
#include "BmnVertex.h"
#include "CbmVertex.h"
#include "BmnGemStripStationSet.h"
#include "BmnGemStripStationSet_RunWinter2016.h"
#include "BmnGemStripStationSet_RunSpring2017.h"
#include "BmnGemStripHit.h"
#include "BmnKalmanFilter.h"
#include <BmnParticlePair.h>
#include <UniDetectorParameter.h>
#include <UniRun.h>
#include "BmnTrackMatch.h"
#include <DstEventHeader.h>
#include <BmnEventHeader.h>

using namespace std;
using namespace TMath;

// Extenion of "standard" dst event header by info to be used in dec. particle anal ... 


class BmnPairFinder : public FairTask {
private:

BmnKalmanFilter* fKalman;

DstEventHeader* fDstHeader;
TClonesArray* fStsTracks;
CbmVertex* fVertex;

TClonesArray* fParticlePair;

Double_t FindV0ByVirtualPlanes(BmnGlobalTrack *track1, BmnGlobalTrack *track2, Double_t z_0, Double_t range);
Double_t FindInvMass(FairTrackParam pi_plus_meson_V0, FairTrackParam pi_minus_meson_V0);
Double_t GetPath(Double_t vPx, Double_t vPy, Double_t vPz, Double_t v0x, Double_t v0y, Double_t v0z);
Double_t FindDCA0(FairTrackParam pi_plus_meson_V0, FairTrackParam pi_minus_meson_V0, Double_t vPx, Double_t vPy, Double_t vPz, Double_t v0x, Double_t v0y, Double_t v0z);
Double_t FindEta(FairTrackParam pi_plus_meson_V0);
Double_t FindAngleDecayProducts(FairTrackParam proton_V0, FairTrackParam pi_minus_meson_V0);

Int_t fRunId;
Int_t evNumber;
unique_ptr<TParticlePDG> fParticle;
unique_ptr<TParticlePDG> fDecayFirstParticle;
unique_ptr<TParticlePDG> fDecaySecondParticle;
Double_t lowV0Z;

public:

    BmnPairFinder(TParticlePDG * particle = TDatabasePDG::Instance()->GetParticle("Lambda0"));
    virtual ~BmnPairFinder();

    virtual void Exec(Option_t * option);
    virtual InitStatus Init();
    virtual void Finish();

    ClassDef(BmnPairFinder, 0)
};

#endif
