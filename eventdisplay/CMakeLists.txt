# Create a library called "libEventDisplay" 

Set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/config
 ${CMAKE_SOURCE_DIR}/eventdisplay
)

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${ROOT_INCLUDE_DIR} 
  ${Geant3_INCLUDE_DIRS}
  ${XML2_INCLUDE_DIR}
)

set(LINK_DIRECTORIES
 ${ROOT_LIBRARY_DIR}
 ${XML2_LIBRARY_DIR}
 ${FAIRROOT_LIBRARY_DIR}
 ${Boost_LIBRARY_DIRS}
)

set(SRCS
 MpdEventManager.cxx  
 MpdMCTracks.cxx
 MpdEventManagerEditor.cxx
 MpdMCTracksEditor.cxx
 MpdBoxSet.cxx
 MpdBoxSetDraw.cxx
 MpdBoxSetEditor.cxx
 MpdPointSetDraw.cxx
 MpdMCPointDraw.cxx
 MpdHitDraw.cxx
 MpdXMLNode.cxx
 MpdHitPointSetDraw.cxx
 MpdWebScreenshots.cxx
)
if (Geant3_FOUND)
  Set(SRCS
    ${SRCS}
     MpdMCStack.cxx
  )
endif (Geant3_FOUND)

if (BMNROOT)
  Set(INCLUDE_DIRECTORIES
    ${INCLUDE_DIRECTORIES}
    ${CMAKE_SOURCE_DIR}/base
    ${CMAKE_SOURCE_DIR}/base/data
    ${CMAKE_SOURCE_DIR}/base/cbm
    ${CMAKE_SOURCE_DIR}/reconstruction/globaltracking
    ${CMAKE_SOURCE_DIR}/detectors/silicon
    ${CMAKE_SOURCE_DIR}/detectors/gem
    ${CMAKE_SOURCE_DIR}/detectors/dch
    ${CMAKE_SOURCE_DIR}/detectors/zdc
    ${CMAKE_SOURCE_DIR}/services
    ${CMAKE_SOURCE_DIR}/eventdisplay/bmn
  )
  Set(SRCS
    ${SRCS}
    bmn/BmnTrackDrawH.cxx
    bmn/BmnGlobalTrackDraw.cxx
    bmn/BmnCaloTowerDraw.cxx
    bmn/FairMCModuleDraw.cxx
  )
  if (Geant3_FOUND)
    Set(SRCS
      ${SRCS}
       bmn/BmnTrackDrawP.cxx
    )
  endif (Geant3_FOUND)
endif (BMNROOT)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

link_directories(${LINK_DIRECTORIES})

Set(HEADERS)

Set(LINKDEF EventDisplayLinkDef.h)
if (Geant3_FOUND)
 Set(LINKDEF EventGDisplayLinkDef.h)
endif (Geant3_FOUND)
if (BMNROOT)
 Set(LINKDEF bmn/EventBLinkDef.h)
 if (Geant3_FOUND)
  Set(LINKDEF bmn/EventBGLinkDef.h)
 endif (Geant3_FOUND)
endif (BMNROOT)

Set(DEPENDENCIES Base XMLParser Eve Geom EG Physics Ged Gui Graf3d Tree RGL Thread XMLIO Core ${XML2_LIBRARIES})
if (Geant3_FOUND)
 Set(DEPENDENCIES Geane geant321 ${DEPENDENCIES})
endif (Geant3_FOUND)
if (BMNROOT)
 Set(DEPENDENCIES BmnGlobalTracking Gem Silicon Dch BmnBase ${DEPENDENCIES})
endif (BMNROOT)

Set(LIBRARY_NAME EventDisplay)

GENERATE_LIBRARY()
