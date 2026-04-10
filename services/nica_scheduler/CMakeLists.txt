# The name of the project is "NICA_SCHEDULER".
# CMakeLists files in the project can refer to the root source directory as ${NICA_SCHEDULER_SOURCE_DIR} or as ${CMAKE_SOURCE_DIR}
# and to the root binary directory as ${NICA_SCHEDULER_BINARY_DIR} or ${CMAKE_BINARY_DIR}.
cmake_minimum_required(VERSION 3.10.0)

# for integrated version if $SIMPATH variable is defined
IF(DEFINED ENV{SIMPATH})
  include(cmake/CMakeLists.cmake)
# for stand-alone version
ELSE(DEFINED ENV{SIMPATH})
  project(NICA_SCHEDULER NONE)
  include(cmake/CMakeListsSAV.cmake)
ENDIF()
