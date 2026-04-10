# Create a library called "libBmnField"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/field
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${ROOT_INCLUDE_DIR}
)

include_directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
 ${ROOT_LIBRARY_DIR}
 ${FAIRROOT_LIBRARY_DIR}
) #${FairLogger_LIBDIR}
 
link_directories(${LINK_DIRECTORIES})

# List of source files
set(SRCS
 BmnNewFieldMap.cxx
 BmnFieldConst.cxx
 BmnFieldContFact.cxx
 BmnFieldMap.cxx
 BmnFieldMapData.cxx
 BmnFieldMapSym3.cxx
 BmnFieldPar.cxx
 BmnFieldCreator.cxx
 BmnSP41FieldMapCreator.cxx
 BmnRegion.cxx
 BmnConstPar.cxx
 BmnMapPar.cxx
 BmnMultiFieldPar.cxx
 BmnMultiField.cxx
 BmnFieldParser.cxx
 BmnFieldPoint.cxx
)

Set(HEADERS)
Set(LINKDEF FieldLinkDef.h)
Set(LIBRARY_NAME BmnField)
Set(DEPENDENCIES ParBase Base FairLogger::FairLogger Physics Gpad Hist RIO Core)

If ((${FairRoot_VERSION_MAJOR} VERSION_GREATER_EQUAL "18") AND (${FairRoot_VERSION_MINOR} VERSION_GREATER_EQUAL "8") AND (${FairRoot_VERSION_PATCH} VERSION_GREATER_EQUAL "0"))
 add_definitions(-DFairRoot_18_8_0)
EndIf()

GENERATE_LIBRARY()
