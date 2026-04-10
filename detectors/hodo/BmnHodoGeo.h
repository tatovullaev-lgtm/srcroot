#ifndef BMNHODOGEO_H
#define BMNHODOGEO_H

#include "BmnGeoTemplate.h"
#include "BmnHodoAddress.h"
#include "BmnHodoGeoPar.h"

#include <regex>

static const std::regex kHodoRegex{BmnHodoGeoPar::fElementPattern};

class BmnHodoGeo
    : public FairGeoSet
    , public BmnGeoTemplate<BmnHodoGeo>
{
  public:
    BmnHodoGeo()
        : FairGeoSet()
    {
        SetName("BmnHodoGeo");
    }

    static bool CheckIfSensitive(const std::string& vol) { return BmnHodoGeoPar::CheckIfSensitive(vol); }
    static const char* PathBeforeLocalSystem() { return BmnHodoGeoPar::fPathBeforeLocalSystem; }
    static const char* DetectorVolumeName() { return BmnHodoGeoPar::fDetectorVolumeName; }
    static const char* TopVolumeName() { return BmnHodoGeoPar::fTopVolumeName; }

    uint32_t GetAddressFromPath(const std::string& spath) const
    {
        std::smatch m;
        if (std::regex_search(spath, m, kHodoRegex) && m.size() == 2) {
            return BmnHodoAddress::GetAddress(std::stoi(m[1]), 0, 0);
        }
        LOG(error) << GetName() << ": no address for " << spath;
        return 0;
    }

    int GetMaxStripId() const
    {
        uint32_t mx = 0;
        for (auto& kv : fPositionMap) {
            mx = std::max(mx, BmnHodoAddress::GetStripId(kv.first));
        }
        return mx;
    }

    ClassDef(BmnHodoGeo, 1)
};

#endif   // BMNHODOGEO_H
