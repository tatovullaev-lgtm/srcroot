/********************************************************************************
 *    BmnFillDstTask.h                                                          *
 *    BM@N Fill DST Task class declaration                                      *
 *    Fill Event Headers in the DST file after reconstruction                   *
 *    Konstantin Gertsenberger                                                  *
 *    Created: Apr. 25 2019                                                     *
 *******************************************************************************/

#ifndef BMNFILLDSTTASK_H 
#define BMNFILLDSTTASK_H

#include "DstRunHeader.h"
#include "DstEventHeader.h"
#include "BmnEventHeader.h"
#include "/lustre/home/user/b/bochkova/bmnroot/detectors/tofcal/BmnTofCalDigit.h"

#include "FairTask.h"
#include "FairMCEventHeader.h"

#include "TClonesArray.h"

#include <map>

class BmnFillDstTask : public FairTask {
   public:
    /** Default constructor **/
    BmnFillDstTask();

    /** Constructor with the given event number to be processed
     ** in order to activate printing only progress bar in terminal **/
    BmnFillDstTask(Long64_t nEvents);

    /** Constructor with input Event Header Name and event number to be processed
     ** in order to activate printing only progress bar in terminal (if not equal -1) **/
    BmnFillDstTask(TString input_event_header_name, Long64_t nEvents = -1);

    /** Constructor with input and output Event Header Name, and event number to be processed
     ** in order to activate printing only progress bar in terminal (if not equal -1) **/
    BmnFillDstTask(TString input_event_header_name, TString output_event_header_name, Long64_t nEvents = -1);

    /** Destructor **/
    ~BmnFillDstTask();

    /** Initiliazation of task at the beginning **/
    virtual InitStatus Init();

    /** ReInitiliazation of task when the runID/file changes **/
    virtual InitStatus ReInit();

    /** Executed for each event **/
    virtual void Exec(Option_t* opt);

    /** Load the parameter container from the runtime database **/
    virtual void SetParContainers();

    /** Finish task called at the end **/
    virtual void Finish();

    /** Setting period-number information to fill RunHeader **/
    void SetRunNumber(Int_t period_number, Int_t run_number) {
        fPeriodNumber = period_number;
        fRunNumber = run_number;
    }
    void DoZCalibration(Bool_t cal) {
        fDoCalibration = cal;
    }

    /** Fill map with weight-charge of possible particles **/
    void InitParticleInfo();

    struct stParticleInfo {
        Int_t A;
        Int_t Z;
    };

   private:
    TString fInputEventHeaderName;
    TString fOutputEventHeaderName;

    /** Input MCEventHeader from Simulation File **/
    FairMCEventHeader* fMCEventHead;
    /** Input BmnEventHeader from Digit File **/
    BmnEventHeader* fEventHead;
    /** whether input file contains simulation data **/
    Bool_t isSimulationInput;
    /** Output DstEventHeader prepared in FairRunAna **/
    DstEventHeader* fDstHead;
    /** Output BmnRunHeader **/
    DstRunHeader* fRunHead;
    /*For ADC, charge calculation*/
    TClonesArray *fT01_1, *fT01_2, *fBC1_1, *fBC1_2, *fBC2_1, *fBC2_2, *fBC3_1, *fBC3_2, *fBC3_S, *fBC4_1, *fBC4_2, *fBC4_S, *fBC5_1, *fBC5_2, *fBC5_S, *fVeto;
    TClonesArray *fBC3_1_X10, *fBC3_2_X10, *fBC4_1_X10, *fBC4_2_X10, *fBC5_1_X10, *fBC5_2_X10, *fTofCal;
    /** event count to be processed for progress bar **/
    Long64_t fNEvents;
    /** current event being processed for progress bar **/
    Long64_t fIEvent;

    /** period number **/
    Int_t fPeriodNumber;
    /** run number **/
    Int_t fRunNumber;



    /** z calibration parameters: **/
    Double_t fZCalib1;
    Double_t fZCalib2;
    Double_t fBC1Calib;
    Double_t fBC2Calib;
    Bool_t fDoCalibration;

    /** map with particle names and corresponding weight-charge pairs **/
    map<TString, stParticleInfo> mapParticleInfo;

    BmnFillDstTask(const BmnFillDstTask&) = delete;
    BmnFillDstTask operator=(const BmnFillDstTask&) = delete;

    ClassDef(BmnFillDstTask, 1);
};

#endif
                                                                                                                                                                                                                                                                                                                                                                                                                  
