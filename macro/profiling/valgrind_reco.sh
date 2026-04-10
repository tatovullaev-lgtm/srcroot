#!/bin/bash
#. $VMCWORKDIR/build/config.sh
valgrind \
--leak-check=full \
--show-leak-kinds=all \
--track-origins=yes \
--num-callers=64 \
--suppressions=$ROOTSYS/etc/root/valgrind-root.supp \
--suppressions=$ROOTSYS/../build/Build/root/builtins/xrootd/XROOTD-prefix/src/XROOTD/tests/XrdClTests/cppunit.supp \
--suppressions=$ROOTSYS/../build/Build/root/builtins/pcre/PCRE-prefix/src/PCRE/testdata/valgrind-jit.supp \
--suppressions=$ROOTSYS/../build/Source/zeromq/builds/valgrind/valgrind.supp \
--suppressions=$ROOTSYS/../build/Source/boost/libs/beast/tools/valgrind.supp \
--suppressions=$ROOTSYS/../build/Source/root/bindings/pyroot/cppyy/cppyy/etc/valgrind-cppyy-cling.supp \
--suppressions=$ROOTSYS/../build/Source/root/interpreter/llvm-project/llvm/utils/valgrind/i386-pc-linux-gnu.supp \
--suppressions=$ROOTSYS/../build/Source/root/interpreter/llvm-project/llvm/utils/valgrind/x86_64-pc-linux-gnu.supp \
--suppressions=$ROOTSYS/../build/Source/root/interpreter/llvm-project/clang/utils/valgrind/x86_64-pc-linux-gnu_gcc-4.3.3.supp \
--gen-suppressions=all \
--suppressions=$VMCWORKDIR/macro/profiling/valgrind/non-bmn_valgrind_2025_06.supp \
--suppressions=$VMCWORKDIR/macro/profiling/valgrind/_to_confirm.supp \
--suppressions=$VMCWORKDIR/macro/profiling/valgrind/bmn_valgrind_2025_06.supp \
--suppressions=$VMCWORKDIR/macro/profiling/valgrind/left-asis_valgrind_2025_06.supp \
--log-file=$VMCWORKDIR/macro/profiling/valgrind/val_reco.log \
./run_reco_bmn

#root.exe -l -b -q $VMCWORKDIR/macro/run/run_reco_bmn.C
#--read-var-info=yes \
#--read-inline-info=yes \

#--suppressions=$VMCWORKDIR/macro/profiling/valgrind/_ready.supp \
