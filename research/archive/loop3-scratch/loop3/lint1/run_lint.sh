#!/bin/sh
cd /home/kyle/workspace/moh-frontline
sh research/tools/landlock.sh acquire lint1 || exit 1
git status --porcelain
free -m
avail=$(free -m | awk '/Mem:/{print $7}')
[ "$avail" -lt 2500 ] && { echo "LOW MEMORY $avail"; exit 2; }
date
( ulimit -v 3000000; timeout 1800 python3 research/tools/layout_lint.py --jobs 4 ) > scratch/loop3/lint1/lint.out 2>&1
echo "lint exit $?"
date
tail -15 scratch/loop3/lint1/lint.out
