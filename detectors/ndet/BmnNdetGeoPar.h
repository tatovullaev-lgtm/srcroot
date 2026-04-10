/*************************************************************************************
 *
 *         Class BmnNdetGeoPar
 *
 *  Adopted for BMN by:   Elena Litvinenko
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  06-11-2015
 *  Modified by M.Golubeva July 2022
 *
 ************************************************************************************/

#ifndef BMNGEONDETPAR_H
#define BMNGEONDETPAR_H

#include "FairParGenericSet.h"
#include "TObjArray.h"
#include "regex"

class BmnNdetGeoPar : public FairParGenericSet
{
  public:
    TObjArray* fGeoSensNodes; /** List of FairGeoNodes for sensitive  volumes */
    TObjArray* fGeoPassNodes; /** List of FairGeoNodes for passive  volumes */

    BmnNdetGeoPar(const char* name = "BmnNdetGeoPar",
                  const char* title = "Ndet Geometry Parameters",
                  const char* context = "NdetDefaultContext");
    ~BmnNdetGeoPar(void);
    void clear(void);
    void putParams(FairParamList*);
    Bool_t getParams(FairParamList*);
    TObjArray* GetGeoSensitiveNodes() { return fGeoSensNodes; }
    TObjArray* GetGeoPassiveNodes() { return fGeoPassNodes; }

    static bool CheckIfSensitive(std::string name);

    static inline const TString fTopVolumeName = "TOP";
    static inline const TString fCommonVolumeName = "NDET_common";
    static inline const TString fEnvelopeVolumeName = "ndet01";
    static inline const TString fDetectorVolumeName = "interior";
    static inline const TString fModuleVolumeName = "ndet01m_NICA";
    static inline const TString Pb_name_NICA_5mm = "ndet01a_NICA_5mm";
    static inline const TString Pb_name_NICA_10mm = "ndet01a_NICA_10mm";
    static inline const TString Air_name_NICA = "ndet01air_NICA";
    static inline const TString Air_name_NICA_2mm = "ndet01air_NICA_2mm";
    static inline const TString G10_name_NICA = "ndet01g10_NICA";
    static inline const TString PLA_name_NICA = "ndet01pla_NICA";
    static inline const TString SensitiveVolume_name_NICA = "ndet01s_NICA";
    static inline const TString SensitiveVolume_name_VETO = "ndet01s_VETO";

    static inline const TString Cu_name_NICA = "ndet01a_NICA";    // Cu
    static inline const TString Al_name_NICA = "ndet01Al_NICA";   // Al
    static inline const TString Pb_name_NICA = "ndet01Pb_NICA";   // Pb

    static inline const TString Al_edge_vert = "ndet01Al_edge_vert";
    static inline const TString Al_edge_hor_front = "ndet01Al_edge_hor_front";
    static inline const TString Al_edge_hor_side = "ndet01Al_edge_hor_side";

    static inline const TString fPathBeforeLocalSystem = "/NDET_common_0/";
    // pattern to retrieve information about element indexes from its node path in geo file
    static inline const std::regex fElementPattern{R"(.*\/ndet01_(\d+)(?:\/[^/]*)*\/ndet01m_NICA_(\d+)_(\d+)_(\d+))"};
    static inline const std::regex fArmPattern{R"(.*\/ndet01_(\d+))"};

    ClassDef(BmnNdetGeoPar, 4)
};

#endif /* !BMNGEONDETPAR_H */
