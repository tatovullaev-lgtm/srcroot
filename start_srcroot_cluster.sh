#!/bin/sh

echo "Start script"

SRCROOTDIR=/zfs/scratch/tatov/srcroot


module add CMake/v3.26.5 libexpat libxml2 GSL

source ${SRCROOTDIR}/fair_config.sh

source ${SRCROOTDIR}/SetEnv.sh
source ${SRCROOTDIR}/build/config.sh


