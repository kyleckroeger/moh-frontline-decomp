#!/bin/sh
# s.sh <func-substr> [draft]  : side-by-side (sbs) for draft (default index draft); template gets include dirs
cd /home/kyle/workspace/moh-frontline
row=$(grep -F "$1" research/drafts/index.tsv | head -1)
tm=$(echo "$row" | cut -f5); a=$(echo "$row" | cut -f3); n=$(echo "$row" | cut -f2); f=$(echo "$row" | cut -f4); d=${2:-$(echo "$row" | cut -f6)}
e=$(printf '0x%x' $((0x$a + n)))
t=$(mktemp /tmp/claude-1000/-home-kyle-workspace-moh-frontline/a5d18f30-70be-41af-9a00-608205f1aeba/scratchpad/tmXXXXXX.json)
python3 -c "import json,sys;t=json.load(open('$tm'));t['include_dirs']=t.get('include_dirs',[])+['src/game','src/ea','include'];json.dump(t,open('$t','w'))"
(ulimit -v 3000000; cd research/tools && timeout 600 python3 sbs.py $t ../../$d "$f" 0x$a $e) 2>&1 | grep -v "^#  *[0-9]*: \|^### \|^# ---\|^#    File"
rm -f $t
