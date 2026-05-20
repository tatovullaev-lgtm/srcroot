#! /bin/bash

#SBATCH -t 10-00:00:00
#SBATCH -p cascade

#SBATCH --mem=4Gb



#WORKDIR=/nica/mpd22/lenivenko/reco
RUNDIR=/zfs/scratch/lhep-src/srcroot_1/srcroot/macro/run/reconstruction
BUILDDIR=/zfs/scratch/lhep-src/srcroot_1/srcroot/build

DATDIR=/lustre/stor2/mpatsyuk/run8/src/Top_2022
DIGIDIR=/zfs/scratch/lhep-src/Reconstruction/srcroot_digi/may26
RECODIR=/zfs/scratch/lhep-src/Reconstruction/srcroot_reco/may26

LOG_DATDIG_DIR=/zfs/scratch/lhep-src/Reconstruction/log/data_digi
LOG_DIGREC_DIR=/zfs/scratch/lhep-src/Reconstruction/log/digi_reco



#see: sinfo
#switch to Govorun: module load GVR/v1.0-1
#          if not : module unload GVR/v1.0-1
#status my jobs   : squeue
#delete all       : scancel -u tatov
#delete jobid     : scancel jobid 271582


RUN=$1

for (( i = ${RUN}*10; i <= ${RUN}*10+9; i++ ))
    do

    # data to digi
    CHKFILE="${DATDIR}/mpd_run_Top_${i}.data"

    if [ -f "${CHKFILE}" ]; then
	echo "data file for the run ${i} exists"
	
      root -b -q ''${RUNDIR}'/run_raw_src.C("'${DATDIR}'/mpd_run_Top_'${i}'.data","'${DIGIDIR}'/Run_'${i}'_Digi.root", 0, 8, kTRUE, kFALSE)' >> ${LOG_DATDIG_DIR}/${i}.log

       # mv ${WORKDIR}/*.o* ${LOGDIR}
       
    fi

    # digi to reco
    CHKFILE="${DIGIDIR}/Run_${i}_Digi.root"

    if [ -f "${CHKFILE}" ]; then
	echo "digi file for the run ${i} exists"
	
      root -b -q ''${RUNDIR}'/run_reco_src.C("'${DIGIDIR}'/Run_'${i}'_Digi.root","'${RECODIR}'/Run_'${i}'_Reco.root", 0, 0)' >> ${LOG_DIGREC_DIR}/${i}.log

       # mv ${WORKDIR}/*.o* ${LOGDIR}
       
    fi


done
#fi
