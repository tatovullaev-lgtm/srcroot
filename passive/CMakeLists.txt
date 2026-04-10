 ################################################################################
 #    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    #
 #                                                                              #
 #              This software is distributed under the terms of the             #
 #              GNU Lesser General Public Licence (LGPL) version 3,             #
 #                  copied verbatim in the file "LICENSE"                       #
 ################################################################################
# Create a library called "libPassive" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(INCLUDE_DIRECTORIES
  ${BASE_INCLUDE_DIRECTORIES}
  ${CMAKE_SOURCE_DIR}/passive
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${ROOT_INCLUDE_DIR}
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

Set(LINK_DIRECTORIES
  ${ROOT_LIBRARY_DIR}
  ${FAIRROOT_LIBRARY_DIR}
) #${VMC_DIR}

Link_Directories(${LINK_DIRECTORIES})

Set(SRCS
  FairCave.cxx
  FairGeoCave.cxx
  FairGeoMagnet.cxx
  FairGeoPassivePar.cxx
  FairGeoPipe.cxx
  FairGeoTarget.cxx
  FairMagnet.cxx
  FairPassiveContFact.cxx
  FairPipe.cxx
  FairTarget.cxx
  FairPassiveDet.cxx
)

Set(HEADERS)
Set(LINKDEF PassiveLinkDef.h)
Set(LIBRARY_NAME Passive)
Set(DEPENDENCIES GeoBase ParBase Base FairLogger::FairLogger Geom Core)

If ((${FairRoot_VERSION_MAJOR} VERSION_GREATER_EQUAL "18") AND (${FairRoot_VERSION_MINOR} VERSION_GREATER_EQUAL "8") AND (${FairRoot_VERSION_PATCH} VERSION_GREATER_EQUAL "0"))
 add_definitions(-DFairRoot_18_8_0)
EndIf()

GENERATE_LIBRARY()
