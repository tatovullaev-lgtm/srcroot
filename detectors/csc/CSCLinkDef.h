// $Id: CSCLinkDef.h,v
#ifdef __CLING__

#pragma link C++ nestedclasses;
#pragma link C++ nestedtypedefs;

#pragma link C++ class BmnCSC + ;
#pragma link C++ class BmnCSCPoint + ;
#pragma link C++ class BmnCSCStationSet + ;
#pragma link C++ class BmnCSCStation + ;
#pragma link C++ class BmnCSCModule + ;
#pragma link C++ class BmnCSCLayer + ;
#pragma link C++ class BmnCSCDigit + ;
#pragma link C++ class BmnCSCDigitizer + ;
#pragma link C++ class BmnCSCHit + ;
#pragma link C++ class BmnCSCHitMaker + ;
#pragma link C++ class BmnCSCTransform + ;

// #pragma link C++ class BmnCSCDigit+;
// #pragma link C++ class BmnCSCHit+;

#pragma link C++ namespace BmnCSCConfiguration;
#pragma link C++ enum BmnCSCConfiguration::CSC_CONFIG;

#endif
