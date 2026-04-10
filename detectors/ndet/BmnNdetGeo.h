// BmnNdetGeo.h
#ifndef BMNNDETGEO_H
#define BMNNDETGEO_H

#include "BmnGeoTemplate.h"
#include "BmnNdetAddress.h"
#include "BmnNdetGeoPar.h"

#include <regex>

static const std::regex kNdetRegex{BmnNdetGeoPar::fElementPattern};
static const std::regex kArmRegex{BmnNdetGeoPar::fArmPattern};

class BmnNdetGeo
    : public FairGeoSet
    , public BmnGeoTemplate<BmnNdetGeo>
{
  public:
    BmnNdetGeo()
        : FairGeoSet()
    {
        SetName("BmnNdetGeo");
    }

    enum class BoxFace
    {
        Unknown,
        Xminus,
        Xplus,
        Yminus,
        Yplus,
        Zminus,
        Zplus
    };
    enum class Direction
    {
        Unknown,
        FromInside,
        FromOutside
    };
    static std::pair<BoxFace, Direction> GetEnteredFace(const TVector3& globalPos, const TVector3& globalMom);

    static bool CheckIfSensitive(const std::string& vol) { return BmnNdetGeoPar::CheckIfSensitive(vol); }
    static const char* PathBeforeLocalSystem() { return BmnNdetGeoPar::fPathBeforeLocalSystem; }
    static const char* DetectorVolumeName() { return BmnNdetGeoPar::fDetectorVolumeName; }
    static const char* TopVolumeName() { return BmnNdetGeoPar::fTopVolumeName; }

    uint32_t GetAddressFromPath(const std::string& spath) const
    {
        LOG(debug4) << GetName() << ": address from path " << spath;
        std::smatch m;
        if (std::regex_search(spath, m, kNdetRegex) && m.size() == 5) {
            int det = std::stoi(m[1]);
            int row = std::stoi(m[2]);
            int col = std::stoi(m[3]);
            int layer = std::stoi(m[4]);
            return BmnNdetAddress::GetAddress(det, row, col, layer);
        }

        LOG(error) << GetName() << ": no address for " << spath;
        return 0;
    }

    uint8_t GetArmFromPath(const std::string& spath) const
    {
        LOG(debug4) << GetName() << ": address from path " << spath;
        std::smatch m;
        if (std::regex_search(spath, m, kArmRegex) && m.size() == 2) {
            return static_cast<uint32_t>(std::stoi(m[1].str()));
        }

        LOG(error) << GetName() << ": no address for " << spath;
        return 0;
    }

    ClassDef(BmnNdetGeo, 4)
};

#endif   // BMNNDETGEO_H
