#!/bin/sh
# mrg.sh <new-id> <new-src> <tpl.json> <original-file> <file-index> <fragment-unit>... [-- port_unit extra args]
# Replaces every fragment unit (config + source) with one unit <new-id> built from
# scratch/loop3/merge10/src/<basename new-src>; new-id may equal one of the fragments.
# Env RMFILES: extra repo files to git rm (e.g. research/data/bss/X.cpp).
set -e
cd /home/kyle/workspace/moh-frontline
nid=$1; nsrc=$2; tpl=$3; of=$4; idx=$5; shift 5
frags=""; extra=""
while [ $# -gt 0 ]; do
  if [ "$1" = "--" ]; then shift; extra="$*"; break; fi
  frags="$frags $1"; shift
done
for o in $frags; do
  osrc=$(python3 -c "import json;print(json.load(open('config/GMFE69/$o.json'))['source'])")
  if [ "$osrc" != "$nsrc" ]; then git rm -q $osrc; fi
  if [ "$o" != "$nid" ]; then git rm -q config/GMFE69/$o.json; fi
done
python3 - $nid $frags <<'EOF'
import json,sys
nid=sys.argv[1]; frags=sys.argv[2:]
p='config/GMFE69/project.json'; m=json.load(open(p)); u=m['units']
pos=min(u.index(f+'.json') for f in frags)
for f in frags: u.remove(f+'.json')
u.insert(pos,nid+'.json')
open(p,'w').write(json.dumps(m,indent=2)+'\n')
EOF
cp scratch/loop3/merge10/src/$(basename $nsrc) $nsrc
for r in $RMFILES; do git rm -q $r; done
if [ -n "$EDITCMD" ]; then sh -c "$EDITCMD"; fi
(ulimit -v 3000000; timeout 900 python3 tools/port_unit.py $nid $nsrc --like $tpl --original-file $of --file-index $idx --category ${CATEGORY:-reconstructed_game} $extra) 2>&1 | tail -3
python3 -c "
import json
p='config/GMFE69/$nid.json'; c=json.load(open(p)); c.pop('adapted_from',None); open(p,'w').write(json.dumps(c,indent=2)+'\n')
print([(s['name'],s['address'],s['size']) for s in c['sections']])"
git add $nsrc config/GMFE69/$nid.json
(ulimit -v 3000000; timeout 900 python3 research/tools/one_unit.py $nid) 2>&1 | tail -8
