#ifndef BMNGEOHODOPAR_H
#define BMNGEOHODOPAR_H

#include "FairParGenericSet.h"
#include "TObjArray.h"
#include "regex"

class BmnHodoGeoPar : public FairParGenericSet
{
  public:
    TObjArray* fGeoSensNodes; /** List of FairGeoNodes for sensitive  volumes */
    TObjArray* fGeoPassNodes; /** List of FairGeoNodes for passive  volumes */

    BmnHodoGeoPar(const char* name = "BmnHodoGeoPar",
                  const char* title = "Hodo Geometry Parameters",
                  const char* context = "HodoDefaultContext");
    ~BmnHodoGeoPar(void);
    void clear(void);
    void putParams(FairParamList*);
    Bool_t getParams(FairParamList*);
    TObjArray* GetGeoSensitiveNodes() { return fGeoSensNodes; }
    TObjArray* GetGeoPassiveNodes() { return fGeoPassNodes; }

    static bool CheckIfSensitive(std::string name);

    static inline const TString fTopVolumeName = "TOP";
    static inline const TString fCommonVolumeName = "Hodo_common";
    static inline const TString fDetectorVolumeName = "hodo01";
    static inline const TString fDetectorVolumeNameBox = "hodo01_box";
    static inline const TString hodoStick_name = "hodo01stick";
    static inline const TString hodoSensitiveStick_name = "hodo_stick_sens";

    static inline const TString fPathBeforeLocalSystem = "/Hodo_common_0/hodo01_box_1/hodo01_1/";
    // pattern to retrieve information about element indexes from its node path in geo file
    static inline const std::regex fElementPattern{R"(/hodo01stick_(\d+))"};

    ClassDef(BmnHodoGeoPar, 1)
};

#endif
