#!/bin/sh
# s.sh <funcsubstr> [src] [sbs|score]: side-by-side or score of src (default: the indexed draft)
cd /home/kyle/workspace/moh-frontline
row=$(awk -F'\t' -v k="$1" 'index($4,k){print; exit}' research/drafts/index.tsv)
fn=$(echo "$row"|cut -f4); a=$(echo "$row"|cut -f3); n=$(echo "$row"|cut -f2); t0=$(echo "$row"|cut -f5); t=/tmp/claude-1000/g3tmpl_$$.json; python3 -c "import json;d=json.load(open('$t0'));d['include_dirs']=d.get('include_dirs',[])+['src/game','src/ea','include'];json.dump(d,open('$t','w'))"; d=$(echo "$row"|cut -f6)
src=${2:-$d}
e=$(printf '0x%x' $((0x$a + n)))
if [ "$3" = score ]; then
 python3 -c "
import sys; sys.path.insert(0,'research/tools'); import score
print(score.real('$t',open('$src',errors='replace').read(),'$fn','$a','$n'))"
else
 python3 research/tools/sbs.py $t $src "$fn" 0x$a $e
fi
