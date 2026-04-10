#ifndef BMNSPILLINFO_H
#define BMNSPILLINFO_H

// #include <chrono>
//  BmnRoot
#include "SpillStatus.h"

// using SysClock = std::chrono::system_clock;
// using SysPoint = std::chrono::time_point<SysClock>;

struct BmnSpillInfo
{
    SysPoint start_ts;
    SysPoint stop_ts;
    SpillCntrs<SpillStatus::counters_cnt> counters;
    std::vector<SpillStatus> vec_spill_status;
    //    TTimeStamp start_ts;
    //    TTimeStamp stop_ts;
    //    TTimeStamp record_ts;
    //    ClassDef(SpillInfo, 1);
};
#endif /* BMNSPILLINFO_H */
