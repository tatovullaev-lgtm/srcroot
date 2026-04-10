#!/bin/bash
#SBATCH -J job-name
#SBATCH -n 1                       # ntasks per job (default, 1)
#SBATCH -c 1                       # cores per task (default, 1)
#SBATCH --mem=2000mb               # max memory for job (optional)
#SBATCH --mail-type=END,FAIL       # mail events (optional)
#SBATCH --mail-user=user@jinr.ru   # where to send mail (optional)

echo "  Start date: `date` (`hostname`)"

#source "[PATH_TO_BUILD_DIR]/build/config.sh"
#root -b -q '$VMCWORKDIR/macro/your_root_macro.C(your_int_parameter, "your_string_parameter"...)'

echo "  End date: `date`"
