# Source files
#SRCS=eventdisplay.cxx

SRCS=run_reco_bmn.cxx
#SRCS=run_sim_bmn.cxx

#SRCS=run_reco_src.cxx
#SRCS=BmnDataToRoot.cxx

# Compiler
CC=g++

OBJECTS=$(patsubst %.cxx, %, $(SRCS))

ROOTCONFIG := root-config

CFLAGS := $(shell $(ROOTCONFIG) --cflags)
CFLAGS += -I$(VMCWORKDIR)/alignment
CFLAGS += -I$(VMCWORKDIR)/base
CFLAGS += -I$(VMCWORKDIR)/base/cbm
CFLAGS += -I$(VMCWORKDIR)/base/data
CFLAGS += -I$(VMCWORKDIR)/base/math
CFLAGS += -I$(VMCWORKDIR)/base/header
CFLAGS += -I$(VMCWORKDIR)/base/raw
CFLAGS += -I$(VMCWORKDIR)/base/source
CFLAGS += -I$(VMCWORKDIR)/database/uni_db
CFLAGS += -I$(VMCWORKDIR)/database/elog_db
CFLAGS += -I$(VMCWORKDIR)/database/tools
CFLAGS += -I$(VMCWORKDIR)/decoder
CFLAGS += -I$(VMCWORKDIR)/detectors
CFLAGS += -I$(VMCWORKDIR)/detectors/armTriggers
CFLAGS += -I$(VMCWORKDIR)/detectors/bc
CFLAGS += -I$(VMCWORKDIR)/detectors/bd
CFLAGS += -I$(VMCWORKDIR)/detectors/common
CFLAGS += -I$(VMCWORKDIR)/detectors/csc
CFLAGS += -I$(VMCWORKDIR)/detectors/dch
CFLAGS += -I$(VMCWORKDIR)/detectors/ecal
CFLAGS += -I$(VMCWORKDIR)/detectors/FD
CFLAGS += -I$(VMCWORKDIR)/detectors/fhcal
CFLAGS += -I$(VMCWORKDIR)/detectors/gem
CFLAGS += -I$(VMCWORKDIR)/detectors/hodo
CFLAGS += -I$(VMCWORKDIR)/detectors/land
CFLAGS += -I$(VMCWORKDIR)/detectors/mwpc
CFLAGS += -I$(VMCWORKDIR)/detectors/ndet
CFLAGS += -I$(VMCWORKDIR)/detectors/scwall
CFLAGS += -I$(VMCWORKDIR)/detectors/SiBT
CFLAGS += -I$(VMCWORKDIR)/detectors/silicon
CFLAGS += -I$(VMCWORKDIR)/detectors/SiMD
CFLAGS += -I$(VMCWORKDIR)/detectors/SiProf
CFLAGS += -I$(VMCWORKDIR)/detectors/ssd
CFLAGS += -I$(VMCWORKDIR)/detectors/sts
CFLAGS += -I$(VMCWORKDIR)/detectors/tof
CFLAGS += -I$(VMCWORKDIR)/detectors/tof1
CFLAGS += -I$(VMCWORKDIR)/detectors/tof701
CFLAGS += -I$(VMCWORKDIR)/detectors/tofcal
CFLAGS += -I$(VMCWORKDIR)/detectors/vacwall
CFLAGS += -I$(VMCWORKDIR)/detectors/zdc
CFLAGS += -I$(VMCWORKDIR)/eventdisplay
CFLAGS += -I$(VMCWORKDIR)/field
CFLAGS += -I$(VMCWORKDIR)/gconfig
CFLAGS += -I$(VMCWORKDIR)/generators
CFLAGS += -I$(VMCWORKDIR)/generators/unigen_format
CFLAGS += -I$(VMCWORKDIR)/monitor
CFLAGS += -I$(VMCWORKDIR)/passive
CFLAGS += -I$(VMCWORKDIR)/physics
CFLAGS += -I$(VMCWORKDIR)/QA
CFLAGS += -I$(VMCWORKDIR)/QA/report
CFLAGS += -I$(VMCWORKDIR)/reconstruction
CFLAGS += -I$(VMCWORKDIR)/reconstruction/bmnrecotools
CFLAGS += -I$(VMCWORKDIR)/reconstruction/cat
CFLAGS += -I$(VMCWORKDIR)/reconstruction/cat/OffLineInterface
CFLAGS += -I$(VMCWORKDIR)/reconstruction/centrality
CFLAGS += -I$(VMCWORKDIR)/reconstruction/dst
CFLAGS += -I$(VMCWORKDIR)/reconstruction/fhcal
CFLAGS += -I$(VMCWORKDIR)/reconstruction/globaltracking
CFLAGS += -I$(VMCWORKDIR)/reconstruction/hodo
CFLAGS += -I$(VMCWORKDIR)/reconstruction/identification
CFLAGS += -I$(VMCWORKDIR)/reconstruction/KF
CFLAGS += -I$(VMCWORKDIR)/reconstruction/KF/KFQA
CFLAGS += -I$(VMCWORKDIR)/reconstruction/KF/Interface
CFLAGS += -I$(VMCWORKDIR)/reconstruction/ndet
CFLAGS += -I$(VMCWORKDIR)/reconstruction/scwall
CFLAGS += -I$(VMCWORKDIR)/reconstruction/tof2
CFLAGS += -I$(VMCWORKDIR)/reconstruction/tracking
CFLAGS += -I$(VMCWORKDIR)/reconstruction/tracking/sts
CFLAGS += -I$(VMCWORKDIR)/reconstruction/vacwall
CFLAGS += -I$(VMCWORKDIR)/reconstruction/zdc
CFLAGS += -I$(VMCWORKDIR)/steering
CFLAGS += -I${FAIRROOTPATH}/include
CFLAGS += -I$(SIMPATH)/include
CFLAGS += -I$(ROOT_INCLUDE_PATH)
CFLAGS += -I$(subst :, -I,$(ROOT_INCLUDE_DIR))
CFLAGS += -I$(Geant3_INCLUDE_DIRS)
CFLAGS += -I$(subst :, -I,$(Geant4_INCLUDE_DIRS))
CFLAGS += -I$(subst :, -I,$(Geant4VMC_INCLUDE_DIRS))
CFLAGS += -I${SIMPATH}/include/root/TMVA
CFLAGS += --std=c++17 -g -O0 

