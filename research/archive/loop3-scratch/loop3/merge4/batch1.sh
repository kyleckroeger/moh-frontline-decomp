#!/bin/sh
cd /home/kyle/workspace/moh-frontline
T=scratch/loop3/merge4
sleep 60
sh $T/land_sph.sh > $T/land_sph.log 2>&1
sleep 60
sh $T/land_fib.sh > $T/land_fib.log 2>&1
echo BATCH1 DONE
