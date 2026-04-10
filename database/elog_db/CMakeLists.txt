# Create a library called "libElogDb"

set(INCLUDE_DIRECTORIES
 ${CMAKE_CURRENT_SOURCE_DIR}
)

set(SYSTEM_INCLUDE_DIRECTORIES
 ${ROOT_INCLUDE_DIR}
 ${Boost_INCLUDE_DIRS}
)

include_directories(${INCLUDE_DIRECTORIES})
include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
 ${ROOT_LIBRARY_DIR}
 ${Boost_LIBRARY_DIRS}
)
 
link_directories(${LINK_DIRECTORIES})

# List of source files
set(SRCS
 ElogPerson.cxx
 ElogType.cxx
 ElogTrigger.cxx
 ElogBeam.cxx
 ElogTarget.cxx
 ElogRecord.cxx
 ElogAttachment.cxx 
 
 ElogConnection.cxx
 ElogSearchCondition.cxx

 elog_db_settings.h
)

set(HEADERS)
set(LINKDEF ElogDbLinkDef.h)
set(LIBRARY_NAME ElogDb)
set(DEPENDENCIES Net MathCore Core)

GENERATE_LIBRARY()

INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/examples/ DESTINATION ${CMAKE_INSTALL_PREFIX}/database/elog_db/examples)
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/macros/ DESTINATION ${CMAKE_INSTALL_PREFIX}/database/elog_db/macros)
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scheme/ DESTINATION ${CMAKE_INSTALL_PREFIX}/database/elog_db/scheme)
