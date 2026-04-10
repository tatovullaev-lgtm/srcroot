# Create a library called "libSiProf" (Silicon Beam Tracker)

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/base
 ${CMAKE_SOURCE_DIR}/base/data
 ${CMAKE_SOURCE_DIR}/base/raw
 ${CMAKE_SOURCE_DIR}/base/header
 ${CMAKE_SOURCE_DIR}/detectors/SiProf
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
 BmnSiProf.cxx
 BmnSiProfPoint.cxx
 BmnSiProfStationSet.cxx
 BmnSiProfStation.cxx
 BmnSiProfModule.cxx
 BmnSiProfLayer.cxx
 BmnSiProfDigit.cxx
 BmnSiProfDigitizer.cxx
 BmnSiProfHit.cxx
 BmnSiProfHitMaker.cxx
 BmnSiProfTransform.cxx

 BmnSiProfConfiguration.h
)

Set(HEADERS)
Set(LINKDEF SiProfLinkDef.h)
Set(LIBRARY_NAME SiProf)
Set(DEPENDENCIES BmnBase GeoBase Base GenVector Core XMLParser Gdml)

GENERATE_LIBRARY()
