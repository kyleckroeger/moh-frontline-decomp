#!/bin/sh
# land3.sh <new-id> <new-src> <tpl> <origfile> <idx> "<fragments>" "<logline>" "<subject>" "<body>" ["<extra port_unit args>"]
# lock, merge (mrg.sh), verify, land, push, release; reverts on failure. Env RMFILES passes through.
R=/home/kyle/workspace/moh-frontline; cd $R
nid=$1; nsrc=$2; tpl=$3; of=$4; idx=$5; frags=$6; logl=$7; subj=$8; body=$9; shift 9; extra="$1"
sh research/tools/landlock.sh acquire merge11 || exit 1
if [ -n "$(git status --porcelain | grep -v '^ M research/plans/three-day-loop.md$')" ]; then echo "DIRTY TREE"; git status --porcelain | head; sh research/tools/landlock.sh release merge11; exit 1; fi
git pull --ff-only -q
if [ -n "$extra" ]; then a="-- $extra"; else a=""; fi
out=$( (ulimit -v 3000000; sh scratch/loop3/merge11/mrg.sh $nid $nsrc $tpl $of $idx $frags $a) 2>&1 ); echo "$out" > scratch/loop3/merge11/last_mrg.log; echo "$out" | tail -4
revert() { cp research/plans/three-day-loop.md /tmp/claude-1000/tdl.bak; git reset -q; git checkout -q HEAD -- config src research docs progress README.md include tests tools; git clean -fdq config src research/data include; cp /tmp/claude-1000/tdl.bak research/plans/three-day-loop.md; }
if ! echo "$out" | grep -q "^$nid: VERIFIED"; then
  echo "MERGE FAILED $nid; reverting"; revert
  sh research/tools/landlock.sh release merge11; exit 1
fi
echo "$logl" >> research/log/autopilot.md
(ulimit -v 6000000; timeout 3000 sh research/tools/land.sh "$subj" "$body") 2>&1 | tail -3
if [ "$(git log -1 --format=%s)" = "$subj" ]; then git push 2>&1 | tail -1; else echo "LAND FAILED $nid; reverting"; git status --porcelain | head; revert; fi
sh research/tools/landlock.sh release merge11
