#!/bin/bash

ADDR="tcp://localhost:5603"

BMNROOT=$HOME/bmnroot

source $BMNROOT/build/config.sh

while true; 

do
/opt/fairsoft/install/bin/root.exe -b -q $BMNROOT"/macro/monitor/run_mon_prof.C(\"${ADDR}\")"
sleep 20

done