LDFLAGS := $(shell $(ROOTCONFIG) --ldflags)
LDFLAGS += -lEG $(shell $(ROOTCONFIG) --glibs)
LDFLAGS += -L$(VMCWORKDIR)/build/lib
LDFLAGS += -L$(GEANT3_LIBRARY_DIR) -L$(GEANT4_LIB_DIR) -L$(GEANT4VMC_LIBRARY_DIR)
LDFLAGS += -L$(ROOT_LIBRARY_DIR) -L$(FAIRROOTPATH)/lib #-L/usr/lib64
LDFLAGS += -lTMVA -lBmnAlignment -lEventDisplay -lBmnSteering -lBmnDst -lCSC
#LDFLAGS += -lLAND
LDFLAGS += -lSilicon -lMwpc
LDFLAGS += -lSts -lBd -lEcal -lZdc -lTOF -lGem -lTOF1 -lDch -lBmnPidSRC 
LDFLAGS += -lBmnGlobalTracking -lBmnRecoTof2 -lBmnRecoZdc -lBmnRecoFHCal -lBmnRecoScWall -lBmnTracking
LDFLAGS += -lCat -lKF -lBmnField -lMpdGen -lPassive -lGen -lUniDb -lBmnDecoder
LDFLAGS += -lBmnBase -lBase -lGeoBase -lParBase -lTrkBase -lGeane -lEve -lFairTools
LDFLAGS += -lXMLParser -lXMLIO -lSpectrum -lGenVector -lMinuit -lGeom
LDFLAGS += -lGeomPainter -lVMCLibrary -lpthread -lBC -lArmTrig -lFD -lFHCal -lScWall -lGdml
LDFLAGS += -lHodo -lBmnQa -lScWall -lSiBT -lSiMD -lgeant321 -lgeant4vmc
LDFLAGS += -lCLHEP-2.4.7.1
LDFLAGS += -lpythia6 -lEGPythia6
LDFLAGS += -lBmnMonitor -lRHTTP
LDFLAGS += -lBmnRecoNdet -lBmnOnlineDevices -lBmnRecoHodo -lBmnRecoVacWall -lBmnPhysics -lBmnPidSRC -lBmnRecoTools -lBmnMisc
LDFLAGS += -lSiProf -lNdet -lTMVA
LDFLAGS += -lFairLogger -lTOF701 -lBmnRecoCentrality -lVacWall

all:	$(OBJECTS)

$(OBJECTS): % : $(SRCS)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -vf $(basename $(SRCS)) *.o

