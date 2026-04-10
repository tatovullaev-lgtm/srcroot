// $Id: UniDbLinkDef.h,v
#ifdef __CLING__

#pragma link C++ nestedclasses;
#pragma link C++ nestedtypedefs;

#pragma link C++ class UniRunPeriod + ;
#pragma link C++ class UniRun + ;
#pragma link C++ class UniRawFile + ;
#pragma link C++ class UniRunGeometry + ;

#pragma link C++ class UniSimulationFile + ;

#pragma link C++ class UniDetector + ;
#pragma link C++ class UniParameterType + ;
#pragma link C++ class UniParameter + ;
#pragma link C++ class UniDetectorParameter + ;

#pragma link C++ class UniConnection + ;
#pragma link C++ class UniSearchCondition + ;
// #pragma link C++ struct structParseSchema++;
// #pragma link C++ class vector<structParseSchema>+;
// #pragma link C++ struct structParseValue++;
// #pragma link C++ class vector<structParseValue*>+;

// the database enums
#pragma link C++ enum enumValueType;
#pragma link C++ enum ConnectionType;
// #pragma link C++ enum enumParameterType;

// the database structures to use directly in macros
#pragma link C++ struct UniqueRunNumber++;
#pragma link C++ struct UniValue++;
#pragma link C++ struct BoolValue++;
#pragma link C++ struct IntValue++;
#pragma link C++ struct UIntValue++;
#pragma link C++ struct DoubleValue++;
#pragma link C++ struct StringValue++;
#pragma link C++ struct BinaryValue++;
#pragma link C++ struct IIValue++;
#pragma link C++ struct DchMapValue++;
#pragma link C++ struct GemMapValue++;
#pragma link C++ struct GemPedestalValue++;
#pragma link C++ struct TriggerMapValue++;
#pragma link C++ struct LorentzShiftValue++;
#pragma link C++ struct MapBoolValue++;
#pragma link C++ struct MapIntValue++;
#pragma link C++ struct MapDVectorValue++;
#pragma link C++ struct AlignmentValue++;
#pragma link C++ struct TdcInlValue++;
#pragma link C++ struct JsonValue++;

#endif
