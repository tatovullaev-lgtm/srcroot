#!/bin/bash

#ADDR="localhost"
ADDR=10.18.86.58

BMNROOT=$HOME/bmnroot

source $BMNROOT/build/config.sh

while true; 

do
/opt/fairsoft/install/bin/root.exe -b -q $BMNROOT"/macro/monitor/run_prof_deco.C(\"${ADDR}\")"
sleep 20

done
