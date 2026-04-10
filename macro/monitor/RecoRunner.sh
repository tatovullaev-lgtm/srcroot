#!/bin/bash

BMNROOT=$HOME/bmnroot

source $BMNROOT/build/config.sh

while true; 

do
/opt/fairsoft/install/bin/root.exe -b -q $BMNROOT/macro/monitor/run_reco_bmn.C
sleep 20

done
