# Create a library called "libBmnMonitor"

Set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/database/uni_db
 ${CMAKE_SOURCE_DIR}/database/elog_db
 ${CMAKE_SOURCE_DIR}/database/tango
 ${CMAKE_SOURCE_DIR}/database/tools
 ${CMAKE_SOURCE_DIR}/field
 ${CMAKE_SOURCE_DIR}/base
 ${CMAKE_SOURCE_DIR}/base/data
 ${CMAKE_SOURCE_DIR}/base/raw
 ${CMAKE_SOURCE_DIR}/base/header
 ${CMAKE_SOURCE_DIR}/base/math
 ${CMAKE_SOURCE_DIR}/base/cbm
 ${CMAKE_SOURCE_DIR}/base/source
 ${CMAKE_SOURCE_DIR}/QA
 ${CMAKE_SOURCE_DIR}/QA/report
 ${CMAKE_SOURCE_DIR}/detectors/mwpc
 ${CMAKE_SOURCE_DIR}/detectors/csc
 ${CMAKE_SOURCE_DIR}/detectors/dch
 ${CMAKE_SOURCE_DIR}/detectors/silicon
 ${CMAKE_SOURCE_DIR}/detectors/SiBT
 ${CMAKE_SOURCE_DIR}/detectors/SiProf
 ${CMAKE_SOURCE_DIR}/detectors/gem
 ${CMAKE_SOURCE_DIR}/detectors/sts
 ${CMAKE_SOURCE_DIR}/detectors/tof1
 ${CMAKE_SOURCE_DIR}/detectors/tof
 #${CMAKE_SOURCE_DIR}/detectors/tof701
 ${CMAKE_SOURCE_DIR}/detectors/ecal
 ${CMAKE_SOURCE_DIR}/detectors/zdc
 ${CMAKE_SOURCE_DIR}/detectors/vsp
 ${CMAKE_SOURCE_DIR}/detectors/hodo
 ${CMAKE_SOURCE_DIR}/detectors/scwall
 ${CMAKE_SOURCE_DIR}/detectors/fhcal
 ${CMAKE_SOURCE_DIR}/detectors/ndet
 ${CMAKE_SOURCE_DIR}/detectors/common
 ${CMAKE_SOURCE_DIR}/alignment
 ${CMAKE_SOURCE_DIR}/decoder
 ${CMAKE_SOURCE_DIR}/reconstruction/dst
 ${CMAKE_SOURCE_DIR}/reconstruction/tof2
 ${CMAKE_SOURCE_DIR}/reconstruction/KF
 ${CMAKE_SOURCE_DIR}/reconstruction/KF/Interface
 ${CMAKE_SOURCE_DIR}/reconstruction/tracking
 ${CMAKE_SOURCE_DIR}/reconstruction/globaltracking
 ${CMAKE_SOURCE_DIR}/monitor
 ${CMAKE_SOURCE_DIR}/steering
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${ROOT_INCLUDE_DIR}
 ${XML2_INCLUDE_DIR}
 ${ZeroMQ_INCLUDE_DIR}
 ${Boost_INCLUDE_DIRS}
 ${JSON_INCLUDE_DIR}
)

include_directories(${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

Set(LINK_DIRECTORIES
 ${ROOT_LIBRARY_DIR}
 ${FAIRROOT_LIBRARY_DIR}
 ${Boost_LIBRARY_DIRS}
)

link_directories(${LINK_DIRECTORIES})

# List of source files
Set(SRCS
BmnMonitor.cxx
BmnDataReceiver.cxx
BmnOnlineDecoder.cxx
BmnHistGem.cxx
BmnHistVsp.cxx
BmnHistCsc.cxx
BmnHistSilicon.cxx
BmnHistZDC.cxx
BmnHistScWall.cxx
BmnHistECAL.cxx
BmnHistTrigger.cxx
BmnHistToF.cxx
BmnHistToF700.cxx
BmnHistFHCal.cxx
BmnHistHodo.cxx
BmnHistNdet.cxx
BmnHistStat.cxx
BmnHistSiBT.cxx
#BmnMQRecoTaskProcessor.cxx
#BmnMQFileSaver.cxx
 BmnMQSource.cxx
 BmnMQSink.cxx
 #BmnMQRunDevice.cxx
)

add_executable(bmn-mon-prof BmnMonProf.cxx)
target_link_libraries(bmn-mon-prof PRIVATE
 ${ZeroMQ_LIBRARY_SHARED}
 Core
 RIO
 RHTTP
 Hist
 BmnDecoder
)

target_link_directories(bmn-mon-prof PRIVATE
 ${LINK_DIRECTORIES}
)

Set(HEADERS)
Set(LINKDEF MonitorLinkDef.h)
Set(LIBRARY_NAME BmnMonitor)
Set(DEPENDENCIES BmnQa BmnGlobalTracking BmnTracking BmnDecoder Alignment
    FHCal ScWall Hodo Ndet Zdc Ecal TOF1 Gem Silicon SiBT CSC VSP
    BmnBase BmnField DbTools ElogDb UniDb FairMQ::FairMQ Base Core ${ZeroMQ_LIBRARY_SHARED})

#ROOT_GENERATE_DICTIONARY(bmn-mon-prof-dict BmnMonProf.cxx MODULE bmn-mon-prof LINKDEF MonitorLinkDef.h)

GENERATE_LIBRARY()
