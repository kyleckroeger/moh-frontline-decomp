#!/bin/sh
# sc.sh tmpl src func addr size -> fast score + structural
cd /home/kyle/workspace/moh-frontline
(ulimit -v 3000000; timeout 300 python3 scratch/game/fast.py $1 $2 $3; timeout 300 python3 scratch/loop3/scripting/tools/sdiff.py $1 $2 $3 $4 $5 $6)
