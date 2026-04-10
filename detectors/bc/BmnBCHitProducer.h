#ifndef BmnBCHitProducer_H
#define BmnBCHitProducer_H

#include "BmnBCHit.h"
#include "FairMCPoint.h"
#include "FairRunSim.h"
#include "FairTask.h"
#include "Rtypes.h"
#include "TClonesArray.h"

#include <iostream>

using namespace std;

class BmnBCHitProducer : public FairTask
{
  public:
    BmnBCHitProducer();
    virtual ~BmnBCHitProducer();

    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);
    virtual void Finish();

    // void ProcessMCPoints();

  private:
    TString fInputBranchName;
    TString fOutputHitsBranchName;
    TString fOutputHitMatchesBranchName;

    /** Input array of BC Points **/
    TClonesArray* fBmnBCPointsArray;   //!

    /** Output array of BC Hits **/
    TClonesArray* fBmnBCHitsArray;   //!

    Bool_t fOnlyPrimary;
    Bool_t fUseRealEffects;

    ClassDef(BmnBCHitProducer, 1);
};

#endif /*  BmnBCHitProducer_H */
