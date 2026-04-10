// ---------------------------------------------------------------------------
// -----                    BmnTrackDrawH header file
// -----                created 05/10/15 by K. Gertsenberger
// ----- class to visualize BmnTrack* collection by hits of neighbour bramch
// ---------------------------------------------------------------------------
#ifndef BMNTRACKDRAWH_H
#define BMNTRACKDRAWH_H

#include "FairTask.h"
#include "MpdEventManager.h"
#include "TClonesArray.h"
#include "TEveTrack.h"
#include "TEveTrackPropagator.h"
#include "TObjArray.h"
#include "TParticle.h"

class BmnTrackDrawH : public FairTask
{
  public:
    /** Default constructor **/
    BmnTrackDrawH();

    /** Standard constructor
     *@param name        Name of task and branch with BmnTrack* collection
     *@param iVerbose    Verbosity level
     **/
    BmnTrackDrawH(const char* name, TString hitsBranchName, Int_t iVerbose = 0);

    /** Destructor **/
    virtual ~BmnTrackDrawH();

    /** Set verbosity level. For this task and all of the subtasks. **/
    void SetVerbose(Int_t iVerbose) { fVerbose = iVerbose; }
    /** execute function of this task **/
    virtual void Exec(Option_t* option);

    // initialization of the track drawing task
    virtual InitStatus Init();
    virtual void SetParContainers();
    void Reset();
    virtual void Finish();

    // return pointer to EVE track list for given group_name and/or particle name. if list don't exist then create it
    TEveTrackList* GetTrGroup(const char* group_name, TParticle* P = nullptr);

  protected:
    // tracks collection
    TClonesArray* fTrackList;   //!
    // hits collection corresponding tracks
    TClonesArray* fHitList;   //!

    // EVE track propagator
    TEveTrackPropagator* fTrPr;
    MpdEventManager* fEventManager;   //!
    TObjArray* fEveTrList;            //!
    TEveTrackList* fTrList;           //!
    TString fHitsBranchName;

    Bool_t fIsCbmStsTrack;

  private:
    BmnTrackDrawH(const BmnTrackDrawH&) = delete;
    BmnTrackDrawH& operator=(const BmnTrackDrawH&) = delete;

    ClassDef(BmnTrackDrawH, 1);
};
#endif
