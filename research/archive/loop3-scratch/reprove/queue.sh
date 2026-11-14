#!/bin/sh
cd /home/kyle/workspace/moh-frontline/scratch/reprove
for spec in "m1 260" "menu 90" "l12 240" "l21 240"; do
  set -- $spec
  sh run.sh $1 $2
  sh run.sh $1 $2 ctl
done
echo QUEUEDONE
