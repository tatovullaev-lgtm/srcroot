/*************************************************************************************
 *
 *         Class BmnScWallGeoPar
 *
 ************************************************************************************/

#ifndef BMNGEOSCWALLPAR_H
#define BMNGEOSCWALLPAR_H

#include "FairParGenericSet.h"
#include "TObjArray.h"
#include "regex"

class BmnScWallGeoPar : public FairParGenericSet
{
  public:
    TObjArray* fGeoSensNodes; /** List of FairGeoNodes for sensitive  volumes */
    TObjArray* fGeoPassNodes; /** List of FairGeoNodes for passive  volumes */

    BmnScWallGeoPar(const char* name = "BmnScWallGeoPar",
                    const char* title = "ScWall Geometry Parameters",
                    const char* context = "ScWallDefaultContext");
    ~BmnScWallGeoPar(void);
    void clear(void);
    void putParams(FairParamList*);
    Bool_t getParams(FairParamList*);
    TObjArray* GetGeoSensitiveNodes() { return fGeoSensNodes; }
    TObjArray* GetGeoPassiveNodes() { return fGeoPassNodes; }

    static inline const int fTotalCells = 216;   // inclusive
    static inline const int fSmallCells = 40;    // inclusive

    static bool CheckIfSensitive(std::string name);

    static inline const TString fTopVolumeName = "TOP";
    static inline const TString fCommonVolumeName = "ScWAll_common";
    static inline const TString fDetectorVolumeName = "scwall01";
    static inline const TString scwallCell_name_large = "scwall01large";

    static inline const TString scwallCell_name_small = "scwall01small";
    static inline const TString scwallCell_name_small_10mm = "scwall01small_10mm";
    static inline const TString scwallCell_name_small_20mm = "scwall01small_20mm";

    static inline const TString scwall_name_box = "scwall01_box";   // Scint+box
    static inline const TString scwall_name_Al_plate_left = "scwall01_Al_plate_left";
    static inline const TString scwall_name_Al_plate_right = "scwall01_Al_plate_right";
    static inline const TString scwall_name_Al_plate_top = "scwall01_Al_plate_top";
    static inline const TString scwall_name_Air_plate = "scwall01_Air_plate";

    static inline const TString scwallSensitiveCell_name_small_cutted = "scwall_sens_small_cutted";
    static inline const TString scwallSensitiveCell_name_small_cutted_10mm = "scwall_sens_small_cutted_10mm";
    static inline const TString scwallSensitiveCell_name_small_cutted_20mm = "scwall_sens_small_cutted_20mm";

    static inline const TString scwallSensitiveCell_name_small_trap = "scwall_sens_small_trap";
    static inline const TString scwallSensitiveCell_name_small_trap_10mm = "scwall_sens_small_trap_10mm";
    static inline const TString scwallSensitiveCell_name_small_trap_20mm = "scwall_sens_small_trap_20mm";

    static inline const TString scwallSensitiveCell_name_large_cutted = "scwall_sens_large_cutted";
    static inline const TString scwallSensitiveCell_name_large_trap = "scwall_sens_large_trap";

    static inline const TString fPathBeforeLocalSystem = "/ScWAll_common_0/scwall01_box_1/scwall01_1/";
    // pattern to retrieve information about element indexes from its node path in geo file
    static inline const std::regex fElementPattern{R"(/scwall01(?:small|large)_(\d+))"};

    ClassDef(BmnScWallGeoPar, 2)
};

#endif
