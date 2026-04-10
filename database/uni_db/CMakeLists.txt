# Create a library called "libUniDb"

# check for zlib found to switch to Adler32 checksum
if(ZLIB_FOUND)
    add_compile_definitions(HAVE_ZLIB=1)
endif()

set(INCLUDE_DIRECTORIES
 ${CMAKE_CURRENT_SOURCE_DIR}
)

set(SYSTEM_INCLUDE_DIRECTORIES
 ${Boost_INCLUDE_DIRS}
 ${ROOT_INCLUDE_DIR}
 ${XML2_INCLUDE_DIR}
 ${JSON_INCLUDE_DIR}
)

include_directories(${INCLUDE_DIRECTORIES})
include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
 ${ROOT_LIBRARY_DIR}
 ${XML2_LIBRARY_DIR}
 ${Boost_LIBRARY_DIRS}
)
 
link_directories(${LINK_DIRECTORIES})

# List of source files
set(SRCS
 UniRunPeriod.cxx
 UniRun.cxx
 UniRawFile.cxx
 UniRunGeometry.cxx
 
 UniSimulationFile.cxx
 
 UniDetector.cxx
 UniParameterType.cxx
 UniParameter.cxx
 UniDetectorParameter.cxx
 
 UniConnection.cxx
 UniSearchCondition.cxx 

 uni_db_settings.h
 
 UniValue.cxx
 # add definitions of the possible database structures and enums
 uni_db_structures.h
)

set(HEADERS)
set(LINKDEF UniDbLinkDef.h)
set(LIBRARY_NAME UniDb)
set(DEPENDENCIES MathCore Core Geom Gpad Net xml2)
if(ZLIB_FOUND)
    set(DEPENDENCIES ZLIB::ZLIB ${DEPENDENCIES})
endif()

GENERATE_LIBRARY()

add_executable (show_simulation_files show_simulation_files.cxx)
target_link_libraries(show_simulation_files ${ROOT_LIBRARIES})

add_executable (show_experiment_files show_experiment_files.cxx)
target_link_libraries(show_experiment_files ${ROOT_LIBRARIES})

INSTALL(PROGRAMS ${EXECUTABLE_OUTPUT_PATH}/show_simulation_files DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/)
INSTALL(PROGRAMS ${EXECUTABLE_OUTPUT_PATH}/show_experiment_files DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/)
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/docs/ DESTINATION ${CMAKE_INSTALL_PREFIX}/database/uni_db/docs)
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/examples/ DESTINATION ${CMAKE_INSTALL_PREFIX}/database/uni_db/examples)
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/macros/ DESTINATION ${CMAKE_INSTALL_PREFIX}/database/uni_db/macros)
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scheme/ DESTINATION ${CMAKE_INSTALL_PREFIX}/database/uni_db/scheme)
