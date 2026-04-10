#include "BmnConverterTools.h"

// STL
#include <chrono>
#include <regex>
// ROOT
#include <TPRegexp.h>
// BmnRoot
#include "BmnFunctionSet.h"
#include "BmnMath.h"
#include "IndentedConfigParser.h"

// using std::string;
// using std::make_pair;
using namespace std::chrono_literals;
using namespace std::chrono;

BmnConverterTools::T0Config BmnConverterTools::GetT0Cfg(string str)
{
    if (str.rfind("Comment", 0) == 0)
        return T0Config::Comment;
    if (str.rfind("Discriminators", 0) == 0)
        return T0Config::Discriminators;
    if (str.rfind("Delay lines", 0) == 0)
        return T0Config::DelayLines;
    if (str.rfind("Switches", 0) == 0)
        return T0Config::Switches;
    if (str.rfind("LV", 0) == 0)
        return T0Config::LV;
    if (str.rfind("BD_multiplicity", 0) == 0)
        return T0Config::BDMult;
    if (str.rfind("Extra delays mask", 0) == 0)
        return T0Config::ExtraDelaysMask;
    return T0Config::Undefined;
}

SysPoint BmnConverterTools::ParseTAI(json& jt, Int_t tai_utc_dif)
{
    UInt_t nans = static_cast<UInt_t>(jt["ns"]);
    UInt_t sec = static_cast<UInt_t>(jt["sec"]) - tai_utc_dif;
    bool valid = static_cast<UInt_t>(jt["valid"]);
    return valid ? SysPoint{sec * 1s + nans * 1ns} : SysPoint::min();
}

