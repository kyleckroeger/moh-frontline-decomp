#!/bin/sh
# s.sh src [func start end] -> score + sbs file
cd /home/kyle/workspace/moh-frontline
F=${2:-CreateGerman__FP20TriggerObject_structib}; A=${3:-0x8003f1ec}; B=${4:-0x8003f8b0}
(ulimit -v 3000000; timeout 300 python3 scratch/game/fast.py scratch/loop3/systems4/t.json scratch/loop3/systems4/$1 $F)
(cd research/tools && ulimit -v 3000000; timeout 300 python3 sbs.py ../../scratch/loop3/systems4/t.json ../../scratch/loop3/systems4/$1 $F $A $B) > scratch/loop3/systems4/${1%.cpp}.sbs 2>&1
