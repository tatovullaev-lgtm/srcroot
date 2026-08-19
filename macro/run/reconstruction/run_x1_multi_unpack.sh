#!/bin/sh


if [ "$#" -lt 2 ]; then
  echo "Illegal number of parameters"
  exit 1
fi
echo "Start script"

RUNDIR=/zfs/scratch/tatov/srcroot/macro/run/reconstruction
SRCROOTDIR=/zfs/scratch/tatov/srcroot


module add CMake/v3.26.5 libexpat libxml2 GSL
source ${SRCROOTDIR}/fair_config.sh
source ${SRCROOTDIR}/build/config.sh


for iRun in $(seq $1 $2)
  do

  sbatch --job-name=Run_${iRun}_Reco_SRC ${RUNDIR}/run_x1_single_unpack.sh ${iRun}
  echo ${iRun}

done
