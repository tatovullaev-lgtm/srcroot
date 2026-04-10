#!/bin/bash

BMNROOT=$HOME/bmnroot

source $BMNROOT/build/config.sh

root -l -b -q  $BMNROOT'/macro/raw/BmnMetadataCollect.C({}, "bmn_run'$1'_meta.root", '$1')'

root -l -b -q  $BMNROOT'/macro/raw/readSpillStat.C("bmn_run'$1'_meta.root")' >> ss_$1.txt