#ifndef TRIGCONFIG_H
#define TRIGCONFIG_H

#include "BmnTrigChannelConfig.h"
#include "TNamed.h"

#include <string>
#include <unordered_map>

using std::string;
using std::unordered_map;

/**
 * Class BmnTrigConfig
 * Trigger DAQ config
 */
class BmnTrigConfig : public TNamed
{
  public:
    BmnTrigConfig();

    unordered_map<string, BmnTrigChannelConfig>& TrigChannelConfig() { return trig_channel_config; }

    uint32_t GetMatchWindow() { return match_window; }

    void SetMatchWindow(uint32_t v) { match_window = v; }

    uint32_t GetTrigDelay() { return trig_delay; }

    void SetTrigDelay(uint32_t v) { trig_delay = v; }

    uint32_t GetReadChannelMask() { return read_channel_mask; }

    void SetReadChannelMask(uint32_t v) { read_channel_mask = v; }

    uint32_t GetStartChannelMask() { return start_channel_mask; }

    void SetStartChannelMask(uint32_t v) { start_channel_mask = v; }

    uint32_t GetCalibCount() { return calib_count; }

    void SetCalibCount(uint32_t v) { calib_count = v; }

    bool GetCalibEn() { return calib_en; }

    void SetCalibEn(bool v) { calib_en = v; }

  private:
    unordered_map<string, BmnTrigChannelConfig> trig_channel_config;
    uint32_t match_window;   // TRC match window [ns]
    uint32_t trig_delay;     // TRC trig delay [ns]
    uint32_t read_channel_mask;
    uint32_t start_channel_mask;
    uint32_t calib_count;
    bool calib_en;
    // enum ChMask {
    //         ChMaskCalib = 1 << 16,
    //         ChMaskRandom = 1 << 17,
    //         ChMaskTimer = 1 << 18,
    //         ChMaskExt = 1 << 19,
    //     };
    ClassDef(BmnTrigConfig, 4)
};

#endif /* TRIGCONFIG_H */
