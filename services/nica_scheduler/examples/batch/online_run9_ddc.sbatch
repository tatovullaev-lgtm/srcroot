#!/bin/bash
#SBATCH -J online_processing
#SBATCH -n 1                       # ntasks per job (default, 1)
#SBATCH -c 1                       # cores per task (default, 1)

echo "  Start date: `date` (`hostname`)"

export input_filename_ext=$(basename -- "$input_raw_file")
export input_filename="${input_filename_ext%.*}"
#input_extension="${input_filename_ext##*.}"
output_digi="/eos/lpp/nica/bmn/exp/digi/run9/online/$input_filename.root"
output_dst="/eos/lpp/nica/bmn/exp/dst/run9/online/$input_filename.root"

source /cvmfs/bmn.jinr.ru/bmnroot/dev/x86_64-alma9/bmnroot_config.sh
#root -b -q '$VMCWORKDIR/macro/raw/BmnDataToRoot.C("$input_raw_file","$output_digi",0,9)'
#root -b -q '$VMCWORKDIR/macro/run/run_reco_bmn.C("$output_digi","$output_dst",0,0)'
eos cp -n $input_raw_file /ceph/bmn/ssd/tmp/${input_filename_ext}
root -b -q '$VMCWORKDIR/macro/raw/BmnDataToRoot.C("/ceph/bmn/ssd/tmp/${input_filename_ext}","/ceph/bmn/ssd/tmp/${input_filename}_digi.root",0,9)'
root -b -q '$VMCWORKDIR/macro/run/run_reco_bmn.C("/ceph/bmn/ssd/tmp/${input_filename}_digi.root","/ceph/bmn/ssd/tmp/${input_filename}_dst.root",0,0)'
eos cp -n /ceph/bmn/ssd/tmp/${input_filename}_digi.root $output_digi
eos cp -n /ceph/bmn/ssd/tmp/${input_filename}_dst.root $output_dst
rm -rf /ceph/bmn/ssd/tmp/${input_filename_ext}
rm -rf /ceph/bmn/ssd/tmp/${input_filename}_digi.root
rm -rf /ceph/bmn/ssd/tmp/${input_filename}_dst.root

echo "  End date: `date`"
