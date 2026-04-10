/**
 * \file BmnTrackingQaExp.h
 * \brief FairTask for tracking performance calculation.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - original author for CBM experiment
 * \author Sergey Merts <sergey.merts@gmail.com> - modifications for BMN experiment
 * \author Ilnur Gabdrakhmanov <ilnur@jinr.ru> - disentangle Exp and MC processing
 * \date 2007-2022
 */

#ifndef BMNTRACKINGQAEXP_H_
#define BMNTRACKINGQAEXP_H_

#include "BmnQaBase.h"
#include "BmnTrackingQaExpReport.h"
#include "CbmVertex.h"
#include "DstEventHeader.h"
#include "FairTask.h"

#include <TMethod.h>
#include <TTree.h>
#include <chrono>
#include <memory>
#include <time.h>

class BmnHistManager;
class BmnTrackMatch;
class BmnGlobalElectronId;

using std::multimap;
using std::string;
using std::vector;

class BmnTrackingQaExp : public BmnQaBase
{
  public:
    /**
     * \brief Constructor.
     */
    BmnTrackingQaExp() {};

    /**
     * \brief Constructor.
     */
    BmnTrackingQaExp(TString name, UInt_t periodId = 9, BmnSetup std = kBMNSETUP);

    /**
     * \brief Destructor.
     */
    virtual ~BmnTrackingQaExp();

    /**
     * \brief Derived from FairTask.
     */
    virtual InitStatus Init();

    /**
     * \brief Derived from FairTask.
     */
    virtual void Exec(Option_t* opt);

    /**
     * \brief Derived from FairTask.
     */
    virtual void Finish();

    /** Setters **/

    void SetOutputDir(const std::string& dir) { fOutputDir = dir; }

    void SetInnerTrackerSetup(map<DetectorId, Bool_t> setup) { fInnerTrackerSetup = setup; };

    void SetDetectorPresence(DetectorId det, Bool_t use = true) { fInnerTrackerSetup[det] = use; };

    void SetInnerTracksBranchName(TString name) { fInnerTrackBranchName = name; }

    void ResetSiBT();

    void RegisterCommands();

    void CreateTofHits(DetectorId detId);
    void FillTofHits(TClonesArray* tof_hits, DetectorId detId);

  private:
    /**
     * \brief Read data branches from input data files.
     */
    void ReadDataBranches();
    void CreateHistograms();
    void ProcessGlobal();

    map<DetectorId, Bool_t> fInnerTrackerSetup;
    UInt_t fPeriodId;
    UInt_t fRunId;
    BmnSetup fSetup;

    // Pointers to data arrays
    //    TClonesArray* fMCTracks; // CbmMCTrack array
    TClonesArray* fGlobalTracks;   // BmnGlobalTrack array
    TClonesArray* fStsTracks;
    TClonesArray* fSiBTTracks;
    TClonesArray* fStsHits;
    TClonesArray* fGemHits;
    TClonesArray* fSilHits;
    TClonesArray* fSiBTHits;
    TClonesArray* fTof400Hits;
    TClonesArray* fTof700Hits;
    TClonesArray* fCscHits;
    TClonesArray* fVspHits;
    TClonesArray* fDchTracks;

    DstEventHeader* fDstEventHeader;

    CbmVertex* fVertexL1;   // L1 tracking
                            //    TString fConfigGem;
                            //    TString fConfigSil;
                            //    TString fConfigCsc;
    unique_ptr<BmnGemStripStationSet> fGemDetector;
    unique_ptr<BmnSiliconStationSet> fSilDetector;
    unique_ptr<BmnCSCStationSet> fCscDetector;
    unique_ptr<BmnVSPStationSet> fVspDetector;

    TString fInnerTrackBranchName;

    const Int_t fNItersToUpdate = 200;
    const chrono::seconds fTimeToUpdate = chrono::seconds(5);   //<-redraw each timeout seconds
    Int_t fNItersSinceUpdate;
    chrono::time_point<chrono::system_clock> fTicksLastUpdate;

    BmnTrackingQaExp(const BmnTrackingQaExp&) = delete;
    BmnTrackingQaExp& operator=(const BmnTrackingQaExp&) = delete;

    ClassDef(BmnTrackingQaExp, 1);
};

#endif /* BMNTRACKINGQAEXP_H_ */
