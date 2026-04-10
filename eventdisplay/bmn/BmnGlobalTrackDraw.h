// -------------------------------------------------------------------------
// -----                    BmnGlobalTrackDraw header file -----
// -----              created 10/12/13 by K. Gertsenberger             -----
// ----- class to visualize reconstructed GlobalTracks in EventDisplay -----
// -------------------------------------------------------------------------

#ifndef BmnGlobalTrackDraw_H
#define BmnGlobalTrackDraw_H

#include "FairTask.h"
#include "MpdEventManager.h"
#include "TClonesArray.h"
#include "TEveTrack.h"
#include "TEveTrackPropagator.h"
#include "TObjArray.h"
#include "TParticle.h"

class BmnGlobalTrackDraw : public FairTask
{
  public:
    // default constructor
    BmnGlobalTrackDraw();

    // constructor: @name - name of task, @iVerbose- verbosity level
    BmnGlobalTrackDraw(const char* name, Int_t iVerbose = 0);

    // destructor
    virtual ~BmnGlobalTrackDraw();

    // set verbosity level for this task and all of the subtasks
    void SetVerbose(Int_t iVerbose) { fVerbose = iVerbose; }
    // execute function of this task
    virtual void Exec(Option_t* option);
    // initialization of the track drawing task
    virtual InitStatus Init();
    virtual void SetParContainers();

    // action after each event processing
    virtual void Finish();
    void Reset();

    // return pointer to EVE track list for given group_name and/or particle name. if list don't exist then create it
    TEveTrackList* GetTrGroup(const char* group_name, TParticle* P = nullptr);

  protected:
    // global tracks collection
    unique_ptr<TClonesArray> fTrackList;   //!

    // Upstream hits collection
    unique_ptr<TClonesArray> fUpstreamHitList;   //!
    // Upstream tracks collection
    unique_ptr<TClonesArray> fUpstreamTrackList;   //!
    // Silicon hits collection
    unique_ptr<TClonesArray> fSiliconHitList;   //!
    // Silicon tracks collection
    unique_ptr<TClonesArray> fSiliconTrackList;   //!
    // GEM hits collection
    unique_ptr<TClonesArray> fGemHitList;   //!
    // GEM tracks collection
    unique_ptr<TClonesArray> fGemTrackList;   //!
    // Sts hits collection
    unique_ptr<TClonesArray> fStsHitList;   //!
    // Sts tracks collection
    unique_ptr<TClonesArray> fStsTrackList;   //!
    // StsVector tracks collection
    unique_ptr<TClonesArray> fStsVectorTrackList;   //!
    // CSC hits collection
    unique_ptr<TClonesArray> fCscHitList;   //!
    // TOF1 hits collection
    unique_ptr<TClonesArray> fTof1HitList;   //!
    // TOF2 hits collection
    unique_ptr<TClonesArray> fTof2HitList;   //!
    // DCH tracks collection
    unique_ptr<TClonesArray> fDchTrackList;   //!
    // DCH hits collection
    unique_ptr<TClonesArray> fDchHitList;   //!

    // EVE track propagator
    TEveTrackPropagator* fTrPr;
    MpdEventManager* fEventManager;   //!
    TObjArray* fEveTrList;
    TEveTrackList* fTrList;   //!

  private:
    BmnGlobalTrackDraw(const BmnGlobalTrackDraw&) = delete;
    BmnGlobalTrackDraw& operator=(const BmnGlobalTrackDraw&) = delete;

    // an order of detectors for the given configuration
    vector<Int_t> vOrderOfDetectors;   //!

    ClassDef(BmnGlobalTrackDraw, 1);
};
#endif
