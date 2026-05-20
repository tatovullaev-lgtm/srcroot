# GCC 14.3.0
export PATH=/cvmfs/sft.cern.ch/lcg/releases/gcc/14.3.0-c8dfb/x86_64-el9/bin:$PATH
export LD_LIBRARY_PATH="/cvmfs/sft.cern.ch/lcg/releases/gcc/14.3.0-c8dfb/x86_64-el9/lib64${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
export LD_LIBRARY_PATH="/cvmfs/sft.cern.ch/lcg/releases/gcc/14.3.0-c8dfb/x86_64-el9/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
export CPATH=/cvmfs/sft.cern.ch/lcg/releases/gcc/14.3.0-c8dfb/x86_64-el9/include:$CPATH
gcc_home=/cvmfs/sft.cern.ch/lcg/releases/gcc/14.3.0-c8dfb/x86_64-el9/
export FC=$(which gfortran)
export CC=$(which gcc)
export CXX=$(which g++)
export COMPILER_PATH=${gcc_home}

# XercesC 3.2.4
export CMAKE_PREFIX_PATH=/cvmfs/sft.cern.ch/lcg/releases/XercesC/3.2.4-9e637/x86_64-el9-gcc14-opt:$CMAKE_PREFIX_PATH
export XERCESC_ROOT=/cvmfs/sft.cern.ch/lcg/releases/XercesC/3.2.4-9e637/x86_64-el9-gcc14-opt
export XERCESC_ROOT_DIR=/cvmfs/sft.cern.ch/lcg/releases/XercesC/3.2.4-9e637/x86_64-el9-gcc14-opt
export LD_LIBRARY_PATH="/cvmfs/sft.cern.ch/lcg/releases/XercesC/3.2.4-9e637/x86_64-el9-gcc14-opt/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
export CPATH=/cvmfs/sft.cern.ch/lcg/releases/XercesC/3.2.4-9e637/x86_64-el9-gcc14-opt/include:$CPATH

# FairSoft jan24p5 and FairRoot v18.8.2
export SIMPATH=/cvmfs/bmn.jinr.ru/fairsoft/jan24p5/x86_64-alma9
export FAIRROOTPATH=/cvmfs/bmn.jinr.ru/fairroot/v18.8.2/x86_64-alma9

echo "========================================================================"
echo "                  Dear users of the BM@N experiment"
echo
echo "       The environment for working with the current versions"
echo "           of FairSoft and FairRoot on the cluster loaded"
echo
echo " FairSoft: $SIMPATH"
echo " FairRoot: $FAIRROOTPATH"
echo
echo "          Please, carefully read the corresponding manual"
echo "                  before working on the cluser"
echo
echo " NICA cluster (ncx): https://bmn.jinr.int/how-to-use-the-nica-cluster"
echo " CICC complex (lxui): https://bmn.jinr.int/jinr-cicc-complex"
echo " HybriLIT platform (hydra): https://bmn.jinr.int/hybrilit-govorun"
echo "========================================================================"
echo
