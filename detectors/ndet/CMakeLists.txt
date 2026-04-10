# Create a library called "libNdet"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/base
 ${CMAKE_SOURCE_DIR}/base/data
 ${CMAKE_SOURCE_DIR}/detectors/ndet
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
BmnNdetAddress.h
BmnNdetAddressMixin.h
BmnNdetPoint.h  
BmnNdetSurfacePoint.h
BmnNdetDigi.h
BmnNdetDigit.h
BmnHgndDigi.h
BmnNdet.cxx 
BmnNdetContFact.cxx  
BmnNdetGeo.cxx
BmnNdetGeoPar.cxx
BmnNdetDigitizer.cxx
BmnNdetEvent.cxx
BmnNdetCell.cxx
BmnNdetCluster.cxx
BmnNdetClusterFinder.cxx
)


Set(HEADERS)
Set(LINKDEF NdetLinkDef.h)
Set(LIBRARY_NAME Ndet)
Set(DEPENDENCIES BmnBase GeoBase Base Core BmnGeoTemplate)

GENERATE_LIBRARY()
