# Create a library called "libBmnDecoder"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/database/uni_db
 ${CMAKE_SOURCE_DIR}/database/elog_db
 ${CMAKE_SOURCE_DIR}/database/tango
 ${CMAKE_SOURCE_DIR}/database/tools
 ${CMAKE_SOURCE_DIR}/base
 ${CMAKE_SOURCE_DIR}/base/data
 ${CMAKE_SOURCE_DIR}/base/raw
 ${CMAKE_SOURCE_DIR}/base/header
 ${CMAKE_SOURCE_DIR}/base/math
 ${CMAKE_SOURCE_DIR}/detectors/mwpc
 ${CMAKE_SOURCE_DIR}/detectors/csc
 ${CMAKE_SOURCE_DIR}/detectors/silicon
 ${CMAKE_SOURCE_DIR}/detectors/vsp
 ${CMAKE_SOURCE_DIR}/detectors/SiBT
 ${CMAKE_SOURCE_DIR}/detectors/SiProf
 ${CMAKE_SOURCE_DIR}/detectors/gem
 ${CMAKE_SOURCE_DIR}/detectors/dch
 ${CMAKE_SOURCE_DIR}/detectors/tof1
 ${CMAKE_SOURCE_DIR}/detectors/tof
 #${CMAKE_SOURCE_DIR}/detectors/tof701
 ${CMAKE_SOURCE_DIR}/detectors/ecal
 ${CMAKE_SOURCE_DIR}/detectors/zdc
 ${CMAKE_SOURCE_DIR}/detectors/scwall
 ${CMAKE_SOURCE_DIR}/detectors/hodo
 ${CMAKE_SOURCE_DIR}/detectors/fhcal
 ${CMAKE_SOURCE_DIR}/detectors/ndet
 ${CMAKE_SOURCE_DIR}/decoder
 ${CMAKE_SOURCE_DIR}/decoder/fles_cbm
 ${CMAKE_SOURCE_DIR}/decoder/fles_ipc
 ${CMAKE_SOURCE_DIR}/reconstruction/cat
 ${CMAKE_SOURCE_DIR}/reconstruction/cat/vectors
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${ROOT_INCLUDE_DIR} 
 ${Boost_INCLUDE_DIRS}
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
 BmnAdcProcessor.cxx
 BmnAdcQA.cxx
 BmnDchRaw2Digit.cxx
 BmnMwpcRaw2Digit.cxx
 BmnZDCRaw2Digit.cxx
 BmnScWallRaw2Digit.cxx
 BmnFHCalRaw2Digit.cxx
 BmnHodoRaw2Digit.cxx
 BmnNdetRaw2Digit.cxx
 BmnECALRaw2Digit.cxx
 BmnGemRaw2Digit.cxx
 BmnTof2Raw2DigitNew.cxx
 BmnTof1Raw2Digit.cxx
 BmnTof701Raw2Digit.cxx
 BmnSiliconRaw2Digit.cxx
 BmnSiBTRaw2Digit.cxx
 BmnRawDataDecoder.cxx
 BmnAbstractThread.cxx
 BmnConverterThread.cxx
 BmnThreadManager.cxx
 BmnConverter.cxx
 BmnRawSource.cxx
 IndentedConfigParser.cxx
 BmnConverterTools.cxx
 BmnDecoder.cxx
 BmnMetadataDecoder.cxx
 #BmnSlewingTOF700.cxx
 BmnTrigRaw2Digit.cxx
 BmnCscRaw2Digit.cxx
 BmnVspRaw2Digit.cxx
# BmnProfRaw2Digit.cxx
# BmnProfAsic2Raw.cxx
 BmnMscRaw2Digit.cxx
 WfmProcessor.cxx
 PronyFitter.cxx
 BmnHgndRaw2Digit.cxx
 BmnHgndCalibration.h
 #MatrixInversion.h
 #PolynomComplexRoots.h
 #PolynomRealRoots.h
 RLDeconvolutor.h
fles_ipc/ArchiveDescriptor.h
fles_ipc/MicrosliceView.h
fles_ipc/Microslice.h
fles_ipc/StorableMicroslice.h
fles_ipc/System.h
fles_ipc/TimesliceComponentDescriptor.h
fles_ipc/MicrosliceDescriptor.h
fles_ipc/StorableTimeslice.cxx
fles_ipc/Timeslice.h
fles_ipc/TimesliceDescriptor.h
fles_cbm/StsXyterMessage.h
fles_cbm/bitmask_operators.h
)

If (${CMAKE_BUILD_TYPE} STREQUAL Debug)
  add_compile_definitions(BUILD_DEBUG=${CMAKE_BUILD_TYPE})
EndIf(${CMAKE_BUILD_TYPE} STREQUAL Debug)

if (CMAKE_BUILD_TYPE STREQUAL Release)
    ADD_DEFINITIONS(-DHAVE_SSE)
    SET_SOURCE_FILES_PROPERTIES(${SRCS} PROPERTIES COMPILE_FLAGS
            "-msse4.1 -O3")
    Message(STATUS "Decoder will be compiled in -O3 SSE mode")
else()
    Message(STATUS "Decoder will be compiled in debug SSE mode")
    SET_SOURCE_FILES_PROPERTIES(${SRCS} PROPERTIES COMPILE_FLAGS
            "-msse4.1  ")
endif()

Set(HEADERS)
Set(LINKDEF DecoderLinkDef.h)
Set(LIBRARY_NAME BmnDecoder)
Set(DEPENDENCIES FHCal Hodo ScWall Ndet Zdc Ecal TOF TOF1 CSC Gem Silicon SiProf VSP
SiBT Dch Mwpc BmnBase DbTools UniDb ElogDb TangoData Base Minuit Core ROOTDataFrame TreePlayer boost_program_options fmt)

GENERATE_LIBRARY()
