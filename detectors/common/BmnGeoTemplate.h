#ifndef BMNGEOTEMPLATE_H
#define BMNGEOTEMPLATE_H

#include "BmnEnums.h"
#include "FairGeoSet.h"
#include "FairLogger.h"
#include "TFile.h"
#include "TGeoBBox.h"
#include "TGeoManager.h"
#include "TGeoNavigator.h"
#include "TGeoNode.h"
#include "TGeoVolume.h"
#include "TString.h"
#include "TVector3.h"

#include <memory>
#include <string>
#include <unordered_map>

// -----------------------------------------------------------------------------
// CRTP base: loads geometry from either an existing TGeoManager or a ROOT file,
// then delegates to Derived::ListNodesImpl() to fill fPositionMap.
// -----------------------------------------------------------------------------
template<class Derived>
class BmnGeoTemplate
{
  public:
    BmnGeoTemplate() = default;
    virtual ~BmnGeoTemplate() = default;

    // Load via existing gGeoManager
    BmnStatus ReadGeometryFromGeoManager(bool getGlobalPosition = true)
    {
        if (!gGeoManager) {
            LOG(error) << Name() << ": no GeoManager!";
            return kBMNERROR;
        }
        gGeoManager->GetCache()->BuildIdArray();
        auto* saveTop = gGeoManager->GetTopVolume();
        if (!getGlobalPosition) {
            gGeoManager->CdTop();
            if (gGeoManager->CheckPath(Derived::PathBeforeLocalSystem())) {
                gGeoManager->cd(Derived::PathBeforeLocalSystem());
                gGeoManager->SetTopVolume(gGeoManager->GetCurrentNode()->GetVolume());
            } else {
                LOG(error) << Name() << ": wrong path " << Derived::PathBeforeLocalSystem();
            }
        }

        ListNodes();

        if (!getGlobalPosition) {
            gGeoManager->SetTopVolume(saveTop);
        }
        return kBMNSUCCESS;
    }

    // Load standalone from ROOT file (with VMCWORKDIR fallback)
    BmnStatus ReadGeometryFromFile(const std::string& path, bool getGlobalPosition = true)
    {
        TString full = Form("%s/geometry/%s", getenv("VMCWORKDIR"), path.c_str());
        LOG(info) << Name() << ": ReadGeometryFromFile " << full;
        if (gGeoManager) {
            LOG(warn) << Name() << ": GeoManager exists. Delegating to GeoManager";
            return ReadGeometryFromGeoManager(getGlobalPosition);
        } else {
            TFile f(full.Data(), "READONLY");
            auto* top = dynamic_cast<TGeoVolume*>(f.Get(Derived::TopVolumeName()));
            if (!top) {
                LOG(error) << Name() << ": no TOP volume in file " << full;
                return kBMNERROR;
            }
            gGeoManager->SetTopVolume(top);
            gGeoManager->CloseGeometry();
            if (ReadGeometryFromGeoManager(getGlobalPosition) == kBMNSUCCESS) {
                delete gGeoManager;
                gGeoManager = nullptr;
            }
        }
        return kBMNSUCCESS;
    }

    // Public entry‐point: clear maps & dispatch to impl, using gGeoManager
    void ListNodes()
    {
        fPositionMap.clear();
        if (!gGeoManager) {
            LOG(error) << Name() << ": ListNodes called without a GeoManager";
            return;
        }
        // call detector‐specific implementation
        static_cast<Derived*>(this)->ListNodesImpl();

        if (FairLogger::GetLogger()->IsLogNeeded(fair::Severity::debug)) {
            for (const auto& kv : fPositionMap) {
                uint32_t address = kv.first;
                const auto& pos = kv.second.first;
                const auto& err = kv.second.second;
                LOG(debug) << Form("%s: addr=0x%08X  pos=(%.2f, %.2f, %.2f)  err=(%.2f, %.2f, %.2f)", Name(), address,
                                   pos.X(), pos.Y(), pos.Z(), err.X(), err.Y(), err.Z());
            }
        }
    }

    // IsPointInside just delegates to Derived’s volume name
    bool IsPointInside(const TVector3& p) const
    {
        if (!gGeoManager)
            return false;
        auto* nav = gGeoManager->GetCurrentNavigator();
        if (!nav)
            return false;
        nav->SetCurrentPoint(p.X(), p.Y(), p.Z());
        nav->FindNode();
        return TString(nav->GetPath()).Contains(Derived::DetectorVolumeName());
    }

    // Accessors
    const auto& GetPositionMap() const { return fPositionMap; }

  protected:
    /// Helper to call the FairGeoSet::GetName() on the actual object
    const char* Name() const { return static_cast<Derived*>(const_cast<BmnGeoTemplate*>(this))->GetName(); }

    // --- default implementation: one node to one entry ---
    void ListNodesImpl()
    {
        int n = gGeoManager->CountNodes();
        for (int i = 0; i < n; ++i) {
            gGeoManager->CdNode(i);
            auto* node = gGeoManager->GetCurrentNode();
            if (!node)
                continue;
            if (!Derived::CheckIfSensitive(node->GetVolume()->GetName()))
                continue;

            // address from path
            uint32_t addr = static_cast<Derived*>(this)->GetAddressFromPath(gGeoManager->GetPath());

            // position
            double loc[3] = {0}, glb[3];
            gGeoManager->LocalToMaster(loc, glb);
            fPositionMap[addr] =
                std::make_pair(TVector3(glb[0], glb[1], glb[2]), static_cast<Derived*>(this)->GetPosError(node));
        }
    }

    /// by default: box‐half‐sizes
    TVector3 GetPosError(TGeoNode* node) const
    {
        if (auto* box = dynamic_cast<TGeoBBox*>(node->GetVolume()->GetShape())) {
            return TVector3(box->GetDX(), box->GetDY(), box->GetDZ());
        }
        return TVector3(0, 0, 0);
    }

    std::map<uint32_t, std::pair<TVector3, TVector3>> fPositionMap;   //! Transient. address to position and error

    ClassDef(BmnGeoTemplate, 1)
};

#endif   // BMNGEOTEMPLATE_H
