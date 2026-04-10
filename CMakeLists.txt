# The name of our project is "BMNROOT".  CMakeLists files in this project can
# refer to the root source directory of the project as ${BMNROOT_SOURCE_DIR}
# or as ${CMAKE_SOURCE_DIR} and to the root binary directory of the project as
# ${BMNROOT_BINARY_DIR} or ${CMAKE_BINARY_DIR}.

# Check if cmake has the required version
CMAKE_MINIMUM_REQUIRED(VERSION 3.10.0 FATAL_ERROR)
foreach(p
  CMP0025 # Compiler id for Apple Clang is now AppleClang
  CMP0028 # Double colon in target name means ALIAS or IMPORTED target
  CMP0042 # MACOSX_RPATH is enabled by default
  CMP0054 # Only interpret ``if()`` arguments as variables or keywords when unquoted
  CMP0167 # The FindBoost module is removed since CMake 3.30
  )
  if(POLICY ${p})
  cmake_policy(SET ${p} NEW)
  endif()
endforeach()
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.12)
  cmake_policy(SET CMP0074 NEW) # allow to have _ROOT suffix in variables containing program installation roots
endif()
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.27)
  cmake_policy(SET CMP0144 NEW) # search also uppercase variables (terminated by _ROOT) in addition to original case
endif()

# Set name of our project to "BMNROOT"
project(BMNROOT)

# Set the build type: None, Debug, Release, RelWithDebInfo, MinSizeRel, OMP
# OMP - the quickest multithreading BmnRoot version for a multicore machine
If(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Debug)
EndIf(NOT CMAKE_BUILD_TYPE)

if(APPLE)
  if(NOT DEFINED CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER clang)
  endif()
  if(NOT DEFINED CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER clang++)
  endif()
endif()

Option(USE_PATH_INFO "Information from PATH and LD_LIBRARY_PATH are used." ON)
Option(ALIBUILD "Flag if we are building with AliBuild." OFF)
# Possibility to run BmnRoot on cluster nodes with different OS (stability cannot be guaranteed)
Option(IGNORE_DIFFERENT_SYSTEM "Allow running compiled BmnRoot on different OS" ON)

#Check if necessary environment variables are set
#If not stop execution
IF (NOT ALIBUILD)
 IF(NOT DEFINED ENV{FAIRROOTPATH})
  MESSAGE(FATAL_ERROR "You did not define the environment variable FAIRROOTPATH which is needed to find FairRoot. Please set this variable and execute cmake again.")
 ENDIF(NOT DEFINED ENV{FAIRROOTPATH})
 IF(NOT DEFINED ENV{SIMPATH})
  MESSAGE(FATAL_ERROR "You did not define the environment variable SIMPATH which is nedded to find the external packages. Please set this variable and execute cmake again.")
 ENDIF(NOT DEFINED ENV{SIMPATH})
 SET(SIMPATH $ENV{SIMPATH})
 SET(FAIRROOTPATH $ENV{FAIRROOTPATH})
 SET(ZEROMQ_ROOT $ENV{SIMPATH})
ELSE (NOT ALIBUILD)
 SET(FAIRROOTPATH $ENV{FAIRROOT_ROOT})
 SET(ZEROMQ_ROOT $ENV{SIMPATH})
 SET(GEANT3_PATH $ENV{GEANT3_ROOT})
 SET(BOOST_ROOT $ENV{BOOST_ROOT})
 SET(GSL_DIR $ENV{GSL_ROOT})
ENDIF (NOT ALIBUILD)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")
list(APPEND CMAKE_MODULE_PATH "${SIMPATH}/share/fairmq/cmake")
list(APPEND CMAKE_MODULE_PATH "${FAIRROOTPATH}/share/fairbase/cmake/private")
list(APPEND CMAKE_MODULE_PATH "${FAIRROOTPATH}/share/fairbase/cmake/modules")

# Generate compile_commands.json file (https://clang.llvm.org/docs/JSONCompilationDatabase.html)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Define CMAKE_INSTALL_*DIR family of variables
include(GNUInstallDirs)
include(FairRootTargetRootDictionary)

