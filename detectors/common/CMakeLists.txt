# detectors/common/CMakeLists.txt
set(target BmnGeoTemplate)

set(headers
  BmnGeoTemplate.h
)

#fair_change_extensions_if_exists(.cxx .h FILES "${sources}" OUTVAR headers)

add_library(${target} INTERFACE)

target_include_directories(${target} INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

install(FILES ${headers} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

