#!/bin/sh
# nr.sh tmpl func start end files... : count non-relocation differing lines
cd /home/kyle/workspace/moh-frontline
t=$1; f=$2; a=$3; e=$4; shift 4
for s in "$@"; do
 n=$(python3 research/tools/sbs.py $t $s $f $a $e 2>/dev/null | grep '<<' | grep -v '0(0)\|lis r[0-9]*,0 \|lis r[0-9]*,0$\|,0(r[0-9]*) *<<' | wc -l)
 echo "$n $(basename $s)"
done
