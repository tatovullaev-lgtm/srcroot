#! /bin/bash

#SBATCH -t 10-00:00:00
#SBATCH -p bmn
#SBATCH -w n02p042
#SBATCH --mem=6Gb




echo "Start single_unpack"

RUNDIR=/zfs/scratch/tatov/srcroot/macro/run/reconstruction
BUILDDIR=/zfs/scratch/tatov/srcroot/build

DATDIR=/lustre/stor2/src/run8/src/Top_2022
DIGIDIR=/zfs/scratch/tatov/Reconstruction/srcroot_digi/jun26
RECODIR=/zfs/scratch/tatov/Reconstruction/srcroot_reco/jun26
COMBDIR=/zfs/scratch/tatov/Reconstruction/Combined_Files/jun26
UPSTDIR=/zfs/scratch/tatov/Reconstruction/UpStream_Tracks/jun26
MDFDIR=/zfs/scratch/tatov/Reconstruction/MDF/jun26
ARMALIGDIR=/zfs/scratch/tatov/Reconstruction/Arm_Allignment/jun26
PHYSDIR=/zfs/scratch/tatov/Reconstruction/Physics_Tree/jun26

LOG_DATDIG_DIR=/zfs/scratch/tatov/Reconstruction/log/data_digi
LOG_DIGREC_DIR=/zfs/scratch/tatov/Reconstruction/log/digi_reco
LOG_RECCOMB_DIR=/zfs/scratch/tatov/Reconstruction/log/reco_combfiles
LOG_COMBUPST_DIR=/zfs/scratch/tatov/Reconstruction/log/comb_upst
LOG_UPSTMDF_DIR=/zfs/scratch/tatov/Reconstruction/log/upst_mdf
LOG_MDFARMALIG_DIR=/zfs/scratch/tatov/Reconstruction/log/mdf_armalig
LOG_ARMALIGPHYS_DIR=/zfs/scratch/tatov/Reconstruction/log/armalig_phys
LOG_SLURM_DIR=/zfs/scratch/tatov/Reconstruction/log/slurm



#see: sinfo
#switch to Govorun: module load GVR/v1.0-1
#          if not : module unload GVR/v1.0-1
#status my jobs   : squeue
#delete all       : scancel -u tatov
#delete jobid     : scancel jobid 271582


RUN=$1

# cyrcle for 10 runs
for (( i = ${RUN}; i <= ${RUN}; i++ ))
    do

    # data to digi
    CHKFILE="${DATDIR}/mpd_run_Top_${i}.data"
    echo "${DATDIR}/mpd_run_Top_${i}.data"
    echo "${CHKFILE}"

    if [ -f "${CHKFILE}" ]; then
	echo "data file for the run ${i} exists"
	
      root -b -q ''${RUNDIR}'/run_raw_src.C("'${DATDIR}'/mpd_run_Top_'${i}'.data","'${DIGIDIR}'/Run_'${i}'_Digi.root", 0, 8, kTRUE, kFALSE)' >> ${LOG_DATDIG_DIR}/${i}_2.log

     
       
    fi

    # # digi to reco
    # CHKFILE="${DIGIDIR}/Run_${i}_Digi.root"
    # echo "${DIGIDIR}/Run_${i}_Digi.root"

    # if [ -f "${CHKFILE}" ]; then
	# echo "digi file for the run ${i} exists"
	
    #   root -b -q ''${RUNDIR}'/run_reco_src.C("'${DIGIDIR}'/Run_'${i}'_Digi.root","'${RECODIR}'/Run_'${i}'_Reco.root", 0, 0)' >> ${LOG_DIGREC_DIR}/${i}.log

 
       
    # fi


    # # reco to combined files
    # CHKFILE="${RECODIR}/Run_${i}_Reco.root"
    # echo "${RECODIR}/Run_${i}_Reco.root"

    
    # if [ -f "${CHKFILE}" ]; then
	#   echo "reco file for the run ${i} exists"
	
    #   root -b -q ''${RUNDIR}'/Si_MWPC_Coord_For_Mass_Production.cpp('${i}')' >> ${LOG_RECCOMB_DIR}/${i}.log
    #   # too much paths to add into function, please check it in the macro
       
    # fi


    # # combined files to upstream tracks
    # CHKFILE="${COMBDIR}/Si_MWPC_DCH_dCSC_gemCSC_combined_data_${i}.root"
    # echo "${COMBDIR}/Si_MWPC_DCH_dCSC_gemCSC_combined_data_${i}.root"
    
    
    # if [ -f "${CHKFILE}" ]; then
	#   echo "combined file for the run ${i} exists"
	
    #   root -b -q ''${RUNDIR}'/Si_MWPC_Tracks_Final_Mass_Production.cpp("'${COMBDIR}'/Si_MWPC_DCH_dCSC_gemCSC_combined_data_'${i}'.root","'${UPSTDIR}'/Run_'${i}'_Upstr.root")' >> ${LOG_COMBUPST_DIR}/${i}.log
       
    # fi


    # # upstream tracks to MDF files
    # CHKFILE="${UPSTDIR}/Run_${i}_Upstr.root"
    # echo "${UPSTDIR}/Run_${i}_Upstr.root"
    
    
    # if [ -f "${CHKFILE}" ]; then
	#   echo "upstream tracks file for the run ${i} exists"
	
    #   root -b -q ''${RUNDIR}'/Reco_with_MDF.cpp("'${UPSTDIR}'/Run_'${i}'_Upstr.root","'${MDFDIR}'/Run_'${i}'_MDF.root")' >> ${LOG_UPSTMDF_DIR}/${i}.log
       
    # fi


    # # MDF to arm allignment files
    # CHKFILE="${MDFDIR}/Run_${i}_MDF.root"
    # echo "${MDFDIR}/Run_${i}_MDF.root"
    
    
    # if [ -f "${CHKFILE}" ]; then
	#   echo "MDF file for the run ${i} exists"
	
    #   root -b -q ''${RUNDIR}'/Goran_Tracks_Allignment.cpp("'${MDFDIR}'/Run_'${i}'_MDF.root","'${ARMALIGDIR}'/Run_'${i}'_GTrack_Alig.root","'${RUNDIR}'/BadTriggerList.txt")' >> ${LOG_MDFARMALIG_DIR}/${i}.log
       
    # fi


    # # arm allignment to arm tracks and physics files
    # CHKFILE="${ARMALIGDIR}/Run_${i}_GTrack_Alig.root"
    # echo "${ARMALIGDIR}/Run_${i}_GTrack_Alig.root"
    
    
    # if [ -f "${CHKFILE}" ]; then
	#   echo "arm allignment file for the run ${i} exists"
	
    #   root -b -q ''${RUNDIR}'/Goran_Tracks_Physics_M.cpp("'${ARMALIGDIR}'/Run_'${i}'_GTrack_Alig.root","'${PHYSDIR}'/Run_'${i}'_GTrack_Phys.root")' >> ${LOG_ARMALIGPHYS_DIR}/${i}.log
       
    # fi
   
   
done

#mv ${RUNDIR}/slurm*.out ${LOG_SLURM_DIR}

echo "Finish single_unpack"
#fi