BmnStatus BmnConverterTools::ParseRawT0TextConfig(string& str, BmnT0Raw<kT0_BIN_BLOCK_WORDS>* rt0dig)
{
    IndentedConfigParser::Node node{};
    regex rempty("\\s+\n*");
    istringstream iss(str);
    string line;
    bool main_found = false;
    bool cfg_found = false;
    while (std::getline(iss, line)) {
        if (regex_match(line, rempty)) {
            if (main_found) {
                cfg_found = true;
                break;
            }
        } else {
            smatch sm;
            regex rSpillTime("(\\d{4})(\\d{2})(\\d{2})(\\d{2})(\\d{2})(\\d{2})\\.BMNCfg\n*");
            if (regex_search(line, sm, rSpillTime)) {
                if (sm.size() > 6) {
                    int32_t year = std::stoi(sm[1]);
                    int32_t month = std::stoi(sm[2]);
                    int32_t day = std::stoi(sm[3]);
                    int32_t hour = std::stoi(sm[4]);
                    int32_t min = std::stoi(sm[5]);
                    int32_t sec = std::stoi(sm[6]);
                    TTimeStamp ts(year, month, day, hour, min, sec);
                    LOGF(debug, "ts cfg:  %s", ts.AsString());
                    //                    rt0dig->SetTS(ts);
                }
            }
            main_found = true;
        }
    }
    if (!cfg_found)
        return kBMNERROR;
    IndentedConfigParser::ParseIStringStream(iss, node);
    for (const auto& n : node.children) {
        auto& ch = n.children;
        switch (GetT0Cfg(n.value)) {
            case T0Config::Comment: {
                if (ch.size() > 0)
                    rt0dig->SetComment(ch[0].value);
                break;
            }
            case T0Config::Discriminators: {
                regex re("Discriminator(\\d+)\\r*");
                for (const auto& node_par : ch) {
                    string snum = regex_replace(node_par.value, re, "$1");
                    uint32_t idis = std::stoul(snum.data());
                    auto& dvec = rt0dig->Discriminators();
                    /* if (dvec.size() < (idis + 1))
                        dvec.resize(idis + 1); */
                    auto& disc = dvec[idis];
                    for (const auto& node_chan : node_par.children) {
                        if (node_chan.value.rfind("Polarity", 0) == 0) {
                            if (node_chan.children.size() > 0) {
                                const auto& str_field = node_chan.children[0].value;
                                if (str_field.rfind("Positive", 0) == 0)
                                    disc.polarity = 1;
                                else
                                    disc.polarity = -1;
                            }
                            continue;
                        }
                        regex rthr("Threshold,\\s+(\\w+)\\r*");
                        if (regex_match(node_chan.value, rthr)) {
                            string unit = regex_replace(node_chan.value, rthr, "$1");
                            if (node_chan.children.size() > 0) {
                                const auto& str_field = node_chan.children[0].value;
                                disc.threshold = std::atoi(str_field.data());
                                disc.threshold_unit = move(unit);
                            }
                            continue;
                        }
                    }
                    // disc.print();
                }
            } break;
            case T0Config::DelayLines: {
                regex re("Delay(\\d+)\\r*");
                for (const auto& node_par : ch) {
                    string snum = regex_replace(node_par.value, re, "$1");
                    uint32_t idis = std::stoul(snum.data());
                    auto& delvec = rt0dig->DelayLines();
                    /* if (delvec.size() < (idis + 1))
                        delvec.resize(idis + 1); */
                    if (node_par.children.size() > 0) {
                        const auto& str_field = node_par.children[0].value;
                        delvec[idis] = std::atoi(str_field.data());
                    }
                }
            } break;
            case T0Config::Switches: {
                regex re("Switch(\\d+)\\r*");
                for (const auto& node_par : ch) {
                    string snum = regex_replace(node_par.value, re, "$1");
                    uint32_t idis = std::stoul(snum.data());
                    auto& swvec = rt0dig->Switches();
                    /* if (swvec.size() < (idis + 1))
                        swvec.resize(idis + 1); */
                    if (node_par.children.size() > 0) {
                        const auto& str_field = node_par.children[0].value;
                        swvec[idis] = (str_field.rfind("On", 0) == 0) ? true : false;
                    }
                }
            } break;
            case T0Config::LV: {
                regex re("Channel(\\d+)\\r*");
                for (const auto& node_par : ch) {
                    string snum = regex_replace(node_par.value, re, "$1");
                    uint32_t idis = std::stoul(snum.data());
                    auto& dvec = rt0dig->LVChannelConfig();
                    /* if (dvec.size() < (idis + 1))
                        dvec.resize(idis + 1); */
                    auto& cfg = dvec[idis];
                    for (const auto& node_chan : node_par.children) {
                        if (node_chan.value.rfind("Active", 0) == 0) {
                            if (node_chan.children.size() > 0) {
                                const auto& str_field = node_chan.children[0].value;
                                cfg.active = (str_field.rfind("1", 0) == 0) ? true : false;
                            }
                            continue;
                        }
                        if (node_chan.value.rfind("Channel name", 0) == 0) {
                            if (node_chan.children.size() > 0)
                                cfg.name = node_chan.children[0].value;
                            continue;
                        }
                        if (node_chan.value.rfind("V_P1", 0) == 0) {
                            if (node_chan.children.size() > 0) {
                                const auto& str_field = node_chan.children[0].value;
                                cfg.V_P1 = std::atoi(str_field.data());
                            }
                            continue;
                        }
                        if (node_chan.value.rfind("V_P2", 0) == 0) {
                            if (node_chan.children.size() > 0) {
                                const auto& str_field = node_chan.children[0].value;
                                cfg.V_P1 = std::atoi(str_field.data());
                            }
                            continue;
                        }
                        if (node_chan.value.rfind("State", 0) == 0) {
                            if (node_chan.children.size() > 0) {
                                const auto& str_field = node_chan.children[0].value;
                                cfg.state = (str_field.rfind("On", 0) == 0) ? true : false;
                            }
                            continue;
                        }
                    }
                    // cfg.print();
                }
            } break;
            case T0Config::BDMult: {
                regex re("BDM_(\\w+)\\r*");
                for (const auto& node_par : ch) {
                    string spar = regex_replace(node_par.value, re, "$1");
                    if (node_par.children.size() > 0) {
                        const auto& str_field = node_par.children[0].value;
                        uint32_t thr = std::atoi(str_field.data());
                        if (spar == "H")
                            rt0dig->SetBDMultH(thr);
                        if (spar == "L")
                            rt0dig->SetBDMultL(thr);
                        //                        LOGF(info, "BDMH %u BDML %u ", rt0dig->GetBDMultH(),
                        //                        rt0dig->GetBDMultL());
                    }
                }
            } break;
            case T0Config::ExtraDelaysMask: {
                regex re("\\$([0-9a-fA-F]+)\\r*");
                for (const auto& node_par : ch) {
                    string snum = regex_replace(node_par.value, re, "$1");
                    uint32_t num = std::stoul(snum.data(), nullptr, 16);
                    //                    LOGF(info, " num %X", num);
                    rt0dig->SetExtraDelaysMask(num);
                }
            } break;
            default:
                break;
        }
    }
    return kBMNSUCCESS;
}

