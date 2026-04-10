/* * \file BmnGlobalTracking.h
 * \author Sergey Merts <Sergey.Merts@gmail.com>
 * \date 2014
 * \brief BMN task for global track reconstruction.
 *
 * Output is reconstructed global tracks BmnGlobalTrack
 * Hit-to-track merger attaches the TOF hit finally the track is refitted.
 **/

#ifndef BmnGlobalTracking_H_
#define BmnGlobalTracking_H_

#include "BmnCSCHit.h"
#include "BmnCSCStationSet.h"
#include "BmnDchHit.h"
#include "BmnDchTrack.h"
#include "BmnGemTrack.h"
#include "BmnGlobalTrack.h"
#include "BmnHit.h"
#include "BmnKalmanFilter.h"
#include "BmnScWallEvent.h"
#include "BmnSiliconHit.h"
#include "BmnSiliconStationSet.h"
#include "BmnTask.h"
#include "BmnVertex.h"
#include "CbmKFTrack.h"
#include "CbmMCTrack.h"
#include "CbmStsHit.h"
#include "CbmStsTrack.h"
#include "CbmTofPoint.h"
#include "CbmVertex.h"
#include "FairMCPoint.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "TClonesArray.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TGraph.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"

#include <TStopwatch.h>
#include <string>
#include <vector>
using namespace std;

class BmnGlobalTracking : public BmnTask
{
  public:
    /**
     * \brief Constructor.
     */
    BmnGlobalTracking();
    BmnGlobalTracking(Bool_t isExp, Bool_t doAlign);

    /**
     * \brief Destructor.
     */
    virtual ~BmnGlobalTracking();

    /**
     * \brief Inherited from FairTask.
     */
    virtual InitStatus Init();

    /**
     * \brief Inherited from FairTask.
     */
    virtual void Exec(Option_t* opt);

    /**
     * \brief Inherited from FairTask.
     */
    virtual void Finish();

    virtual void OnlineWrite(const std::unique_ptr<TTree>& resultTree);

    void SetDoAlign(Bool_t a) { fDoAlign = a; };

    const Short_t GetDoAlign() const { return fDoAlign; };

    void SetRunNumber(Int_t r) { fRunId = r; }

    void SetPeriodNumber(Short_t p) { fPeriod = p; }

    void SetInnerTracksBranchName(TString name) { fInnerTrackBranchName = name; }

  private:
    /*
     * \brief Calculate length of the global track
     */
    Double_t CalculateLength(CbmStsTrack* tr);

    // INPUT ARRAYS
    TClonesArray* fInnerTracks;       //! GEM+SIL for BM@N
    TClonesArray* fGemTracks;         //!
    TClonesArray* fGemVertex;         //!
    TClonesArray* fGemHits;           //!
    TClonesArray* fSilHits;           //!
    TClonesArray* fCscHits;           //!
    TClonesArray* fDchTracks;         //!
    TClonesArray* fDchHits;           //!
    TClonesArray* fTof1Hits;          //!
    TClonesArray* fTof2Hits;          //!
    TClonesArray* fGemLowerCluster;   //!

    TClonesArray* fEvHead;     //!
    TClonesArray* fMCTracks;   //!

    TClonesArray* fStsHits;        //!
    TClonesArray* fStsTracks;      //!
    TClonesArray* fGlobalTracks;   //!

    BmnScWallEvent* fScWall;   //!

    TString fInnerTrackBranchName;

    Short_t fPeriod;
    Bool_t fIsExp;
    Bool_t fDoAlign;

    Int_t fEventNo;   // event counter
    Int_t fRunId;

    Int_t fPDG;          // PDG hypothesis
    Float_t fChiSqCut;   // Chi square cut for hit to be attached to track.

    BmnVertex* fVertex;     //! vertex information
    CbmVertex* fVertexL1;   //! vertex information for L1 case

    BmnKalmanFilter* fKalman;   //!
    Double_t fTime;

    const static Int_t nModTof700 = 60;
    const static Int_t nStripsTof700 = 32;
    Float_t timeShiftsTof700[nModTof700][nStripsTof700];

    const static Int_t nGemSt = 7;
    Float_t qCalib_a0[nGemSt];
    Float_t qCalib_b0[nGemSt];
    Float_t qCalib_a1[nGemSt];
    Float_t qCalib_b1[nGemSt];

    TF1* fitMeanPosResX_tof400;
    TF1* fitMeanPosResY_tof400;
    TF1* fitMeanNegResX_tof400;
    TF1* fitMeanNegResY_tof400;
    TF1* fitSigmaPosResX_tof400;
    TF1* fitSigmaPosResY_tof400;
    TF1* fitSigmaNegResX_tof400;
    TF1* fitSigmaNegResY_tof400;

    TF1* fitMeanPosResX_tof700;
    TF1* fitMeanPosResY_tof700;
    TF1* fitMeanNegResX_tof700;
    TF1* fitMeanNegResY_tof700;
    TF1* fitSigmaPosResX_tof700;
    TF1* fitSigmaPosResY_tof700;
    TF1* fitSigmaNegResX_tof700;
    TF1* fitSigmaNegResY_tof700;

    TFile* fTOF400_file;
    TFile* fTOF700_file;
    TFile* fCalib1_file;
    TFile* fCalib2_file;

    Long_t fNMatchedDch1;
    Long_t fNMatchedDch2;
    Long_t fNMatchedTof400;
    Long_t fNMatchedTof700;
    Long_t fNMatchedNearCsc;
    Long_t fNMatchedFarCsc;
    Long_t fNInnerTracks;
    Long_t fNGoodInnerTracks;
    Long_t fNGoodGlobalTracks;

    void PrintStatistics();

    BmnStatus MatchingTOF700();
    BmnStatus MatchingTOF400();
    BmnStatus MatchingScWall();
    BmnStatus MatchingFarCsc();
    BmnStatus MatchingDCH(BmnGlobalTrack* tr);
    BmnStatus MatchingDCH(BmnGlobalTrack* tr, Int_t num);
    // BmnStatus MatchingCSC(BmnGlobalTrack* glTr, vector<Int_t> stations);
    BmnStatus MatchingCSC(BmnGlobalTrack* glTr);

    map<Float_t, pair<Int_t, Int_t>> FindPairsTof400(TClonesArray* tracks,
                                                     TClonesArray* hits,
                                                     Float_t xCut,
                                                     Float_t yCut);
    map<Float_t, pair<Int_t, Int_t>> FindPairsTof700(TClonesArray* tracks,
                                                     TClonesArray* hits,
                                                     Float_t xCut,
                                                     Float_t yCut);
    Int_t FindNearestHit(FairTrackParam* par, TClonesArray* hits, Float_t xCut, Float_t yCut);
    Int_t FindNearestHit(FairTrackParam* par, TClonesArray* hits, Float_t xCut, Float_t yCut, vector<Int_t> stations);

    BmnStatus Refit(BmnGlobalTrack* tr);
    Double_t MagFieldIntegral(FairTrackParam& par, Double_t zMin, Double_t zMax, Double_t step);
    BmnStatus UpdateMomentum(BmnGlobalTrack* tr);

    BmnGlobalTracking(const BmnGlobalTracking&);

    void CalcdQdn(BmnGlobalTrack* tr);

    ClassDef(BmnGlobalTracking, 1);
};

#endif /* BmnGlobalTracking_H_ */
