// $Id: SiBTLinkDef.h,v
#ifdef __CLING__

#pragma link C++ nestedclasses;
#pragma link C++ nestedtypedefs;

#pragma link C++ class BmnSiBT + ;
#pragma link C++ class BmnSiBTPoint + ;
#pragma link C++ class BmnSiBTStationSet + ;
#pragma link C++ class BmnSiBTStation + ;
#pragma link C++ class BmnSiBTModule + ;
#pragma link C++ class BmnSiBTLayer + ;
#pragma link C++ class BmnSiBTDigit + ;
#pragma link C++ class BmnSiBTDigitizer + ;
#pragma link C++ class BmnSiBTHit + ;
#pragma link C++ class BmnSiBTHitMaker + ;
#pragma link C++ class BmnSiBTTransform + ;

#pragma link C++ namespace BmnSiBTConfiguration;
#pragma link C++ enum BmnSiBTConfiguration::SiBT_CONFIG;

#endif