SpillStatus BmnConverterTools::ParseJsonStatus(json& jv, Int_t tai_utc_dif)
{
    SpillStatus ss{};
    auto& j_runtime = jv["runTime"];
    auto& j_spill = j_runtime["spill"];
    ss.phase = static_cast<UInt_t>(j_spill["phase"]);
    ss.cur_spill_time_ms = static_cast<UInt_t>(j_spill["curSpillTimeMs"]);
    ss.spill_cnt = static_cast<UInt_t>(j_spill["count"]);
    auto j_device_time = j_runtime["time"];
    ss.app_ts = SysClock::from_time_t(static_cast<UInt_t>(j_device_time["app"]));
    auto j_device_time_WR = j_device_time["WR"];
    ss.device_ts = ParseTAI(j_device_time_WR, tai_utc_dif);
    auto j_spill_start = j_spill["spillStartTaiWrStat"];
    ss.start_ts = ParseTAI(j_spill_start, tai_utc_dif);
    auto j_spill_stop = j_spill["spillStopTaiWrStat"];
    ss.stop_ts = ParseTAI(j_spill_stop, tai_utc_dif);
    ss.CheckTimes();
    auto& j_counters = j_runtime["counters"];
    if (j_counters.contains("input_afterReduc")) {
        {
            auto& j_cnt_input = j_counters["input"];
            UInt_t size_input = j_cnt_input.size();
            ss.counters.cntr_input.resize(size_input);
            size_t iInput = 0;
            for (auto& [key, val] : j_cnt_input.items()) {
                string name = static_cast<string>(key);
                ss.counters.cntr_input[iInput].first = move(name);
                size_t size = min(val.size(), SpillStatus::counters_cnt);
                for (size_t i = 0; i < size; i++)
                    ss.counters.cntr_input[iInput].second[i] = static_cast<UInt_t>(val[i]);
                iInput++;
            }
        }
        {
            auto& j_cnt_input = j_counters["input_afterReduc"];
            UInt_t size_input = j_cnt_input.size();
            ss.counters.cntr_input_after_red.resize(size_input);
            size_t iInput = 0;
            for (auto& [key, val] : j_cnt_input.items()) {
                string name = static_cast<string>(key);
                ss.counters.cntr_input_after_red[iInput].first = move(name);
                size_t size = min(val.size(), SpillStatus::counters_cnt);
                for (size_t i = 0; i < size; i++)
                    ss.counters.cntr_input_after_red[iInput].second[i] = static_cast<UInt_t>(val[i]);
                iInput++;
            }
        }
    } else {   // crutch for runs before #7081
        {
            auto& j_cnt_input = j_counters["input"];
            size_t size = min(j_cnt_input.size(), SpillStatus::counters_cnt);
            ss.counters.cntr_input.resize(1);
            ss.counters.cntr_input[0].first = "7: ~xOff & Run & Spill";
            for (size_t i = 0; i < size; i++)
                ss.counters.cntr_input[0].second[i] = static_cast<UInt_t>(j_cnt_input[i]);
        }
        {
            auto& j_cnt_input = j_counters["after_reduc"];
            size_t size = min(j_cnt_input.size(), SpillStatus::counters_cnt);
            ss.counters.cntr_input_after_red.resize(1);
            ss.counters.cntr_input_after_red[0].first = "7: ~xOff & Run & Spill";
            for (size_t i = 0; i < size; i++)
                ss.counters.cntr_input_after_red[0].second[i] = static_cast<UInt_t>(j_cnt_input[i]);
        }
    }
    {
        auto& j_cnt_ae = j_counters["after_err"];
        size_t size = min(j_cnt_ae.size(), SpillStatus::counters_cnt);
        for (size_t i = 0; i < size; i++)
            ss.counters.cntr_after_err[i] = static_cast<UInt_t>(j_cnt_ae[i]);
    }
    {
        auto& j_cnt_ae = j_counters["befor_err"];
        size_t size = min(j_cnt_ae.size(), SpillStatus::counters_cnt);
        for (size_t i = 0; i < size; i++)
            ss.counters.cntr_befor_err[i] = static_cast<UInt_t>(j_cnt_ae[i]);
    }
    LOG(debug) << BmnFunctionSet::TimePoint2String(ss.start_ts) << " start";
    LOG(debug) << BmnFunctionSet::TimePoint2String(ss.stop_ts) << "  stop";
    LOG(debug) << BmnFunctionSet::TimePoint2String(ss.device_ts) << " dev ts WR";
    LOG(debug) << BmnFunctionSet::TimePoint2String(ss.app_ts) << " app ts";
    return ss;
}

