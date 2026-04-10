
#ifndef BMNHYPNUCLPAIRFINDER_H
#define BMNHYPNUCLPAIRFINDER_H 1

#include "BmnFieldMap.h"
#include "BmnGlobalTrack.h"
#include "BmnHypNuclPair.h"
#include "BmnKalmanFilter.h"
#include "BmnNewFieldMap.h"
#include "BmnStripData.h"
#include "BmnTofHit.h"
#include "BmnTrigInfoDst.h"
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

class BmnHypNuclPairFinder : public FairTask
{
  private:
    UInt_t fEventCounter;   //! event counter

    TClonesArray* fStsHits;
    TClonesArray* fUpperGemClusters;
    TClonesArray* fLowerGemClusters;
    TClonesArray* fUpperFsdClusters;
    TClonesArray* fLowerFsdClusters;
    TClonesArray* fStsTracks;
    TClonesArray* fGlobTracks;
    TClonesArray* fTof700Hits;
    CbmVertex* fVertex;
    BmnTrigInfoDst* fTrigger;

    BmnKalmanFilter* fKalman;

    TClonesArray* fParticlePair;

    Float_t massPos;
    Float_t massNeg;

    const Int_t fPDG1 = 2212;   // we use proton insted of he3, because kalman cannot work with ions; 1000020030; //He3
    const Int_t fPDG2 = -211;   // pi-minus

    const Float_t fMassHe4 = 3.7284013;
    const Float_t fMassHe3 = 2.8094135;
    const Float_t fMassPi = 0.1395704;
    const Float_t fMassP = 0.9382721;
    const Float_t fMassD = 1.8756129;

    Bool_t isExp;

    static const Int_t nSt = 11;

    // parameters for dedx scaling
    // const Float_t aLow[nSt] = { 1.36, 1.66, 1.51, 1.39, 1.60, 1.39, 1.16, 0.83, 1.10, 1.01, 1.00 };
    // const Float_t bLow[nSt] = { -345.40, -332.74, -324.52, -364.43, -5.74, -0.76, 10.83, 33.75, 5.49, 18.37, 0.00 };
    // const Float_t aUpp[nSt] = { 0.92, 0.99, 0.97, 0.84, 1.40, 1.30, 1.18, 0.93, 1.14, 1.07, 1.00 };
    // const Float_t bUpp[nSt] = { -373.27, -226.39, -259.46, -240.51, 64.50, 64.31, 1.91, 14.73, 4.15, 63.88, 0.00 };

    const Float_t aLow[nSt] = {1.00, 1.00, 1.00, 1.00, 1.66, 1.32, 1.10, 0.85, 1.09, 1.02, 1.00};
    const Float_t bLow[nSt] = {0.00, 0.00, 0.00, 0.00, -0.99, 2.79, 13.76, 25.16, 6.47, 17.96, 0.00};
    const Float_t aUpp[nSt] = {1.00, 1.00, 1.00, 1.00, 1.60, 1.48, 1.11, 0.93, 1.14, 0.78, 1.00};
    const Float_t bUpp[nSt] = {0.00, 0.00, 0.00, 0.00, -4.22, 1.43, 16.15, 14.73, 4.04, 162.81, 0.00};

  public:
    BmnHypNuclPairFinder();
    // hyp = "H3L"/"h3l" or "H4L"/"h4l"
    BmnHypNuclPairFinder(TString hyp, Bool_t exp = kTRUE);
    virtual ~BmnHypNuclPairFinder();

    virtual void Exec(Option_t* option);
    virtual InitStatus Init();
    virtual void Finish();

  private:
    void Analysis();

    Float_t FindV0ByVirtualPlanes(FairTrackParam* par1, FairTrackParam* par2, Float_t z_0, Float_t range = 50.);
    Float_t GetdEdx(CbmStsTrack* tr);
    void GetNHits(CbmStsTrack* tr, Int_t& nGEM, Int_t& nFSD);
    Float_t GetSigXTof700He3(FairTrackParam* par);
    Float_t GetSigYTof700He3(FairTrackParam* par);
    Float_t GetDyTof700(FairTrackParam* par);
    Float_t GetDxTof700(FairTrackParam* par);

    map<Float_t, pair<Int_t, Int_t>> FindPairs(TClonesArray* tracks, TClonesArray* hits);

    ClassDef(BmnHypNuclPairFinder, 0)
};

#endif
