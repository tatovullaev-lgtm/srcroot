#!/bin/bash
# marge.sh is the same for different batch systems (SGE, Torque, SLURM) because of comment symbol

# SGE params
#$ -j y
#$ -N merge-result
#$ -cwd
# request Bourne again shell as shell for job
#$ -S /bin/bash

# TORQUE params
#PBS -j oe
#PBS -N merge-result
#PBS -d .
# request Bourne again shell as shell for job
#PBS -S /bin/bash

# SLURM params
#SBATCH -J merge-result

echo "Start date of file merge operation: `date` (`hostname`)"

if [ "$ConfigFiles" != "" ]
then
 eval $ConfigFiles
fi

root -b -q "./merge.C(\"$MergeFiles\", $MergeMode)"

echo "  End date of file merge operation: `date`"
