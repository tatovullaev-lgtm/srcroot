#!/bin/bash

ADDR=10.18.86.58
BMNROOT=$HOME/bmnroot

source $BMNROOT/build/config.sh

while true; 

do
${BMNROOT}/build/bin/bmn-mon-prof "tcp://${ADDR}:5602" 10002 board2
sleep 5

done
