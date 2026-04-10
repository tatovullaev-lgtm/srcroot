# Create a library called "libGeoDb"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_SOURCE_DIR}/field
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${ROOT_INCLUDE_DIR}
)

include_directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
 ${ROOT_LIBRARY_DIR}
 ${XML2_LIBRARY_DIR}
 ${FAIRROOT_LIBRARY_DIR}
)
 
link_directories(${LINK_DIRECTORIES})

# List of source files
set(SRCS
 GeoSetup.cxx
)

Set(HEADERS)
Set(LINKDEF GeoSetup.h)
Set(LIBRARY_NAME GeoDb)
Set(DEPENDENCIES Core Geom Gpad Net RSQLite BmnField XMLParser XMLIO)

GENERATE_LIBRARY()

# Download database files if not exists
IF ((DEFINED ENV{DBL_FILE_PATH}) AND (NOT EXISTS "$ENV{DBL_FILE_PATH}"))
 message(STATUS "Downloading Geometry Database...")
 execute_process(COMMAND root -b -l -q ${CMAKE_CURRENT_SOURCE_DIR}/macros/installLocalDB.C OUTPUT_VARIABLE OUTPUT_TEXT)
 string(FIND ${OUTPUT_TEXT} "(int) 0" FIND_POS)
 IF (NOT ${FIND_POS} EQUAL -1)
  message(STATUS "Downloading Geometry Database... successfull")
 ELSE (NOT ${FIND_POS} EQUAL -1)
  message(STATUS "Downloading Geometry Database... failed")
 ENDIF (NOT ${FIND_POS} EQUAL -1)
ENDIF ((DEFINED ENV{DBL_FILE_PATH}) AND (NOT EXISTS "$ENV{DBL_FILE_PATH}"))

# make install settings
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${CMAKE_INSTALL_PREFIX}/database/
        FILES_MATCHING PATTERN "*.*" PATTERN "CMakeLists.txt" EXCLUDE PATTERN "*.h" EXCLUDE PATTERN "*.cxx" EXCLUDE)
