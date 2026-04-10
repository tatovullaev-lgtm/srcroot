# Create executable files for online processing flow

set(INCLUDE_DIRECTORIES
  ${BASE_INCLUDE_DIRECTORIES}
  ${CMAKE_SOURCE_DIR}/base
  ${CMAKE_SOURCE_DIR}/base/cbm
  ${CMAKE_SOURCE_DIR}/base/data
  ${CMAKE_SOURCE_DIR}/base/header
  ${CMAKE_SOURCE_DIR}/base/math
  ${CMAKE_SOURCE_DIR}/base/raw
  ${CMAKE_SOURCE_DIR}/database/uni_db
  ${CMAKE_SOURCE_DIR}/decoder
  ${CMAKE_SOURCE_DIR}/detectors/csc
  ${CMAKE_SOURCE_DIR}/detectors/dch
  ${CMAKE_SOURCE_DIR}/detectors/fhcal
  ${CMAKE_SOURCE_DIR}/detectors/gem
  ${CMAKE_SOURCE_DIR}/detectors/hodo
  ${CMAKE_SOURCE_DIR}/detectors/mwpc
  ${CMAKE_SOURCE_DIR}/detectors/ndet
  ${CMAKE_SOURCE_DIR}/detectors/scwall
  ${CMAKE_SOURCE_DIR}/detectors/SiBT
  ${CMAKE_SOURCE_DIR}/detectors/silicon
  ${CMAKE_SOURCE_DIR}/detectors/sts
  #${CMAKE_SOURCE_DIR}/detectors/tof
  ${CMAKE_SOURCE_DIR}/detectors/tof1
  #${CMAKE_SOURCE_DIR}/detectors/tof2
  #${CMAKE_SOURCE_DIR}/detectors/tof701
  ${CMAKE_SOURCE_DIR}/detectors/vsp
  ${CMAKE_SOURCE_DIR}/field
  ${CMAKE_SOURCE_DIR}/physics
  ${CMAKE_SOURCE_DIR}/physics/decay
  ${CMAKE_SOURCE_DIR}/physics/efficiency
  ${CMAKE_SOURCE_DIR}/physics/embedding
  ${CMAKE_SOURCE_DIR}/reconstruction/cat
  ${CMAKE_SOURCE_DIR}/reconstruction/cat/L1Algo
  ${CMAKE_SOURCE_DIR}/reconstruction/cat/OffLineInterface
  ${CMAKE_SOURCE_DIR}/reconstruction/cat/ParticleFinder
  ${CMAKE_SOURCE_DIR}/reconstruction/centrality
  ${CMAKE_SOURCE_DIR}/reconstruction/dst
  ${CMAKE_SOURCE_DIR}/reconstruction/fhcal
  ${CMAKE_SOURCE_DIR}/reconstruction/globaltracking
  ${CMAKE_SOURCE_DIR}/reconstruction/hodo
  ${CMAKE_SOURCE_DIR}/reconstruction/KF
  ${CMAKE_SOURCE_DIR}/reconstruction/KF/Interface
  ${CMAKE_SOURCE_DIR}/reconstruction/KF/KFParticlesFinder
  ${CMAKE_SOURCE_DIR}/reconstruction/KF/KFQA
  ${CMAKE_SOURCE_DIR}/reconstruction/ndet
  ${CMAKE_SOURCE_DIR}/reconstruction/scwall
  #${CMAKE_SOURCE_DIR}/reconstruction/tof2
  ${CMAKE_SOURCE_DIR}/reconstruction/tracking
  ${CMAKE_SOURCE_DIR}/reconstruction/tracking/sts
  ${CMAKE_SOURCE_DIR}/reconstruction/vacwall
  ${CMAKE_SOURCE_DIR}/services
)

set(SYSTEM_INCLUDE_DIRECTORIES
  ${ROOT_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
)

set(LINK_DIRECTORIES
  ${ROOT_LIBRARY_DIR}
  ${FAIRROOT_LIBRARY_DIR}
  ${Boost_LIBRARY_DIRS}
)

include_directories(${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

link_directories(${LINK_DIRECTORIES})


set(EXE_DEPENDENCIES
  UniDb
  BmnField
  BmnDst
  SiBT
  Silicon
  Gem
  CSC
  TOF1
  #TOF701
  #BmnRecoTof2
  BmnRecoFHCal
  Sts
  KF
  Cat
  BmnTracking
  BmnGlobalTracking
  FairMQ::FairMQ
  ${ROOT_LIBRARIES}
  ${Boost_PROGRAMM_OPTIONS_LIBRARY}
  ${Boost_INTERPROCESS_LIBRARY}
  rt
)

add_executable(bmn-online-sampler sampler.cxx)
target_link_libraries(bmn-online-sampler PRIVATE ${EXE_DEPENDENCIES})
install(PROGRAMS ${EXECUTABLE_OUTPUT_PATH}/bmn-online-sampler DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/)

add_executable(bmn-online-decoder-proxy decoder_proxy.cxx)
target_link_libraries(bmn-online-decoder-proxy PRIVATE ${EXE_DEPENDENCIES})
install(PROGRAMS ${EXECUTABLE_OUTPUT_PATH}/bmn-online-decoder-proxy DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/)

add_executable(bmn-online-reco-processor reco_processor.cxx)
target_link_libraries(bmn-online-reco-processor PRIVATE ${EXE_DEPENDENCIES})
install(PROGRAMS ${EXECUTABLE_OUTPUT_PATH}/bmn-online-reco-processor DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/)

