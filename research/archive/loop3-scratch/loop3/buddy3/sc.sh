#!/bin/sh
# sc.sh src.cpp -> key buddy scores (tmpl_pf)
cd /home/kyle/workspace/moh-frontline
(ulimit -v 3000000; timeout 300 python3 scratch/buddy/cmpall.py scratch/loop3/buddy2/tmpl_pf.json $1 2>&1 | grep -v MISSING | grep -E "Checkpoints|Progress|Died|GetInitial|AddBuddy|Create|TOTAL|rror" | awk '{printf "%s %s | ", $1, $4} END {print ""}')
