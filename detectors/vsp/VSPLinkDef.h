// $Id: VSPLinkDef.h,v
#ifdef __CLING__

#pragma link C++ nestedclasses;
#pragma link C++ nestedtypedefs;

#pragma link C++ class BmnVSP + ;
#pragma link C++ class BmnVSPPoint + ;
#pragma link C++ class BmnVSPLayer + ;
#pragma link C++ class BmnVSPModule + ;
#pragma link C++ class BmnVSPStation + ;
#pragma link C++ class BmnVSPStationSet + ;
#pragma link C++ class BmnVSPDigitizer + ;
#pragma link C++ class BmnVSPHit + ;
#pragma link C++ class BmnVSPHitMaker + ;
#pragma link C++ class BmnVSPDigit + ;
#pragma link C++ class BmnVSPTrack + ;
#pragma link C++ class BmnVSPHitProducer + ;
#pragma link C++ class BmnVSPTransform + ;

#pragma link C++ namespace BmnVSPConfiguration;
#pragma link C++ enum BmnVSPConfiguration::VSP_CONFIG;

#endif
