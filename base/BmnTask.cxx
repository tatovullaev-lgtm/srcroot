#include "BmnTask.h"

// BmnTask::BmnTask() : FairTask() {}

// BmnTask::BmnTask(const char *name, Int_t iVerbose) : FairTask(name, iVerbose) {}

BmnTask::BmnTask()
: FairTask(),
  fOnlineActive(kTRUE)
{}

BmnTask::BmnTask(const char *name, Int_t iVerbose) : FairTask(name, iVerbose), fOnlineActive(kTRUE)
{}


BmnTask::~BmnTask() {}
