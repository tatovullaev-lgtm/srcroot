#include "BmnAnalysisTask.h"

#include "FairRootManager.h"
#include "fairlogger/Logger.h"

// ---- Default constructor -------------------------------------------
BmnAnalysisTask::BmnAnalysisTask()
    : FairTask("BmnAnalysisTask")
{
    LOG(debug) << "Defaul Constructor of BmnAnalysisTask";
}

// ---- Destructor ----------------------------------------------------
BmnAnalysisTask::~BmnAnalysisTask()
{
    LOG(debug) << "Destructor of BmnAnalysisTask";
}

// ----  Initialisation  ----------------------------------------------
void BmnAnalysisTask::SetParContainers()
{
    LOG(debug) << "SetParContainers of BmnAnalysisTask";
    // Load all necessary parameter containers from the runtime data base
    /*
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb = ana->GetRuntimeDb();

    <BmnAnalysisTaskDataMember> = (<ClassPointer>*)
    (rtdb->getContainer("<ContainerName>"));
    */
}

// ---- Init ----------------------------------------------------------
InitStatus BmnAnalysisTask::Init()
{
    LOG(debug) << "Initilization of BmnAnalysisTask";

    // Get a handle from the IO manager
    // FairRootManager* ioman = FairRootManager::Instance();

    // Get a pointer to the previous already existing data level
    /*
    <InputDataLevel> = (TClonesArray*)ioman->GetObject("InputDataLevelName");
    if (!<InputLevel>)
    {
        LOG(error) << "No InputDataLevelName array!\n NewTask will be inactive";
        return kERROR;
    }
    */

    // Create the TClonesArray for the output data and register
    // it in the IO manager
    /*
    <OutputDataLevel> = new TClonesArray("OutputDataLevelName", 100);
    ioman->Register("OutputDataLevelName", "OutputDataLevelName", <OutputDataLevel>, kTRUE);
    */

    // Do whatever else is needed at the initilization stage
    // Create histograms to be filled
    // initialize variables

    return kSUCCESS;
}

// ---- ReInit  -------------------------------------------------------
InitStatus BmnAnalysisTask::ReInit()
{
    LOG(debug) << "Initialization of BmnAnalysisTask";
    return kSUCCESS;
}

// ---- Exec ----------------------------------------------------------
void BmnAnalysisTask::Exec(Option_t* /*option*/)
{
    LOG(debug) << "Exec of BmnAnalysisTask";
}

// ---- Finish --------------------------------------------------------
void BmnAnalysisTask::Finish()
{
    LOG(debug) << "Finish of BmnAnalysisTask";
}
