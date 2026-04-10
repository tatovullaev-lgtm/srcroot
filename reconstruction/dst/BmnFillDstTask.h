/********************************************************************************
 *    BmnFillDstTask.h                                                          *
 *    BM@N Fill DST Task class declaration                                      *
 *    Add necessary data and event header to the DST file after reconstruction  *
 *******************************************************************************/

#ifndef BMNFILLDSTTASK_H
#define BMNFILLDSTTASK_H

#include "BmnEventHeader.h"
#include "BmnMCInfoDst.h"
#include "BmnTask.h"
#include "CbmVertex.h"
#include "DstEventHeader.h"
#include "DstRunHeader.h"
#include "FairMCEventHeader.h"
#include "TClonesArray.h"

#include <map>

class BmnFillDstTask : public BmnTask
{
  public:
    /** Default constructor **/
    BmnFillDstTask();

    /** Constructor with start event and event number to be processed, and autodefinition whether experimental data are
     * used **/
    BmnFillDstTask(Long64_t nStartEvent, Long64_t nEvents);

    /** Constructor with start event and event number to be processed, experimental data flag, period and run numbers
     * **/
    BmnFillDstTask(Long64_t nStartEvent,
                   Long64_t nEvents,
                   Bool_t isExp,
                   Int_t period_number = -1,
                   Int_t run_number = -1);

    /** Destructor **/
    ~BmnFillDstTask();

    /** Initialization of task at the beginning **/
    virtual InitStatus Init();

    /** ReInitialization of task when the runID/file changes **/
    virtual InitStatus ReInit();

    /** Executed for each event **/
    virtual void Exec(Option_t* opt);

    /** Load the parameter container from the runtime database **/
    virtual void SetParContainers();

    /** Finish task called at the end **/
    virtual void Finish();

    /** Setting period-number information to fill RunHeader **/
    void SetRunNumber(Int_t period_number, Int_t run_number)
    {
        fPeriodNumber = period_number;
        fRunNumber = run_number;
    }
    void DoZCalibration(Bool_t cal) { fDoCalibration = cal; }

    /** Fill map with weight-charge of possible particles **/
    void InitParticleInfo();

    struct stParticleInfo
    {
        Int_t A;
        Int_t Z;
    };

  private:
    int isExpData;
    /** period number **/
    Int_t fPeriodNumber;
    /** run number **/
    Int_t fRunNumber;

    /** Input MCEventHeader from Simulation File **/
    FairMCEventHeader* fMCEventHeader;   //!
    /** Input Monte-Carlo tracks from Simulation File **/
    TClonesArray* fMCTracks;   //!
    /** Output MC Information for physics analysis **/
    BmnMCInfoDst* fMCInfoDst;   //!
    /** Input BmnEventHeader from Experimental Digit File **/
    BmnEventHeader* fEventHeader;   //!
    /** Output DstEventHeader prepared in FairRunAna **/
    DstEventHeader* fDstHeader;   //!
    /** Output BmnRunHeader **/
    DstRunHeader* fRunHeader;   //!
    /** start event from which the processing is started **/
    Long64_t fNStartEvent;
    /** event count to be processed **/
    Long64_t fNEvents;
    /** current event being processed for progress bar **/
    Long64_t fIEvent;
    /** pointer to primary vertex in the current event **/
    CbmVertex* fPrimaryVertex;
    /** pointer to global tracks in the current event **/
    TClonesArray* fGlobalTracks;

    /** z calibration parameters: **/
    Double_t fZCalib1;
    Double_t fZCalib2;
    Double_t fBC1Calib;
    Double_t fBC2Calib;
    Bool_t fDoCalibration;

    /*For ADC, charge calculation*/
    TClonesArray *fT0, *fBC1, *fBC2, *fBC3, *fBC4;   //!

    /** map with particle names and corresponding weight-charge pairs **/
    map<TString, stParticleInfo> mapParticleInfo;

    BmnFillDstTask(const BmnFillDstTask&) = delete;
    BmnFillDstTask operator=(const BmnFillDstTask&) = delete;

    /** Reset variables between events **/
    void Reset();

    void applyZCalibration();
    /** set headers with autodefinition whether experimental or simulation data are used **/
    bool autoSetHeaders();

    ClassDef(BmnFillDstTask, 2);
};

#endif
