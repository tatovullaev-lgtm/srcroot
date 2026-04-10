// Preliminary version of BmnVSPHitProducer

#ifndef BMNVSPHITPRODUCER_H
#define BMNVSPHITPRODUCER_H 1

#include <math.h>
#include <iostream>
#include <vector>
#include "TString.h"
#include "TClonesArray.h"
#include "FairTask.h"
#include "BmnVSPStationSet.h"
#include "BmnVSPConfiguration.h"


#include "CbmStsPoint.h"
#include "CbmStsTrack.h"

#include "BmnHit.h"

using namespace std;

class BmnVSPHitProducer : public FairTask {
public:

    /** Default constructor **/
    BmnVSPHitProducer();

    /** Destructor **/
    virtual ~BmnVSPHitProducer();

    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);

    /** Virtual method Finish **/
    virtual void Finish();

private:

    TString fInputBranchName;
    TString fTracksBranchName;
    TString fOutputHitsBranchName;
    TString fOutputVSPHitMatchesBranchName;

    /** Input array of VSP Points **/
    TClonesArray* fBmnPointsArray;

    /** Input array of MC Tracks **/
    TClonesArray* fMCTracksArray;

    /** Output array of VSP Hits **/
    TClonesArray* fBmnHitsArray;

    /** Output array of VSP Hit Matches **/
    TClonesArray* fBmnVSPHitMatchesArray;

    BmnVSPStationSet *VSPStationSet; //Entire detector

    ClassDef(BmnVSPHitProducer, 1);

};

#endif
