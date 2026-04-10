#ifndef BMNVSPDIGITIZER_H
#define BMNVSPDIGITIZER_H

#include "BmnVSPConfiguration.h"
#include "BmnVSPDigit.h"
#include "BmnVSPStationSet.h"
#include "BmnVSPTransform.h"
#include "FairField.h"
#include "FairMCPoint.h"
#include "FairRunSim.h"
#include "FairTask.h"
#include "Rtypes.h"
#include "TClonesArray.h"

#include <iostream>

using namespace std;

class BmnVSPDigitizer : public FairTask
{
  public:
    BmnVSPDigitizer();

    virtual ~BmnVSPDigitizer();

    virtual InitStatus Init();

    virtual void Exec(Option_t* opt);

    virtual void Finish();

    void ProcessMCPoints();

    // Setters

    void SetOnlyPrimary(Bool_t opt = kFALSE) { fOnlyPrimary = opt; }

    void SetStripMatching(Bool_t opt = kTRUE) { fStripMatching = opt; }

    void SetUseRealEffects(Bool_t opt = kTRUE) { fUseRealEffects = opt; }

    void SetCurrentConfig(BmnVSPConfiguration::VSP_CONFIG config) { fCurrentConfig = config; }

    void SetNoise(Double_t sigma, Double_t thresh)
    {
        fSigma = sigma;
        fThresh = thresh;
    }   // AZ-010725 - set noise sigma and threshold

  private:
    TString fInputBranchName;
    TString fOutputDigitsBranchName;
    TString fOutputDigitMatchesBranchName;

    /** Input array of VSP Points **/
    TClonesArray* fBmnVSPPointsArray;

    /** Input array of MC Tracks **/
    TClonesArray* fMCTracksArray;

    /** Output array of VSP Digits **/
    TClonesArray* fBmnVSPDigitsArray;

    /** Output array of VSP Digit Matches **/
    TClonesArray* fBmnVSPDigitMatchesArray;

    Bool_t fOnlyPrimary;
    Bool_t fStripMatching;
    Bool_t fUseRealEffects;

    FairField* fField;

    BmnVSPConfiguration::VSP_CONFIG fCurrentConfig;
    BmnVSPStationSet* StationSet;   // Entire detector
    BmnVSPTransform* TransfSet;     // Transformations for detector modules

    Double_t fSigma;    // AZ-010725
    Double_t fThresh;   // AZ-010725

    ClassDef(BmnVSPDigitizer, 1);
};

#endif /* BMNVSPDIGITIZER_H */
