#!/bin/bash
# batch_job.sh is the same for different batch systems (SGE, Torque, SLURM) because of comment symbol

# SGE params
#$ -j y
#$ -cwd
# request Bourne again shell as shell for job
#$ -S /bin/bash

# SLURM params
#SBATCH --ignore-pbs

# TORQUE params
#PBS -j oe
#PBS -d .
# request Bourne again shell as shell for job
#PBS -S /bin/bash

# common info
echo "Start date: `date` (host: `hostname`)"

# if configuration file was set then execute it
if [ "$config" != "" ]
then
 eval $config
fi

# if non-ROOT command line
if [ "$sched_command_line" != "" ]
then
 eval $sched_command_line 
 echo "  End date: `date`"
 exit 0
fi

#cat "${files}"
if [ "$SGE_TASK_ID" != "" ]
then
 line="$(sed -n "$SGE_TASK_ID{p;q;}" "$files")"
elif [ "$SLURM_ARRAY_TASK_ID" != "" ]
then
 line="$(sed -n "$SLURM_ARRAY_TASK_ID{p;q;}" "$files")"
elif [ "$PBS_ARRAYID" != "" ]
then
 line="$(sed -n "$PBS_ARRAYID{p;q;}" "$files")"
else
 echo "  ERROR: Unknown batch system for TASK ID"
fi

arr=(${line// / })
comm="root -b -q '$macro("
counter=1
for i in "${arr[@]}"
do
 if [ $counter -gt 1 ]
 then
  comm="$comm,"
 fi
 comm=$comm$i
 let counter=$counter+1
done
comm="$comm)'"

eval $comm

#rm "${files}"

echo "  End date: `date`"
exit 0
