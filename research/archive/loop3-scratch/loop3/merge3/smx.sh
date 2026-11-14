#!/bin/sh
# move MIX_getwetbuffer from smixfram into smixfx; lock, port both, verify both, land, push, release.
R=/home/kyle/workspace/moh-frontline; cd $R; M=scratch/loop3/merge3
sh research/tools/landlock.sh acquire merge3 || exit 1
if [ -n "$(git status --porcelain)" ]; then echo DIRTY; git status --porcelain | head; sh research/tools/landlock.sh release merge3; exit 1; fi
git pull --ff-only -q
cp $M/src/smixfx.c src/ea/smixfx.c; cp $M/src/smixfram.c src/ea/smixfram.c
(ulimit -v 3000000; timeout 900 python3 tools/port_unit.py smixfx src/ea/smixfx.c --like $M/tpl_smixfx.json --original-file smixfx.c --file-index 13011 --category reconstructed_game) 2>&1 | tail -2
(ulimit -v 3000000; timeout 900 python3 tools/port_unit.py smixfram src/ea/smixfram.c --like $M/tpl_smixfram.json --original-file smixfram.c --file-index 13022 --category reconstructed_game) 2>&1 | tail -2
for u in smixfx smixfram; do python3 -c "
import json
p='config/GMFE69/$u.json'; c=json.load(open(p)); c.pop('adapted_from',None); open(p,'w').write(json.dumps(c,indent=2)+'\n')
print([(s['name'],s['address'],s['size']) for s in c['sections']])"; done
a=$( (ulimit -v 3000000; timeout 900 python3 research/tools/one_unit.py smixfx) 2>&1 ); echo "$a" | tail -2
b=$( (ulimit -v 3000000; timeout 900 python3 research/tools/one_unit.py smixfram) 2>&1 ); echo "$b" | tail -2
subj="smixfx.c: MIX_getwetbuffer moves from smixfram into smixfx"
if echo "$a" | grep -q "^smixfx: VERIFIED" && echo "$b" | grep -q "^smixfram: VERIFIED"; then
  echo "- 2026-11-13 [loop merge3] smixfx.c's last function MIX_getwetbuffer (12 B, 0x801611d0) moved from the smixfram unit into smixfx: smixfx.c's .text record is 0x51c bytes; smixfram now starts at MIX_getframe." >> research/log/autopilot.md
  (ulimit -v 6000000; timeout 3000 sh research/tools/land.sh "$subj" "MIX_getwetbuffer is the last function of smixfx.c's record (its .text is 0x51c bytes), so the smixfx unit now holds all of smixfx.c and smixfram starts at MIX_getframe. Both units verified; complete image identical.") 2>&1 | tail -2
fi
if [ "$(git log -1 --format=%s)" = "$subj" ]; then git push 2>&1 | tail -1; else echo "FAILED; reverting"; git reset -q; git checkout -q HEAD -- config src research docs progress README.md; fi
sh research/tools/landlock.sh release merge3
