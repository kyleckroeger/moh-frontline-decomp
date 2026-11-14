#!/bin/sh
# mk.sh name : reads body from stdin, writes tlt/name.cpp, scores
D=/home/kyle/workspace/moh-frontline/scratch/loop3/struct6
{ cat $D/tlt/pre.cpp; cat; } > $D/tlt/$1.cpp
sh $D/t.sh $1.cpp $2
