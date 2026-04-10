#ifndef BMNSiProfHITMAKER_H
#define BMNSiProfHITMAKER_H 1

#include "BmnTask.h"
#include "BmnSiProfDigit.h"
#include "BmnSiProfHit.h"
#include "BmnSiProfStationSet.h"
#include "BmnSiProfConfiguration.h"
#include "BmnSiProfTransform.h"
#include <BmnEventQuality.h>

#include "FairField.h"
#include "FairMCPoint.h"

#include "TClonesArray.h"
#include "TRegexp.h"
#include "TString.h"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

class BmnSiProfHitMaker : public BmnTask {
public:

    BmnSiProfHitMaker();
    BmnSiProfHitMaker(Int_t, Int_t, Bool_t, TString alignFile = "default");

    virtual ~BmnSiProfHitMaker();

    virtual InitStatus Init();

    virtual void Exec(Option_t* opt);

    virtual void Finish();

    void ProcessDigits();

    void SetHitMatching(Bool_t opt = kTRUE) {
        fHitMatching = opt;
    }

    void SetCurrentConfig(BmnSiProfConfiguration::SiProf_CONFIG config) {
        fCurrentConfig = config;
    }

    virtual void OnlineWrite(const std::unique_ptr<TTree> &resultTree);

private:

    TString fInputPointsBranchName;
    TString fInputDigitsBranchName;
    TString fInputDigitMatchesBranchName;

    TString fOutputHitsBranchName;
    TString fOutputHitMatchesBranchName;

    /** Input array of SiProf Points **/
    TClonesArray* fBmnSiProfPointsArray;          //!
    TClonesArray* fBmnSiProfDigitsArray;          //!
    TClonesArray* fBmnSiProfDigitMatchesArray;    //!

    /** Output array of SiProf Hits **/
    TClonesArray* fBmnSiProfHitsArray;            //!

    /** Output array of SiProf Hit Matches **/
    TClonesArray* fBmnSiProfHitMatchesArray;      //!

    Bool_t fHitMatching;
    Bool_t fIsExp; // Specify type of input data (MC or real data)

    BmnSiProfConfiguration::SiProf_CONFIG fCurrentConfig;

    BmnSiProfStationSet *StationSet;  //!Entire SiProf detector

    BmnSiProfTransform *TransfSet;    //! Transformations for each module of the detector

    FairField* fField;              //!

    TString fBmnEvQualityBranchName;
    TClonesArray* fBmnEvQuality;    //!

    void LoadDetectorConfiguration();

  ClassDef(BmnSiProfHitMaker, 1);
};

#endif
