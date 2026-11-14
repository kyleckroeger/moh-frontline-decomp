#!/bin/sh
# run.sh NAME SECS [ctl]
cd /home/kyle/workspace/moh-frontline
N=$1; S=$2; R=scratch/reprove/r; W=/mnt/c/Users/kylec/AppData/Local/Temp/mohf-dolphin
K=${K:-"iPAD_update__Fv Wait__7CScreenFv MathFunSRandom__FUi Get_currentMission__10CShellMenuFv Get_currentStage__10CShellMenuFv"}
free -m | sed -n 2p
if [ "$3" = ctl ]; then
timeout 900 python3 research/tools/dolphin_boot.py control --work $W $R/$N.elf --secs $S --tag rp_${N}_ctl --keep $K 2>&1 | tail -1 | cut -c1-300
cp $W/rp_${N}_ctl.log $R/${N}_ctl.log
else
timeout 900 python3 research/tools/dolphin_boot.py run --work $W $R/$N.elf --secs $S --tag rp_${N}_test 2>&1 | tail -1 | cut -c1-300
cp $W/rp_${N}_test.log $R/${N}_test.log
fi
echo RUNDONE