BmnStatus BmnConverterTools::ParseJsonConfig(json& j, BmnTrigConfig& trig_conf)
{
    // Phys trig config
    auto ph_trig_setups = j["programs"]["Trc#T0"]["knownSetups"];
    if (ph_trig_setups.size()) {
        auto ph_trig = ph_trig_setups.begin().value()["trc"]["phyTrig"];
        UInt_t match_win = ph_trig["matchWin"];
        UInt_t trig_delay = ph_trig["trigDelay"];
        UInt_t read_chan_mask = ph_trig["readChMask"];
        UInt_t start_chan_mask = ph_trig["startChMask"];
        UInt_t calib_cnt = ph_trig["calibTrigger"]["count"];
        UInt_t calib_en = (bool)ph_trig["calibTrigger"]["en"];
        trig_conf.SetMatchWindow(match_win * TRC_CLOCK_NS);
        trig_conf.SetTrigDelay(trig_delay * TRC_CLOCK_NS);
        trig_conf.SetCalibCount(calib_cnt);
        trig_conf.SetCalibEn(calib_en);
        trig_conf.SetStartChannelMask(start_chan_mask);
        trig_conf.SetReadChannelMask(read_chan_mask);
        LOGF(debug, "start_chan_mask %u", start_chan_mask);
        LOGF(debug, "read_chan_mask  %u", read_chan_mask);
        auto ch_conf = ph_trig["chCfg"];
        unordered_map<string, BmnTrigChannelConfig>& tcc = trig_conf.TrigChannelConfig();
        for (auto& trig_chan_conf : ch_conf) {
            LOG(debug2) << trig_chan_conf.dump();
            string name = trig_chan_conf["name"];
            if (name.length() == 0)
                continue;
            BmnTrigChannelConfig tcc_el;
            tcc_el.after_protect_en = (bool)trig_chan_conf["reg"]["after_protect_en"].get<int>();
            uint32_t ap_clocks = trig_chan_conf["reg"]["after_protect_time"];
            tcc_el.after_protect_time = ap_clocks * TRC_CLOCK_NS;
            tcc_el.before_protect_en = (bool)trig_chan_conf["reg"]["before_protect_en"].get<int>();
            uint32_t bp_clocks = trig_chan_conf["reg"]["before_protect_time"];
            tcc_el.before_protect_time = bp_clocks * TRC_CLOCK_NS;
            tcc_el.delay = trig_chan_conf["reg"]["delay"];
            tcc_el.reduction_en = (bool)trig_chan_conf["reg"]["reduction_en"].get<int>();
            tcc_el.reduction_factor = trig_chan_conf["reg"]["reduction_factor"];
            tcc.insert(make_pair(name, tcc_el));
        }
        return kBMNSUCCESS;
    } else
        return kBMNERROR;
}

