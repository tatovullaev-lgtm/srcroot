// $Id: FHCalLinkDef.h,v
#ifdef __CLING__

#pragma link C++ nestedclasses;
#pragma link C++ nestedtypedefs;

#pragma link C++ class BmnFHCal + ;
#pragma link C++ class BmnFHCalPoint + ;
#pragma link C++ class BmnFHCalAddress + ;
#pragma link C++ class BmnFHCalGeo + ;
#pragma link C++ class BmnFHCalGeoPar + ;
#pragma link C++ class BmnFHCalContFact + ;
#pragma link C++ class BmnFHCalDigitizer + ;
#pragma link C++ class BmnFHCalDigi + ;
#pragma link C++ class BmnFHCalDigit + ;
#pragma link C++ class BmnFHCalModule + ;
#pragma link C++ class BmnFHCalEvent + ;

#pragma link C++ class std::vector < BmnFHCalModule> + ;
#pragma link C++ class BmnGeoTemplate < BmnFHCalGeo> + ;

#endif
