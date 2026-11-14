#!/bin/sh
# v.sh base.cpp fnfile "signature" func-substr  -> score
W=scratch/loop3/buddy2
cp $1 $W/_v.cpp
python3 $W/swapfn.py $W/_v.cpp $2 "$3"
(ulimit -v 3000000; timeout 300 python3 scratch/buddy/cmpall.py $W/tmpl_pf.json $W/_v.cpp "$4" 2>&1 | grep -v MISSING | head -${N:-3})
