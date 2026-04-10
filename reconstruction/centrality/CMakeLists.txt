# Create a library called "libBmnRecoCentrality"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/base
 ${CMAKE_SOURCE_DIR}/base/data
 ${CMAKE_SOURCE_DIR}/database/uni_db
 ${CMAKE_SOURCE_DIR}/reconstruction/centrality
 ${CMAKE_SOURCE_DIR}/detectors/fhcal
 ${CMAKE_SOURCE_DIR}/detectors/hodo
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
BmnCentralityClass.cxx
BmnEventCentrality.cxx
CentralityClusterizer.cxx
)

Set(HEADERS)
Set(LINKDEF RecoCentralityLinkDef.h)
Set(LIBRARY_NAME BmnRecoCentrality)
Set(DEPENDENCIES FHCal Hodo UniDb Core TreePlayer)

GENERATE_LIBRARY()
