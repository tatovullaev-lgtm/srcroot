// $Id: NdetLinkDef.h,v
#ifdef __CLING__

#pragma link C++ nestedclasses;
#pragma link C++ nestedtypedefs;

#pragma link C++ class BmnNdet + ;
#pragma link C++ class BmnNdetPoint + ;
#pragma link C++ class BmnNdetSurfacePoint + ;
#pragma link C++ class BmnNdetContFact + ;
#pragma link C++ class BmnNdetAddress + ;
#pragma link C++ class BmnNdetDigi + ;
#pragma link C++ class BmnHgndDigi + ;
#pragma link C++ class BmnNdetGeo + ;
#pragma link C++ class BmnNdetGeoPar + ;
#pragma link C++ class BmnNdetDigit + ;
#pragma link C++ class BmnNdetDigitizer + ;
#pragma link C++ class BmnNdetEvent + ;
#pragma link C++ class BmnNdetCell + ;
#pragma link C++ class BmnNdetCluster + ;
#pragma link C++ class BmnNdetClusterFinder + ;

#pragma link C++ class BmnNdetAddressMixin < BmnNdetPoint> + ;
#pragma link C++ class BmnNdetAddressMixin < BmnNdetDigit> + ;
#pragma link C++ class BmnNdetAddressMixin < BmnNdetCell> + ;

#pragma link C++ class BmnGeoTemplate < BmnNdetGeo> + ;

#endif