# Define install dirs
set(PROJECT_INSTALL_BINDIR ${CMAKE_INSTALL_BINDIR})
set(PROJECT_INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR})
set(PROJECT_INSTALL_DATADIR ${CMAKE_INSTALL_DATADIR}/bmnroot) # TODO: change 'bmnroot' to ${PROJECT_NAME_LOWER}

# https://cmake.org/Wiki/CMake_RPATH_handling
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/${PROJECT_INSTALL_LIBDIR}" isSystemDir)
if("${isSystemDir}" STREQUAL "-1")
  if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CMAKE_INSTALL_RPATH "$ORIGIN/../${PROJECT_INSTALL_LIBDIR}")
  elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(CMAKE_INSTALL_RPATH "@loader_path/../${PROJECT_INSTALL_LIBDIR}")
  else()
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${PROJECT_INSTALL_LIBDIR}")
  endif()
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas")
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} "-Wl,--enable-new-dtags")
  set(CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS} "-Wl,--enable-new-dtags")
  set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -z defs")
  set(CMAKE_MODULE_LINKER_FLAGS  "${CMAKE_MODULE_LINKER_FLAGS} -z defs")
  set(CMAKE_SHARED_LINKER_FLAGS  "${CMAKE_SHARED_LINKER_FLAGS} -z defs")
endif()

if(CMAKE_GENERATOR STREQUAL "Ninja" AND
  ((CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9) OR
   (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.5)))
  # Force colored warnings in Ninja's output, if the compiler has -fdiagnostics-color support.
  # Rationale in https://github.com/ninja-build/ninja/issues/814
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always")
endif()

# Set a default if not specified by the user
if(NOT DEFINED BUILD_SHARED_LIBS)
  set(BUILD_SHARED_LIBS ON)
endif()
if(NOT DEFINED CMAKE_INSTALL_MESSAGE)
  set(CMAKE_INSTALL_MESSAGE LAZY)
endif()

Set(VMCWORKDIR ${CMAKE_SOURCE_DIR})

Set(CheckSrcDir "${FAIRROOTPATH}/share/fairbase/cmake/checks")
include(CheckSymbolExists)

# Load some basic macros which are needed later on
include(BmnMacros)
include(FairMacros)
include(WriteConfigFile)
include(CheckCompiler)

#Check the compiler and set the compile and link flags
Check_Compiler()

