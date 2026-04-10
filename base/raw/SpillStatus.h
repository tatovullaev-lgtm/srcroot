#ifndef SPILLSTATUS_H
#define SPILLSTATUS_H

#include "TObject.h"

#include <chrono>
#include <vector>
// BmnRoot
#include "SpillCntrs.h"

using SysClock = std::chrono::system_clock;
using SysPoint = std::chrono::time_point<SysClock>;
/**
 * Class SpillStatus
 * raw spill status from JSON block
 */
// template<UInt_t counters_cnt = 16>

struct SpillStatus : public TObject
{
    SpillStatus()
        : start_ts{SysPoint::min()}
        , stop_ts{SysPoint::min()}
        , device_ts{SysPoint::min()}
        , app_ts{SysPoint::min()}
        , cur_spill_time_ms{0}
        , spill_cnt{0}
        , phase{0}
        , times_valid{false}
    {}

    bool CheckTimes()
    {
        times_valid = ((start_ts > SysPoint::min()) && (stop_ts > SysPoint::min()) && (device_ts > SysPoint::min()));
        return times_valid;
    }
    // private:
    inline const static size_t counters_cnt = 16;
    SysPoint start_ts;
    SysPoint stop_ts;
    SysPoint device_ts;
    SysPoint app_ts;
    UInt_t cur_spill_time_ms;
    UInt_t spill_cnt;
    UInt_t phase;
    bool times_valid;
    SpillCntrs<counters_cnt> counters;
    //    ULong64_t cntr_after_err[counters_cnt];
    //    ULong64_t cntr_befor_err[counters_cnt];
    //    std::vector<std::pair<std::string, ULong64_t[counters_cnt]> > cntr_input;
    //    std::vector<std::pair<std::string, ULong64_t[counters_cnt]> > cntr_input_after_red;

    ClassDef(SpillStatus, 1)
};

#endif /* SPILLSTATUS_H */
