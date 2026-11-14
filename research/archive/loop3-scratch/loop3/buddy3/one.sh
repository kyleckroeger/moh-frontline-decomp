#!/bin/sh
# one.sh dbv drb das gbo -> scored line
cd /home/kyle/workspace/moh-frontline/scratch/loop3/buddy3
d=$(mktemp -d -p /home/kyle/workspace/moh-frontline/scratch/loop3/buddy3 tmpb.XXXX)
python3 big.py $d $1 $2 $3 $4 && (ulimit -v 3000000; timeout 300 python3 ms.py $d/*.cpp)
rm -rf $d
