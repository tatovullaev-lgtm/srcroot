#ifndef BMNSiProfDIGITIZER_H
#define BMNSiProfDIGITIZER_H 1

#include <iostream>

#include "Rtypes.h"
#include "TClonesArray.h"

#include "FairTask.h"
#include "FairMCPoint.h"

#include "BmnSiProfDigit.h"
#include "BmnSiProfHit.h"
#include "BmnSiProfStationSet.h"
#include "BmnSiProfConfiguration.h"
#include "BmnSiProfTransform.h"

using namespace std;

class BmnSiProfDigitizer : public FairTask {
public:

    BmnSiProfDigitizer();

    virtual ~BmnSiProfDigitizer();

    virtual InitStatus Init();

    virtual void Exec(Option_t* opt);

    virtual void Finish();

    void ProcessMCPoints();

    //Setters

    void SetOnlyPrimary(Bool_t opt = kFALSE) {
        fOnlyPrimary = opt;
    }

    void SetStripMatching(Bool_t opt = kTRUE) {
        fStripMatching = opt;
    }

    void SetCurrentConfig(BmnSiProfConfiguration::SiProf_CONFIG config) {
        fCurrentConfig = config;
    }

private:

    TString fInputBranchName;
    TString fOutputDigitsBranchName;
    TString fOutputDigitMatchesBranchName;

    /** Input array of SiProf Points **/
    TClonesArray* fBmnSiProfPointsArray;

    /** Input array of MC Tracks **/
    TClonesArray* fMCTracksArray;

    /** Output array of SiProf Digits **/
    TClonesArray* fBmnSiProfDigitsArray;

    /** Output array of SiProf Digit Matches **/
    TClonesArray* fBmnSiProfDigitMatchesArray;

    Bool_t fOnlyPrimary;
    Bool_t fStripMatching;

    BmnSiProfConfiguration::SiProf_CONFIG fCurrentConfig;

    BmnSiProfStationSet *StationSet; //Entire SiProf detector

    BmnSiProfTransform *TransfSet; //Transformations for each module of the detector

    ClassDef(BmnSiProfDigitizer,1);
};

#endif
