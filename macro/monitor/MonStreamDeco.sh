#!/bin/bash
# 
# test  socat -u FILE:/ncx/eos/nica/bmn/exp/raw/run8/src_test/Top_2021_Dec/mpd_run_Top_090.data TCP4-LISTEN:32999,reuseaddr
# port logic  2132{id}
#
#c5n17: Top-0 #21320; Top-1 #21321
#c5n18: Top-2 #21322; Top-3 #21323
#c5n19: Top-4 #21324; Top-5 #21325
#c5n20: Top-6 #21326; Top-7 #21327

BMNROOT=$HOME/bmnroot

source $BMNROOT/build/config.sh

while true; 

do
/opt/fairsoft/install/bin/root.exe -b -q $BMNROOT'/macro/monitor/monStreamDecoder.C("c5n12.he.jinr.ru:21325",9)'
sleep 5

done
