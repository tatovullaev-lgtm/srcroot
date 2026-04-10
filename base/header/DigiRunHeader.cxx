#include "DigiRunHeader.h"

#include "FairLogger.h"

DigiRunHeader::DigiRunHeader()
    : fPeriodId(0)
    , fRunId(0)
    , fEventCnt(0)
{}

DigiRunHeader::DigiRunHeader(UInt_t period_id, UInt_t run_id, TTimeStamp run_start_time, TTimeStamp run_end_time)
    : fPeriodId(period_id)
    , fRunId(run_id)
    , fEventCnt(0)
    , fRunStartTime(run_start_time)
    , fRunEndTime(run_end_time)
{}

DigiRunHeader::~DigiRunHeader() {}

ULong64_t DigiRunHeader::GetScaler(string name)
{
    auto it = scalers_name2index_map.find(name);
    if (it == scalers_name2index_map.end()) {
        LOG(error) << "Scaler " << name << " not found\n";
        return 0;
    }
    return fScalersVec[it->second];
}

ULong64_t DigiRunHeader::GetRawT0Counter(string name, UInt_t ch)
{
    auto it = fT0Map.find(make_pair(name, ch));
    if (it == fT0Map.end()) {
        printf("T0 counter %s.%02u not found!", name.c_str(), ch);
        return 0;
    } else {
        return rawt0_cntrs[it->second];
    }
}

// ULong64_t DigiRunHeader::GetTrcCounter(string name)
//{
//     auto it = fTrigBitsMap.find(name);
//     if (it == fTrigBitsMap.end()) {
//         printf("TRC counter %s not found!", name.c_str());
//         return 0;
//     } else {
//         return counters[it->second];
//     }
// }
