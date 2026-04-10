
#ifndef BMNH3LTRIPLEFINDER_H
#define BMNH3LTRIPLEFINDER_H 1

#include "BmnFieldMap.h"
#include "BmnGlobalTrack.h"
#include "BmnKalmanFilter.h"
#include "BmnNewFieldMap.h"
#include "BmnParticleTriple.h"
#include "BmnStripData.h"
#include "CbmStsHit.h"
#include "CbmStsTrack.h"
#include "CbmVertex.h"
#include "DstEventHeader.h"
#include "FairTask.h"
#include "TFile.h"
#include "TGeoManager.h"
#include "TRandom.h"
#include "UniRun.h"

using namespace std;
using namespace TMath;

class BmnH3LTripleFinder : public FairTask
{
  private:
    UInt_t fEventCounter;   //! event counter

    TClonesArray* fStsHits;
    TClonesArray* fUpperClusters;
    TClonesArray* fLowerClusters;
    TClonesArray* fStsTracks;
    TClonesArray* fGlobTracks;
    CbmVertex* fVertex;

    // BmnGemStripStationSet* fDetector; // Detector geometry
    // BmnGemStripConfiguration::GEM_CONFIG fGeometry;

    // BmnFieldMap* fMagField;
    BmnKalmanFilter* fKalman;

    TClonesArray* fParticleTriple;

    // const Int_t fPDG1 = 1000010020; //detron
    const Int_t fPDG1 = 2212;   // proton
    const Int_t fPDG2 = 2212;   // proton
    const Int_t fPDG3 = -211;   // pi-minus

    const Float_t fMassHe3 = 2.8094135;
    const Float_t fMassPi = 0.1395704;
    const Float_t fMassP = 0.9382721;
    const Float_t fMassD = 1.8756129;

  public:
    BmnH3LTripleFinder();
    virtual ~BmnH3LTripleFinder();

    virtual void Exec(Option_t* option);
    virtual InitStatus Init();
    virtual void Finish();

  private:
    void Analysis();

    Double_t FindV0ByVirtualPlanes(FairTrackParam* par1,
                                   FairTrackParam* par2,
                                   FairTrackParam* par3,
                                   Double_t z_0,
                                   Double_t range = 25.);
    Double_t GetdEdx(CbmStsTrack* tr);
    void GetNHits(CbmStsTrack* tr, Int_t& nGEM, Int_t& nFSD);

    ClassDef(BmnH3LTripleFinder, 0)
};

#endif