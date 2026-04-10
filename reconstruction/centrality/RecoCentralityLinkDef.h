// $Id: RecoCentralityLinkDef.h,v
#ifdef __CLING__

#pragma link C++ nestedclasses;
#pragma link C++ nestedtypedefs;

#pragma link C++ class BmnCentralityClass + ;
#pragma link C++ enum BmnCentralityClass::Method;
#pragma link C++ class BmnEventCentrality + ;
#pragma link C++ struct BmnEventCentrality::Comparator + ;
#pragma link C++ class CentralityClusterizer + ;

#pragma link C++ class std::set < BmnCentralityClass> + ;

#endif
