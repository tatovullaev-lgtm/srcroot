#ifndef BMNFHCALGEO_H
#define BMNFHCALGEO_H

#include "BmnFHCalAddress.h"
#include "BmnFHCalGeoPar.h"
#include "BmnGeoTemplate.h"

#include <regex>

static const std::regex kFHCalRegex{BmnFHCalGeoPar::fElementPattern};

class BmnFHCalGeo
    : public FairGeoSet
    , public BmnGeoTemplate<BmnFHCalGeo>
{
  public:
    BmnFHCalGeo()
        : FairGeoSet()
    {
        SetName("BmnFHCalGeo");
    }

    static bool CheckIfSensitive(const std::string& vol) { return BmnFHCalGeoPar::CheckIfSensitive(vol); }
    static const char* PathBeforeLocalSystem() { return BmnFHCalGeoPar::fPathBeforeLocalSystem; }
    static const char* DetectorVolumeName() { return BmnFHCalGeoPar::fDetectorVolumeName; }
    static const char* TopVolumeName() { return BmnFHCalGeoPar::fTopVolumeName; }

    uint32_t GetAddressFromPath(const std::string& spath)
    {
        std::smatch m;
        if (std::regex_search(spath, m, kFHCalRegex) && m.size() == 3) {
            int mod = std::stoi(m[1]);
            int tyv = std::stoi(m[2]);
            int sec = ((tyv - 1) / 6) + 1;   // calculate section number
            int sc = ((tyv - 1) % 6) + 1;    // calculate scintillator number (in section)
            int type = (spath.find(BmnFHCalGeoPar::NICA_pattern) == std::string::npos ? 2 : 1);
            return BmnFHCalAddress::GetAddress(type, mod, sec, sc);
        }
        LOG(error) << GetName() << " : No address for path " << spath;
        return 0;
    }

    // --- override only the traversal/averaging logic ---
    void ListNodesImpl()
    {
        // getting position of 1st scintillator
        fPositionMap.clear();

        int N = gGeoManager->CountNodes();
        for (int i = 0; i < N; ++i) {
            gGeoManager->CdNode(i);
            auto* node = gGeoManager->GetCurrentNode();
            if (!node)
                continue;
            if (!BmnFHCalGeoPar::CheckIfSensitive(node->GetVolume()->GetName()))
                continue;

            uint32_t fullAddr = GetAddressFromPath(gGeoManager->GetPath());
            if (BmnFHCalAddress::GetScintillatorId(fullAddr) != 1)
                continue;

            uint32_t physAddr = BmnFHCalAddress::GetPhysicalAddress(fullAddr);
            double loc[3] = {0}, glb[3];
            gGeoManager->LocalToMaster(loc, glb);
            TVector3 pos(glb[0], glb[1], glb[2]);
            TVector3 err = GetPosError(node);   // Grab the default half–size error (from the TGeoBBox)
            err.SetZ(err.Z() * 2 * 5 * 6);      // Scale eZ. 2 sides. 1:4 scintillator:lead sampling. 6 layers;
            fPositionMap[physAddr] = std::make_pair(pos, err);
        }
    }

    int GetMaxModuleId() const
    {
        uint32_t mx = 0;
        for (auto& kv : fPositionMap) {
            mx = std::max(mx, BmnFHCalAddress::GetModuleId(kv.first));
        }
        return mx;
    }

    ClassDef(BmnFHCalGeo, 4)
};

#endif   // BMNFHCALGEO_H
