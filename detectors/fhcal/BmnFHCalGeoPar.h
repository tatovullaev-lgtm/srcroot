/*************************************************************************************
 *
 *         Class BmnFHCalGeoPar
 *
 ************************************************************************************/

#ifndef BMNGEOFHCALPAR_H
#define BMNGEOFHCALPAR_H

#include "FairParGenericSet.h"
#include "TObjArray.h"
#include "regex"

class BmnFHCalGeoPar : public FairParGenericSet
{
  public:
    TObjArray* fGeoSensNodes; /** List of FairGeoNodes for sensitive  volumes */
    TObjArray* fGeoPassNodes; /** List of FairGeoNodes for passive  volumes */

    BmnFHCalGeoPar(const char* name = "BmnFHCalGeoPar",
                   const char* title = "FHCal Geometry Parameters",
                   const char* context = "FHCalDefaultContext");
    ~BmnFHCalGeoPar(void);
    void clear(void);
    void putParams(FairParamList*);
    Bool_t getParams(FairParamList*);
    TObjArray* GetGeoSensitiveNodes() { return fGeoSensNodes; }
    TObjArray* GetGeoPassiveNodes() { return fGeoPassNodes; }

    static bool CheckIfSensitive(std::string name);

    static inline const TString fTopVolumeName = "TOP";
    static inline const TString fCommonVolumeName = "FHCAL_common";
    static inline const TString fDetectorVolumeName = "fhcal01";
    static inline const TString fhcalMod_name_large = "fhcal01m";
    static inline const TString fhcalMod_name_small = "fhcal01m_NICA";
    static inline const TString Fe_name = "Fe";
    static inline const TString slot_Fe_name = "Fe_slot";
    static inline const TString Pb_name = "fhcal01a";
    static inline const TString slot_Pb_name = "Pb_slot";
    static inline const TString SensitiveVolume_name = "fhcal01s";
    static inline const TString slot_Sc_name = "Sc_slot";
    static inline const TString Tyvec_name = "Tyvec";
    static inline const TString slot_Tyvec_Y_name = "Tyvec_slot_Y";
    static inline const TString slot_Tyvec_A_name = "Tyvec_slot_A";
    static inline const TString Fe_name_NICA = "Fe_NICA";
    static inline const TString Pb_name_NICA = "fhcal01a_NICA";
    static inline const TString SensitiveVolume_name_NICA = "fhcal01s_NICA";
    static inline const TString Tyvec_name_NICA = "Tyvec_NICA";

    static inline const TString fPathBeforeLocalSystem = "/FHCAL_common_0/fhcal01_1/";
    // pattern to retrieve information about element indexes from its node path in geo file
    static inline const std::regex fElementPattern{R"(/fhcal01m(?:_NICA_|_)(\d+)/Tyvec(?:_NICA_|_)(\d+))"};
    static inline const TString NICA_pattern = "_NICA_";   // 7 sections in module

    ClassDef(BmnFHCalGeoPar, 1)
};

#endif
