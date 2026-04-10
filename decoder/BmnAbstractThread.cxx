#include "BmnAbstractThread.h"

#include <mutex>

int32_t BmnAbstractThread::curThreadCount = 0;
int32_t BmnAbstractThread::curThreadWaiting = 0;
std::mutex BmnAbstractThread::statVarAccess;
const bool BmnAbstractThread::calcStatistics = false;