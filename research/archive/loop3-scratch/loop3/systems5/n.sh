#!/bin/sh
# n.sh file func -> number of diff lines
cd /home/kyle/workspace/moh-frontline/scratch/loop3/systems5
for f in "$@"; do case $f in *.cpp) ;; *) continue;; esac
 n=$( (ulimit -v 3000000; timeout 300 python3 d.py $f ${FN:-CreateDestructable__FP20TriggerObject_structi} 0) 2>&1 | grep -c '^[-+][^-+]'); echo "$n $f"; done
