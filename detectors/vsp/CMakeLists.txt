# Create a library called "libVSP"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/base
 ${CMAKE_SOURCE_DIR}/base/data
 ${CMAKE_SOURCE_DIR}/base/cbm
 ${CMAKE_SOURCE_DIR}/database/uni_db
 ${CMAKE_SOURCE_DIR}/detectors/vsp
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${ROOT_INCLUDE_DIR}
 ${JSON_INCLUDE_DIR}
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
 BmnVSP.cxx
 BmnVSPPoint.cxx
 BmnVSPLayer.cxx
 BmnVSPModule.cxx
 BmnVSPStation.cxx
 BmnVSPStationSet.cxx
 BmnVSPDigitizer.cxx
 BmnVSPHitMaker.cxx
 BmnVSPDigit.cxx
 BmnVSPTrack.cxx
 BmnVSPHit.cxx
 BmnVSPHitProducer.cxx
 BmnVSPTransform.cxx
 BmnVSPConfiguration.h
)

Set(HEADERS)
Set(LINKDEF VSPLinkDef.h)
Set(LIBRARY_NAME VSP)
Set(DEPENDENCIES BmnBase UniDb GeoBase Base GenVector Core XMLParser Gdml)

GENERATE_LIBRARY()
