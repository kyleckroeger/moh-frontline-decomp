#!/bin/sh
cd /home/kyle/workspace/moh-frontline/scratch/reprove
for spec in "m1b 260" "l12b 240" "boxtri 240"; do
  set -- $spec
  sh run.sh $1 $2
  sh run.sh $1 $2 ctl
done
echo QUEUEDONE
