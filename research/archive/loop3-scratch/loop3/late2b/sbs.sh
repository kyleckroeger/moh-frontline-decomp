#!/bin/sh
# sbs.sh <function> [draftfile]: side by side for an index row (draft overridable)
cd /home/kyle/workspace/moh-frontline
row=$(awk -F'\t' -v f="$1" '$4==f' research/drafts/index.tsv)
tm=$(echo "$row" | cut -f5); a=$(echo "$row" | cut -f3); n=$(echo "$row" | cut -f2)
d=${2:-$(echo "$row" | cut -f6)}
e=$(printf '0x%x' $((0x$a + n)))
PYTHONPATH=scratch/game:research/tools ulimit -v 3000000 2>/dev/null
PYTHONPATH=scratch/game:research/tools timeout 600 python3 research/tools/sbs.py "$tm" "$d" "$1" 0x$a $e
