#ifndef BMNT0RAW_H
#define BMNT0RAW_H

#include <array>
#include <chrono>
#include <string>
#include <vector>
// ROOT
#include <TObject.h>
// BmnRoot
#include "BmnFunctionSet.h"

using SysClock = std::chrono::system_clock;
using SysPoint = std::chrono::time_point<SysClock>;
using Dur_ns = std::chrono::duration<Double_t, std::nano>;
using std::map;
using std::string;
using std::vector;

struct LV_Channel_Config
{
    LV_Channel_Config()
        : active{}
        , name{}
        , V_P1{}
        , V_P2{}
        , state{}
    {}
    virtual ~LV_Channel_Config() = default;
    bool active;
    string name;
    int32_t V_P1;
    int32_t V_P2;
    bool state;
    void print()
    {
        printf("active: %d\n", active);
        printf("  name: %s\n", name.data());
        printf("  V_P1: %d\n", V_P1);
        printf("  V_P2: %d\n", V_P2);
        printf(" state: %d\n", state);
    }
    ClassDef(LV_Channel_Config, 1);
};

struct Discriminator_Config
{
    Discriminator_Config()
        : polarity{}
        , threshold{}
        , threshold_unit{}
    {}
    virtual ~Discriminator_Config() = default;
    int8_t polarity;
    int32_t threshold;
    string threshold_unit;
    void print()
    {
        printf(" polarity: %2d\n", polarity);
        printf("threshold: %5d %s\n", threshold, threshold_unit.data());
    }
    ClassDef(Discriminator_Config, 1);
};

/**
 * T0 module spill settings and counts
 */
template<UInt_t counters_cnt = 256>
class BmnT0Raw : public TObject
{
  public:
    /** Constructor **/
    BmnT0Raw(UInt_t* src = nullptr)
        : counters{}
        , since_last_event_ns{0}
        , bd_mult_h{0}
        , bd_mult_l{0}
        , extra_delays_mask{0}
        , trigger_mask{0}
    //    { memcpy(counters, src, sizeof(UInt_t) * counters_cnt); }
    {
        if (src)
            memcpy(counters.data(), src, sizeof(UInt_t) * counters_cnt);
    }
    /** Destructor **/
    virtual ~BmnT0Raw() = default;

    constexpr static const UInt_t CountersCount() { return counters_cnt; }

    void SetTime(SysPoint point) { timestamp = BmnFunctionSet::TimePoint2TS(point); }

    SysPoint GetTime() { return BmnFunctionSet::TimeStamp2TP(timestamp); }

    void SetTS(TTimeStamp ts) { timestamp = ts; }

    TTimeStamp GetTS() { return timestamp; }

    //    void SetTimeSinceLastEv(Dur_ns dur) { since_last_event = dur;}
    /**
     * Write time since the last written event.
     * "last" - actually last in the raw data, not necessarily last in the spill
     * because the stat events are written asynchronously
     * @param dur - time (ns) since the last event
     */
    void SetTimeSinceLastEvNs(Double_t dur) { since_last_event_ns = dur; }

    Dur_ns GetTimeSinceLastEv()
    {
        //        return since_last_event;
        return Dur_ns(since_last_event_ns);
    }

    Double_t GetTimeSinceLastEvNs() { return since_last_event_ns; }

    //    UInt_t (&Counters())[counters_cnt] { return counters; }
    std::array<UInt_t, counters_cnt>& Counters() { return counters; }

    //    vector<UInt_t>& Counters() {
    //        return counters;
    //    }

    map<uint32_t, Discriminator_Config>& Discriminators() { return discriminator_cfg; }

    map<uint32_t, LV_Channel_Config>& LVChannelConfig() { return lv_channel_cfg; }
    map<uint32_t, int32_t>& DelayLines() { return delay_lines; }
    map<uint32_t, bool>& Switches() { return switches; }

    void SetComment(string str) { comment = str; }

    string GetComment() { return comment; }

    void SetBDMultH(uint32_t val) { bd_mult_h = val; }

    uint32_t GetBDMultH() { return bd_mult_h; }

    void SetBDMultL(uint32_t val) { bd_mult_l = val; }

    uint32_t GetBDMultL() { return bd_mult_l; }

    void SetExtraDelaysMask(uint32_t val) { extra_delays_mask = val; }

    uint32_t GetExtraDelaysMask() { return extra_delays_mask; }

    void SetTriggerMask(uint32_t val) { trigger_mask = val; }

    uint32_t GetTriggerMask() { return trigger_mask; }

  protected:
    //    static const UInt_t counters_cnt = 256;
    //    UInt_t counters[counters_cnt];
    std::array<UInt_t, counters_cnt> counters;
    //    vector<UInt_t> counters;
    //    SysPoint time;
    TTimeStamp timestamp;
    //    Dur_ns since_last_event;
    Double_t since_last_event_ns;
    string comment;
    uint32_t bd_mult_h;
    uint32_t bd_mult_l;
    uint32_t extra_delays_mask;
    uint32_t trigger_mask;

    map<uint32_t, Discriminator_Config> discriminator_cfg;
    map<uint32_t, LV_Channel_Config> lv_channel_cfg;
    map<uint32_t, int32_t> delay_lines;
    map<uint32_t, bool> switches;

    ClassDef(BmnT0Raw, 8);
};

#endif /* BMNT0RAW_H */
