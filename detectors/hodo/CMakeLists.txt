# Create a library called "libHodo"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/base
 ${CMAKE_SOURCE_DIR}/base/data
 ${CMAKE_SOURCE_DIR}/detectors/hodo
 ${CMAKE_SOURCE_DIR}/detectors/common
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${ROOT_INCLUDE_DIR} 
)

include_directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
 ${ROOT_LIBRARY_DIR}
 ${FAIRROOT_LIBRARY_DIR}
 ${Boost_LIBRARY_DIRS}
)
 
link_directories(${LINK_DIRECTORIES})

# List of source files
set(SRCS
 BmnHodo.cxx
 BmnHodoPoint.cxx
 BmnHodoContFact.cxx
 BmnHodoGeoPar.cxx
 BmnHodoDigitizer.cxx
 BmnHodoStrip.cxx
 BmnHodoEvent.cxx
)

Set(HEADERS
 BmnHodo.h
 BmnHodoPoint.h
 BmnHodoDigit.h
 BmnHodoDigi.h
 BmnHodoAddress.h
 BmnHodoGeo.h
 BmnHodoGeoPar.h
 BmnHodoContFact.h
 BmnHodoDigitizer.h
 BmnHodoStrip.h
 BmnHodoEvent.h
 BmnHodoResolution.h
 #../common/BmnGeoTemplate.h
)


Set(LINKDEF HodoLinkDef.h)
Set(LIBRARY_NAME Hodo)
Set(DEPENDENCIES BmnBase GeoBase Base Core BmnGeoTemplate)

GENERATE_LIBRARY()
