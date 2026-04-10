// $Id: SiProfLinkDef.h,v
#ifdef __CLING__

#pragma link C++ nestedclasses;
#pragma link C++ nestedtypedefs;

#pragma link C++ class BmnSiProf + ;
#pragma link C++ class BmnSiProfPoint + ;
#pragma link C++ class BmnSiProfStationSet + ;
#pragma link C++ class BmnSiProfStation + ;
#pragma link C++ class BmnSiProfModule + ;
#pragma link C++ class BmnSiProfLayer + ;
#pragma link C++ class BmnSiProfDigit + ;
#pragma link C++ class BmnSiProfDigitizer + ;
#pragma link C++ class BmnSiProfHit + ;
#pragma link C++ class BmnSiProfHitMaker + ;
#pragma link C++ class BmnSiProfTransform + ;

#pragma link C++ namespace BmnSiProfConfiguration;
#pragma link C++ enum BmnSiProfConfiguration::SiProf_CONFIG;

#endif
