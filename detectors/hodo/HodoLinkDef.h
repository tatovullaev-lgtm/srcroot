// $Id: HodoLinkDef.h,v
#ifdef __CLING__

#pragma link C++ nestedclasses;
#pragma link C++ nestedtypedefs;

#pragma link C++ class BmnHodo + ;
#pragma link C++ class BmnHodoPoint + ;
#pragma link C++ class BmnHodoAddress + ;
#pragma link C++ class BmnHodoDigi + ;
#pragma link C++ class BmnHodoGeo + ;
#pragma link C++ class BmnHodoGeoPar + ;
#pragma link C++ class BmnHodoContFact + ;
#pragma link C++ class BmnHodoDigit + ;
#pragma link C++ class BmnHodoDigitizer + ;
#pragma link C++ class BmnHodoStrip + ;
#pragma link C++ class BmnHodoEvent + ;

#pragma link C++ class BmnGeoTemplate < BmnHodoGeo> + ;

#pragma link C++ class std::vector < BmnHodoDigi> + ;   // delete me

#endif
