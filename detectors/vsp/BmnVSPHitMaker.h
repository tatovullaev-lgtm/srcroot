#ifndef BMNVSPHITMAKER_H
#define BMNVSPHITMAKER_H

#include "BmnTask.h"
#include "BmnVSPConfiguration.h"
#include "BmnVSPDigit.h"
#include "BmnVSPHit.h"
#include "BmnVSPLayer.h"
#include "BmnVSPModule.h"
#include "BmnVSPStation.h"
#include "BmnVSPStationSet.h"
#include "BmnVSPTransform.h"
#include "FairMCPoint.h"

using namespace std;
using namespace TMath;

class BmnVSPHitMaker : public BmnTask
{
  public:
    BmnVSPHitMaker();
    BmnVSPHitMaker(Int_t run_period, Int_t run_number, Bool_t isExp);

    virtual ~BmnVSPHitMaker();

    virtual InitStatus Init();

    virtual void Exec(Option_t* opt);

    virtual void Finish();

    void ProcessDigits();

    void SetHitMatching(Bool_t opt = kTRUE) { fHitMatching = opt; }

    void SetCurrentConfig(BmnVSPConfiguration::VSP_CONFIG config) { fCurrentConfig = config; }

    void SetSignalWindow(Double_t min, Double_t max)
    {
        fSignalLow = min;
        fSignalUp = max;
    }

    virtual void OnlineWrite(const std::unique_ptr<TTree>& resultTree);

  private:
    void createVSPDetector();

    Int_t fRunPeriod;
    Int_t fRunNumber;

    TString fInputPointsBranchName;
    TString fInputDigitsBranchName;
    TString fInputDigitMatchesBranchName;

    TString fOutputHitsBranchName;

    /** Input array of VSP Points **/
    TClonesArray* fBmnVSPPointsArray;         //!
    TClonesArray* fBmnVSPDigitsArray;         //!
    TClonesArray* fBmnVSPDigitMatchesArray;   //!

    /** Output array of VSP Hits **/
    TClonesArray* fBmnVSPHitsArray;   //!

    /** Output array of VSP Upper Clusters **/
    TClonesArray* fBmnVSPUpperClustersArray;   //!
    /** Output array of VSP Lower Clusters **/
    TClonesArray* fBmnVSPLowerClustersArray;   //!

    Bool_t fHitMatching;

    BmnVSPConfiguration::VSP_CONFIG fCurrentConfig;
    BmnVSPStationSet* StationSet;   //! Entire detector
    BmnVSPTransform* TransfSet;     //! Transformations for detector modules

    Bool_t fIsExp;
    TString fBmnEvQualityBranchName;
    TClonesArray* fBmnEvQuality;   //!

    Double_t*** fAlignCor;   //!

    Double_t fSignalLow;
    Double_t fSignalUp;

    ClassDef(BmnVSPHitMaker, 1);
};

#endif /* BMNVSPHITMAKER_H */
