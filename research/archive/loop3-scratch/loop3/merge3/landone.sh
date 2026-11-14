#!/bin/sh
# landone.sh <main> <tpl> <origfile> <idx> "<others>" "<logline>" "<subject>" "<body>": lock, merge, verify, land, push, release.
R=/home/kyle/workspace/moh-frontline; cd $R
main=$1; tpl=$2; of=$3; idx=$4; others=$5; logl=$6; subj=$7; body=$8
sh research/tools/landlock.sh acquire merge3 || exit 1
if [ -n "$(git status --porcelain)" ]; then echo "DIRTY TREE"; git status --porcelain | head; sh research/tools/landlock.sh release merge3; exit 1; fi
git pull --ff-only -q
out=$( (ulimit -v 3000000; sh scratch/loop3/merge3/merge.sh $main $tpl $of $idx $others) 2>&1 ); echo "$out" | tail -3
if ! echo "$out" | grep -q "^$main: VERIFIED"; then
  echo "MERGE FAILED $main; reverting"
  git reset -q -- config src; git checkout -q -- config src; git clean -fdq config src
  sh research/tools/landlock.sh release merge3; exit 1
fi
for r in $RMFILES; do git rm -q $r; done
echo "$logl" >> research/log/autopilot.md
(ulimit -v 6000000; timeout 3000 sh research/tools/land.sh "$subj" "$body") 2>&1 | tail -3
if [ "$(git log -1 --format=%s)" = "$subj" ]; then git push 2>&1 | tail -1; else echo "LAND FAILED $main; reverting"; git status --porcelain | head; git reset -q; git checkout -q -- .; git checkout -q HEAD -- research; git clean -fdq config src; fi
sh research/tools/landlock.sh release merge3
