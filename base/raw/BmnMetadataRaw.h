#ifndef BMNMETADATARAW_H
#define BMNMETADATARAW_H

#include <map>
#include <string>
#include <unordered_map>
#include <vector>
// ROOT
#include "TObject.h"
// BmnRoot
#include "BmnAliases.h"
#include "BmnSpillInfo.h"
#include "SpillStatus.h"

using std::map;
using std::pair;
using std::string;
using std::unordered_map;
using std::vector;

typedef map<SysPoint, uint8_t> TimeDist;
typedef map<pair<uint32_t, uint8_t>, TimeDist> ScalerChan2TimeDist;
/**
 * Class BmnMetadataRaw
 * for the configuration and raw spill information
 */
class BmnMetadataRaw : public TObject
{
  public:
    BmnMetadataRaw();
    virtual ~BmnMetadataRaw();

    /** TRC JSON blocks (at least 2 for each spill)*/
    vector<SpillStatus>& SpillStatusVec() { return vec_spill_status; }
    /** TRC counters (sum over all spills)*/
    SpillCntrs<SpillStatus::counters_cnt>& SpillCounters() { return counters; }

    //    MapString2Cnt& Scalers() { return scalers; }

    /** Map (Spill start time -> BmnSpillInfo)*/
    map<SysPoint, BmnSpillInfo>& SpillMap() { return spill_map; }

    T0ChMap& RawT0Map() { return fT0Map; }
    NameChMap& TrcMap() { return fTrigBitsMap; }
    Name2SerCh& ScalersMap() { return fScalersMap; }
    ScalerChan2TimeDist& Scalers() { return scalers; }
    TimeDist& ExtConditions() { return ext_cond; }

  private:
    vector<SpillStatus> vec_spill_status;
    SpillCntrs<SpillStatus::counters_cnt> counters;

    map<SysPoint, BmnSpillInfo> spill_map;

    ScalerChan2TimeDist scalers;
    TimeDist ext_cond;

    T0ChMap fT0Map;
    NameChMap fTrigBitsMap;
    Name2SerCh fScalersMap;

    ClassDef(BmnMetadataRaw, 5)
};

#endif /* BMNMETADATARAW_H */
