#ifndef BMNPROFQA_H_
#define BMNPROFQA_H_

#include <time.h>
#include <chrono>
#include <memory>
#include <bitset>

#include <TTree.h>
#include <TMethod.h>

#include "FairTask.h"

#include "CbmVertex.h"
#include "DstEventHeader.h"
#include "BmnSiProfDigit.h"
#include "BmnSiProfStationSet.h"

#include "BmnQaBase.h"
#include "BmnProfQaReport.h"

class BmnHistManager;
class BmnTrackMatch;
class BmnGlobalElectronId;

using std::string;
using std::vector;

class BmnProfQa : public BmnQaBase {
public:

    /**
     * \brief Constructor.
     */
    BmnProfQa() {
    };

    /**
     * \brief Constructor.
     */
    BmnProfQa(TString name, TString gemConf);

    /**
     * \brief Destructor.
     */
    virtual ~BmnProfQa();

    /**
     * \brief Derived from FairTask.
     */
    virtual InitStatus Init();

    /**
     * \brief Derived from FairTask.
     */
    virtual void Exec(
            Option_t* opt);

    /**
     * \brief Derived from FairTask.
     */
    virtual void Finish();

    /** Setters **/

    void SetOutputDir(const std::string& dir) {
        fOutputDir = dir;
    }

    void SetInnerTrackerSetup(map<DetectorId, Bool_t> setup) {
        fInnerTrackerSetup = setup;
    };

    void SetDetectorPresence(DetectorId det, Bool_t use) {
        fInnerTrackerSetup[det] = use;
    };

    void SetInnerTracksBranchName(TString name) {
        fInnerTrackBranchName = name;
    }
    
    void ResetProf();
    
    void RegisterCommands();

private:
    /**
     * \brief Read data branches from input data files.
     */
    void ReadDataBranches();
    void CreateHistograms();
    void ProcessGlobal();

    map<DetectorId, Bool_t> fInnerTrackerSetup;

    // Pointers to data arrays
    TClonesArray* fProfHits;
    TClonesArray* fProfDigits;
    
    DstEventHeader* fDstEventHeader;

    TString fConfigProf;
    unique_ptr<BmnSiProfStationSet> fProfDetector;
    unique_ptr<BmnGemStripStationSet> fGemDetector;
    unique_ptr<BmnSiliconStationSet> fSilDetector;
    unique_ptr<BmnCSCStationSet> fCscDetector;

    TString fInnerTrackBranchName;


    const Int_t fNItersToUpdate = 200;
    const chrono::seconds fTimeToUpdate = chrono::seconds(5); //<-redraw each timeout seconds
    Int_t fNItersSinceUpdate;
    chrono::time_point<chrono::system_clock> fTicksLastUpdate;

    BmnProfQa(const BmnProfQa&) = delete;
    BmnProfQa& operator=(const BmnProfQa&) = delete;

    ClassDef(BmnProfQa, 1);
};

#endif /* BMNPROFQA_H_ */
