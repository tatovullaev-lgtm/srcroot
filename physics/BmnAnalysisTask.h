#ifndef BMNANALYSISTASK_H
#define BMNANALYSISTASK_H

#include "FairTask.h"
#include "TClonesArray.h"

class BmnAnalysisTask : public FairTask
{
  public:
    /** Default constructor **/
    BmnAnalysisTask();

    /** Constructor with parameters (Optional) **/
    //  BmnAnalysisTask(Int_t verbose);

    /** Destructor **/
    ~BmnAnalysisTask();

    /** Initiliazation of task at the beginning of a run **/
    virtual InitStatus Init();

    /** ReInitiliazation of task when the runID changes **/
    virtual InitStatus ReInit();

    /** Executed for each event. **/
    virtual void Exec(Option_t* opt);

    /** Load the parameter container from the runtime database **/
    virtual void SetParContainers();

    /** Finish task called at the end of the run **/
    virtual void Finish();

  private:
    /** Input array from previous already existing data level **/
    //  TClonesArray* <InputDataLevel>;

    /** Output array to  new data level**/
    //  TClonesArray* <OutputDataLevel>;

    BmnAnalysisTask(const BmnAnalysisTask&);
    BmnAnalysisTask operator=(const BmnAnalysisTask&);

    ClassDef(BmnAnalysisTask, 1);
};

#endif
