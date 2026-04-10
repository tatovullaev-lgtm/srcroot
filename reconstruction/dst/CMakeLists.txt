# Create a library called "libBmnDst"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/base
 ${CMAKE_SOURCE_DIR}/base/cbm
 ${CMAKE_SOURCE_DIR}/base/data
 ${CMAKE_SOURCE_DIR}/base/raw
 ${CMAKE_SOURCE_DIR}/base/header
 ${CMAKE_SOURCE_DIR}/field
 ${CMAKE_SOURCE_DIR}/database/uni_db
 ${CMAKE_SOURCE_DIR}/detectors/FD
 ${CMAKE_SOURCE_DIR}/services
 ${CMAKE_SOURCE_DIR}/reconstruction/dst
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
 #ExtractZ2.h
 BmnFillDstTask.cxx
 BmnFillTrigInfoDst.cxx
 BmnTrigInfoDst.cxx
)

Set(HEADERS)
Set(LINKDEF DstLinkDef.h)
Set(LIBRARY_NAME BmnDst)
Set(DEPENDENCIES BmnField BmnBase UniDb Base Core)

GENERATE_LIBRARY()
