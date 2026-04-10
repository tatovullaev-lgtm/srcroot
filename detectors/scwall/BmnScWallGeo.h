#ifndef BMNSCWALLGEO_H
#define BMNSCWALLGEO_H

#include "BmnGeoTemplate.h"
#include "BmnScWallAddress.h"
#include "BmnScWallGeoPar.h"

#include <regex>

static const std::regex kScWallRegex{BmnScWallGeoPar::fElementPattern};

class BmnScWallGeo
    : public FairGeoSet
    , public BmnGeoTemplate<BmnScWallGeo>
{
  public:
    BmnScWallGeo()
        : FairGeoSet()
    {
        SetName("BmnScWallGeo");
    }

    static bool CheckIfSensitive(const std::string& vol) { return BmnScWallGeoPar::CheckIfSensitive(vol); }
    static const char* PathBeforeLocalSystem() { return BmnScWallGeoPar::fPathBeforeLocalSystem; }
    static const char* DetectorVolumeName() { return BmnScWallGeoPar::fDetectorVolumeName; }
    static const char* TopVolumeName() { return BmnScWallGeoPar::fTopVolumeName; }

    uint32_t GetAddressFromPath(const std::string& spath) const
    {
        std::smatch m;
        if (std::regex_search(spath, m, kScWallRegex) && m.size() == 2) {
            return BmnScWallAddress::GetAddress(std::stoi(m[1]));
        }
        LOG(error) << GetName() << ": no address for " << spath;
        return 0;
    }

    int GetMaxCellId() const
    {
        uint32_t mx = 0;
        for (auto& kv : fPositionMap) {
            mx = std::max(mx, BmnScWallAddress::GetCellId(kv.first));
        }
        return mx;
    }

    ClassDef(BmnScWallGeo, 2)
};

#endif   // BMNSCWALLGEO_H
