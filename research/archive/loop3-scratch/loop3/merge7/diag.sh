#!/bin/sh
# diag.sh id src tpl origfile idx "frags" "extra": lock, merge, reconstruct+docs, run tests verbosely, revert, release
R=/home/kyle/workspace/moh-frontline; cd $R; T=scratch/loop3/merge7
sh research/tools/landlock.sh acquire merge7 || exit 1
revert() { cp research/plans/three-day-loop.md /tmp/claude-1000/tdl.bak; git reset -q; git checkout -q HEAD -- config src research docs progress README.md include tests tools; git clean -fdq config src research/data include; cp /tmp/claude-1000/tdl.bak research/plans/three-day-loop.md; }
if [ -n "$(git status --porcelain | grep -v '^ M research/plans/three-day-loop.md$')" ]; then echo DIRTY; sh research/tools/landlock.sh release merge7; exit 1; fi
if [ -n "$7" ]; then a="-- $7"; else a=""; fi
(ulimit -v 3000000; sh $T/mrg.sh $1 $2 $3 $4 $5 $6 $a) 2>&1 | tail -3
python3 tools/reconstruct.py | tail -1
python3 tools/progress_report.py --capture 2>&1 | tail -1
python3 tools/port_manifest.py >/dev/null; python3 tools/sync_docs.py
python3 -m unittest discover -s tests 2>&1 | grep -B5 -A25 "FAIL:\|ERROR:" | head -60
revert
sh research/tools/landlock.sh release merge7