set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
set(INCLUDE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/include")

find_package2(PUBLIC FairRoot REQUIRED)
# Extract the FairRoot version from fairroot-config
# The version info is of the form Major.Minor.Patch and stored in FairRoot_VERSION and FairSoft_VERSION variables
FairSoftVersion()
FairRootVersion()
Message(STATUS "############################")
Message(STATUS "FairSoft Version - ${FairSoft_VERSION}")
Message(STATUS "FairRoot Version - ${FairRoot_VERSION}")

set(BMNROOT TRUE)
add_definitions(-DBMNROOT)
Generate_BmnRoot_Version()
Message(STATUS "############################")

# Set the BmnRoot libraries version
SET(BMNROOT_VERSION "${BMNROOT_MAJOR_VERSION}.${BMNROOT_MINOR_VERSION}.${BMNROOT_PATCH_VERSION}")
#SET(FAIRROOT_LIBRARY_PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES}
SET(PROJECT_LIBRARY_PROPERTIES ${PROJECT_LIBRARY_PROPERTIES}
    VERSION "${BMNROOT_VERSION}"
#    SOVERSION "${BMNROOT_MAJOR_VERSION}"
#    SUFFIX ".so"
)

If(USE_PATH_INFO)
  Set(PATH "$PATH")
  If (APPLE)
    Set(LD_LIBRARY_PATH $ENV{DYLD_LIBRARY_PATH})
  Else (APPLE)
    Set(LD_LIBRARY_PATH $ENV{LD_LIBRARY_PATH})
  EndIf (APPLE)
Else(USE_PATH_INFO)
  STRING(REGEX MATCHALL "[^:]+" PATH $ENV{PATH})
EndIf(USE_PATH_INFO)

# Check if the user wants to build the project in the source
# directory and if the install directory is different from the build directory
CHECK_OUT_OF_SOURCE_BUILD()
CHECK_INSTALL_DIRECTORY()

# Check if we are on an UNIX system. If not stop with an error message
IF(NOT UNIX)
  MESSAGE(FATAL_ERROR "You're not on an UNIX system. The project was up to now only tested on UNIX systems, so we break here. If you want to go on please edit the CMakeLists.txt in the source directory.")
ENDIF(NOT UNIX)

if(SIMPATH)
  set(CMAKE_PREFIX_PATH ${SIMPATH} ${CMAKE_PREFIX_PATH})
  set(LD_LIBRARY_PATH ${LD_LIBRARY_PATH} ${SIMPATH}/lib)
endif()

# searches for needed packages
# REQUIRED means that cmake will stop if this packages are not found
# For example the framework can run without GEANT4, but ROOT is mandatory
find_package2(PUBLIC ROOT VERSION 6.10.00 REQUIRED)
if(ROOT_vmc_FOUND)
  set(VMCLIB ROOT::VMC)
else()
  find_package2(PUBLIC VMC REQUIRED)
  set(VMCLIB VMCLibrary)
endif()

#find_package(FairLogger REQUIRED)
find_package2(PUBLIC FairLogger VERSION 1.2.0 REQUIRED)
foreach(dep IN LISTS FairLogger_PACKAGE_DEPENDENCIES)
if(NOT dep STREQUAL "Boost")
  find_package2(PUBLIC ${dep} REQUIRED VERSION ${FairLogger_${dep}_VERSION})
  set(PROJECT_${dep}_VERSION ${FairLogger_${dep}_VERSION})
  if(dep STREQUAL "fmt") # handling of external fmt installation
    get_target_property(FMT_INCLUDE_DIR fmt::fmt INTERFACE_INCLUDE_DIRECTORIES)
    set(FairLogger_INCDIR ${FairLogger_INCDIR} ${FMT_INCLUDE_DIR})
  endif()
endif()
endforeach()

#find_package(Pythia6)
find_package2(PUBLIC Pythia6)
#find_package(Pythia8 REQUIRED)
find_package2(PUBLIC Pythia8 REQUIRED)

#find_package(Geant3 REQUIRED)
find_package2(PUBLIC Geant3)
#find_package(GEANT4 REQUIRED)
#find_package(GEANT4DATA REQUIRED)
#find_package(GEANT4VMC REQUIRED)
find_package2(PUBLIC Geant4 REQUIRED)
find_package2(PUBLIC Geant4VMC REQUIRED)

#find_package(GSL)
find_package2(PUBLIC GSL)
if(NOT DEFINED GTEST_ROOT)
  set(GTEST_ROOT ${SIMPATH})
endif()

find_package2(PUBLIC FairMQ VERSION 1.4.0 REQUIRED)
#find_package2(PUBLIC DDS)

#find_package(CLHEP)
find_package2(PRIVATE SSE REQUIRED)
#find_package(PLUTO)
#find_package(GENERATORS REQUIRED)
find_package2(PUBLIC XML2 REQUIRED)
find_package2(PUBLIC ZeroMQ REQUIRED)
find_package2(PUBLIC CURL REQUIRED)
find_package2(PUBLIC ZLIB)
find_package2(PUBLIC nlohmann_json VERSION 3.10.5 QUIET)
if(NOT nlohmann_json_FOUND)
  set(JSON_INCLUDE_DIR ${VMCWORKDIR}/services)
endif()

IF (NOT ALIBUILD)
  set(BOOST_ROOT ${SIMPATH})
  set(GSL_DIR ${SIMPATH})
  
  if(NOT DEFINED Boost_NO_SYSTEM_PATHS)
    Set(Boost_NO_SYSTEM_PATHS TRUE)
  endif()
  if(NOT DEFINED Boost_NO_BOOST_CMAKE AND CMAKE_VERSION VERSION_LESS 3.15)
    Set(Boost_NO_BOOST_CMAKE TRUE)
  endif()
  if(Boost_NO_BOOST_CMAKE)
    # If an older version of boost is found both of the variables below are
    # cached and in a second cmake run, a good boost version is found even
    # if the version is to old.
    # To overcome this problem both variables are cleared before checking for boost.
    Unset(Boost_INCLUDE_DIR CACHE)
    Unset(Boost_LIBRARY_DIRS CACHE)
  endif()
ENDIF (NOT ALIBUILD)
set(boost_dependencies)
list(APPEND boost_dependencies filesystem serialization program_options container regex thread)
if(BUILD_UNITTESTS)
  list(APPEND boost_dependencies unit_test_framework)
endif()

#find_package(Boost 1.67 REQUIRED)
find_package2(PUBLIC Boost
  VERSION 1.67
  COMPONENTS ${boost_dependencies}
  ADD_REQUIREMENTS_OF FairMQ FairLogger
  REQUIRED
)
If (Boost_FOUND)
  Set(Boost_Avail 1)
Else (Boost_FOUND)
  Set(Boost_Avail 0)
EndIf (Boost_FOUND)
# Needed for Boost from 1.73.0 to prevent warnings
#add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)

# set a variable which should be used in all CMakeLists.txt
# Defines all basic include directories from fairbase
SetBasicVariables()

if(SIMPATH)
  set(ROOT_INCLUDE_DIR ${ROOT_INCLUDE_DIR} ${VMC_INCLUDE_DIRS})
  set(BASE_INCLUDE_DIRECTORIES ${BASE_INCLUDE_DIRECTORIES} ${FairLogger_INCDIR})
endif()

# Recurse into the given subdirectories. This does not actually cause another cmake executable to run
# The same process will walk through the project's entire directory structure
add_subdirectory (database)
add_subdirectory (field)
add_subdirectory (passive)
add_subdirectory (generators)
add_subdirectory (base)
add_subdirectory (detectors)
add_subdirectory (steering)
add_subdirectory (miscellaneous)
add_subdirectory (alignment)
add_subdirectory (decoder)
add_subdirectory (reconstruction/dst)
add_subdirectory (reconstruction/KF)
add_subdirectory (reconstruction/cat)
add_subdirectory (reconstruction/tracking)
add_subdirectory (reconstruction/globaltracking)
add_subdirectory (reconstruction/identification)
#add_subdirectory (reconstruction/tof2)
add_subdirectory (reconstruction/zdc)
add_subdirectory (reconstruction/fhcal)
add_subdirectory (reconstruction/scwall)
add_subdirectory (reconstruction/hodo)
add_subdirectory (reconstruction/ndet)
add_subdirectory (reconstruction/vacwall)
add_subdirectory (reconstruction/bmnrecotools)
add_subdirectory (reconstruction/centrality)
add_subdirectory (physics)
add_subdirectory (monitor)
add_subdirectory (online)
add_subdirectory (QA)
add_subdirectory (macro)
add_subdirectory (services)



execute_process(COMMAND ${ROOT_CONFIG_EXECUTABLE} --has-opengl
                OUTPUT_VARIABLE ROOT_HAS_OPENGL_VAR
                OUTPUT_STRIP_TRAILING_WHITESPACE)
if(${ROOT_HAS_OPENGL_VAR} MATCHES "yes")
  add_subdirectory(eventdisplay)
  Set(ROOT_HAS_OPENGL TRUE)
else()
  message(STATUS "eventdisplay will not be built, because ROOT has no opengl support.")
  Set(ROOT_HAS_OPENGL FALSE)
endif()

Option(BUILD_DOXYGEN "Build Doxygen" OFF)
if(BUILD_DOXYGEN)
  find_package2(PRIVATE Doxygen COMPONENTS dot)
  if (Doxygen_FOUND)
    if (NOT Doxygen_VERSION AND DOXYGEN_VERSION)
      set(Doxygen_VERSION "${DOXYGEN_VERSION}")
    endif()
    MESSAGE(STATUS "*** Building the Doxygen documentaion ***")
    add_subdirectory(services/doxygen)
  else()
    message(WARNING "BUILD_DOXYGEN requested, but Doxygen not found")
  endif()
endif(BUILD_DOXYGEN)

if(IWYU_FOUND)
  add_custom_target(checkHEADERS DEPENDS $ENV{ALL_HEADER_RULES})
endif(IWYU_FOUND)

#find_package2(PRIVATE ClangFormat)
#if(ClangFormat_FOUND)
#  add_custom_target(FormatCheck
#    ${CMAKE_COMMAND} -E env
#      FAIRROOT_GIT_CLANG_FORMAT_BIN=${GIT_CLANG_FORMAT_BIN}
#      FAIRROOT_CLANG_FORMAT_BIN=${CLANG_FORMAT_BIN}
#      ${CMAKE_SOURCE_DIR}/check-format.sh
#    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#  )
#endif()
#if(${CMAKE_BUILD_TYPE} MATCHES PROFILE)
#  set(BUILD_UNITTESTS ON)
#endif()
#if(BUILD_UNITTESTS)
#  find_package2(PRIVATE GTest REQUIRED VERSION 1.7.0)
#  message(STATUS "Building the unit tests.")
#  add_subdirectory(test)
#endif()

install(FILES ${CMAKE_BINARY_DIR}/install_config.sh
  DESTINATION ${CMAKE_INSTALL_PREFIX}
  RENAME bmnroot_config.sh
)
install(FILES ${CMAKE_BINARY_DIR}/check_system.sh
  DESTINATION ${CMAKE_INSTALL_PREFIX}
)
install(FILES ${CMAKE_BINARY_DIR}/BmnRootVersion.h
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include
)
#install(FILES ${CMAKE_BINARY_DIR}/install_config.csh
#  DESTINATION ${CMAKE_INSTALL_PREFIX}
#  RENAME bmnroot_config.csh
#)
#install(FILES ${CMAKE_BINARY_DIR}/check_system.csh
#  DESTINATION ${CMAKE_INSTALL_PREFIX}
#)
#install(DIRECTORY cmake
#  DESTINATION ${PROJECT_INSTALL_DATADIR}
#)

WRITE_CONFIG_FILE(config.sh)
#WRITE_CONFIG_FILE(config.csh)
SET(VMCWORKDIR ${CMAKE_INSTALL_PREFIX})
SET(ROOT_INCLUDE_PATH ${CMAKE_INSTALL_PREFIX}/include ${ROOT_INCLUDE_PATH})
WRITE_CONFIG_FILE(install_config.sh)
#WRITE_CONFIG_FILE(install_config.csh)

# Summary ######################################################################
if(CMAKE_CXX_FLAGS)
  message(STATUS "  ")
  message(STATUS "  ${Cyan}GLOBAL CXX FLAGS${CR}  ${BGreen}${CMAKE_CXX_FLAGS}${CR}")
endif()
if(CMAKE_CXX_STANDARD)
  message(STATUS "  ")
  message(STATUS "  ${Cyan}GLOBAL CXX STANDARD${CR}  ${BGreen}c++${CMAKE_CXX_STANDARD}${CR}")
endif()
if(CMAKE_CONFIGURATION_TYPES)
  message(STATUS "  ")
  message(STATUS "  ${Cyan}BUILD TYPE         CXX FLAGS${CR}")
  string(TOUPPER "${CMAKE_BUILD_TYPE}" selected_type)
  foreach(type IN LISTS CMAKE_CONFIGURATION_TYPES)
    string(TOUPPER "${type}" type_upper)
    if(type_upper STREQUAL selected_type)
      pad("${type}" 18 " " type_padded)
      message(STATUS "${BGreen}* ${type_padded}${CMAKE_CXX_FLAGS_${type_upper}}${CR}")
    else()
      pad("${type}" 18 " " type_padded)
      message(STATUS "  ${BWhite}${type_padded}${CR}${CMAKE_CXX_FLAGS_${type_upper}}")
    endif()
    unset(type_padded)
    unset(type_upper)
  endforeach()
  message(STATUS "  ")
  message(STATUS "  (Change the build type with ${BMagenta}-DCMAKE_BUILD_TYPE=...${CR})")
endif()
message(STATUS "  ")

show_dependency_info()
################################################################################
