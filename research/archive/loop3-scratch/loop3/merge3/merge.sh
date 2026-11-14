#!/bin/sh
# merge.sh <main-unit> <tpl.json> <original-file> <file-index> <other-unit>... [-- port_unit extra args]
# copies scratch/loop3/merge3/src/<file>.cpp source (named by main unit's source path), removes the others, ports, verifies.
set -e
cd /home/kyle/workspace/moh-frontline
main=$1; tpl=$2; of=$3; idx=$4; shift 4
others=""; extra=""
while [ $# -gt 0 ]; do
  if [ "$1" = "--" ]; then shift; extra="$*"; break; fi
  others="$others $1"; shift
done
src=$(python3 -c "import json;print(json.load(open('config/GMFE69/$main.json'))['source'])")
cp scratch/loop3/merge3/src/$(basename $src) $src
for o in $others; do
  osrc=$(python3 -c "import json;print(json.load(open('config/GMFE69/$o.json'))['source'])")
  git rm -q $osrc config/GMFE69/$o.json
  python3 -c "
import json
p='config/GMFE69/project.json'; m=json.load(open(p)); m['units'].remove('$o.json'); open(p,'w').write(json.dumps(m,indent=2)+'\n')"
done
(ulimit -v 3000000; timeout 900 python3 tools/port_unit.py $main $src --like $tpl --original-file $of --file-index $idx --category reconstructed_game $extra) 2>&1 | tail -3
python3 -c "
import json
p='config/GMFE69/$main.json'; c=json.load(open(p)); c.pop('adapted_from',None); open(p,'w').write(json.dumps(c,indent=2)+'\n')
print([(s['name'],s['address'],s['size']) for s in c['sections']])"
(ulimit -v 3000000; timeout 900 python3 research/tools/one_unit.py $main) 2>&1 | tail -4