BmnStatus BmnConverterTools::ParseComplexTLV(UInt_t* d, UInt_t& len, UInt_t& runId)
{
    uint16_t iWord = 0;
    while (iWord < len / kNBYTESINWORD) {
        UInt_t word = d[iWord++];
        UInt_t idLen = 0;
        switch (word) {
            case RECORD_RUN_NUMBER:
                idLen = d[iWord++];
                if (idLen != kNBYTESINWORD) {
                    LOGF(error, "Wrong RunId length %u", idLen);
                    return kBMNERROR;
                }
                runId = d[iWord];
                LOGF(info, "\tRun Id: %u", runId);
                break;
            case RECORD_EVENT_ORDER: {
                idLen = d[iWord++];
                if (idLen != kNBYTESINWORD) {
                    LOGF(error, "Wrong event order length %u", idLen);
                    return kBMNERROR;
                }
                UInt_t event_order = d[iWord];
                LOGF(info, "\tEvent Order: %u", event_order);
                break;
            }
            case RECORD_FILE_ID: {
                idLen = d[iWord++];
                if (idLen != kNBYTESINWORD) {
                    LOGF(error, "Wrong FileId length %u", idLen);
                    return kBMNERROR;
                }
                UInt_t file_id = d[iWord];
                LOGF(info, "\tFile Id: %u", file_id);
                break;
            }
            case RECORD_RUN_INDEX: {
                idLen = d[iWord++];
                if (idLen + iWord * kNBYTESINWORD > len) {
                    LOGF(error, "Wrong RunIndex length %u", idLen);
                    return kBMNERROR;
                }
                //                printf("idLen %u\n", idLen);
                TString runIndex(reinterpret_cast<const char*>(d + iWord), idLen);
                LOGF(info, "\tRun Index: %s", runIndex.Data());
                break;
            }
            default:
                LOGF(warning, "Unknown record %08X", word);
                break;
        }
        iWord += idLen * kNBYTESINWORD;
    }
    return kBMNSUCCESS;
}

TString BmnConverterTools::GetSubNameAfterRunId(TString name)
{
    TString str(name);
    TPRegexp re_data(".+_(\\d+)([^/]*)\\.data");
    TPRegexp re_raw(".+_(\\d+)([^/]*)_raw\\.root");
    //    TPRegexp re_data(".*\\w+_\\w+_\\w+_(\\d+)([^/]*)\\.data");
    //    TPRegexp re_raw(".*\\w+_run(\\d+)([^/]*)_raw\\.root");
    if (re_data.MatchB(str))
        re_data.Substitute(str, "$2");
    else {
        if (re_raw.MatchB(str))
            re_raw.Substitute(str, "$2");
        else
            return "";
    }
    return str;
}

BmnStatus BmnConverterTools::ParseRawFileName(TString s, BmnFileProp& prop)
{
    string ss(s.Data());
    return ParseRawFileName(ss, prop);
}
BmnStatus BmnConverterTools::ParseRawFileName(string path, BmnFileProp& prop)
{
    int32_t lastSlash = path.rfind('/');
    string name(path.substr(lastSlash + 1, path.length() - lastSlash));
    LOGF(debug, "name: %s", name.data());
    regex re6("(\\w+)_(\\w+)_(\\w+)_(\\d+)_ev(\\d+)_p(\\d+)\\.(\\w+)\\r*");
    regex re5("(\\w+)_(\\w+)_(\\w+)_(\\d+)_ev(\\d+)(_p\\d+){0,1}\\.(\\w+)\\r*");
    if (!regex_match(name, re5)) {
        LOGF(error, "regex doesn't match: %s", name.data());
        return kBMNERROR;
    }
    string str_exp = regex_replace(name, re5, "$1");
    string str_run = regex_replace(name, re5, "$2");
    string str_run_in = regex_replace(name, re5, "$3");
    string str_run_id = regex_replace(name, re5, "$4");
    string str_ev_ord = regex_replace(name, re5, "$5");
    uint32_t run_id = std::stoul(str_run_id.data());
    uint32_t ev_ord = std::stoul(str_ev_ord.data());

    prop.SetRunId(run_id);
    prop.SetRunIndex(str_run_in);
    prop.SetEventOrder(ev_ord);

    uint32_t part_id = 0;
    if (regex_match(name, re6)) {
        string str_part = regex_replace(name, re6, "$6");
        part_id = std::stoul(str_part.data());
    }
    prop.SetFileId(part_id);

    LOGF(debug, "exp %s", str_exp.data());
    LOGF(debug, "run %s", str_run.data());
    LOGF(debug, "run_in %s", prop.GetRunIndex().data());
    LOGF(debug, "run_id %d", prop.GetRunId());
    LOGF(debug, "ev_ord %d", prop.GetEventOrder());
    LOGF(debug, "part_id %d", prop.GetFileId());
    return kBMNSUCCESS;
}