#!/bin/sh
# t.sh file.cpp [-v] : fast score + structural for TLT
cd /home/kyle/workspace/moh-frontline
F=TLT_LoadFromRepository__FPCcPiPb
D=scratch/loop3/struct6/tlt
(ulimit -v 3000000; timeout 300 python3 scratch/game/fast.py $D/td.json $D/$1 $F | head -1; timeout 300 python3 scratch/loop3/scripting/tools/sdiff.py $D/td.json $D/$1 $F 8001cca8 572 $2)
