#!/bin/sh
R=/home/kyle/workspace/moh-frontline; cd $R; T=scratch/loop3/merge9
sh research/tools/landlock.sh acquire merge9 || exit 1
if [ -n "$(git status --porcelain | grep -v '^ M research/plans/three-day-loop.md$')" ]; then echo DIRTY; git status --porcelain|head; sh research/tools/landlock.sh release merge9; exit 1; fi
git pull --ff-only -q
cp $T/incA/game/DwiContainers.h include/game/DwiContainers.h
cp $T/try/scene_dtor/unit.json config/GMFE69/scene_dtor.json
(ulimit -v 3000000; timeout 900 python3 research/tools/one_unit.py scene_dtor) 2>&1 | tail -4
(ulimit -v 3000000; timeout 900 python3 research/tools/one_unit.py alarm) 2>&1 | tail -2
echo "- 2026-11-13 [loop merge9] include/game/DwiContainers.h: dwi::list::~list through clear() (dtor1's rule: an expanded inline numbers its locals in reverse declaration order, so the weak ~list and its copies inlined into reserve colour alike only when both expand clear()); all 115 units on the Dwi/CScene headers verify unchanged (scratch/loop3/merge9/hv.py), scene_dtor's stripped_objects relabelled @177/@178 -> @179/@180 (matched by relocation target)." >> research/log/autopilot.md
(ulimit -v 6000000; timeout 3000 sh research/tools/land.sh "DwiContainers.h: ~list through clear()" "dwi::list::~list now calls the inline clear(), as alarm.cpp's list does: an expanded inline numbers its locals in reverse declaration order, so the weak out-of-line ~list and its copies inlined into reserve colour node/next alike only when both expand clear(). Code-neutral for all 115 units that include game/Dwi*.h or game/CScene.h (each verified with the new header); the compiler's @N data labels shift by two, so scene_dtor's stripped_objects @177/@178 become @179/@180 (matched by section, size and relocation targets). Complete image identical.") 2>&1 | tail -3
if [ "$(git log -1 --format=%s)" = "DwiContainers.h: ~list through clear()" ]; then git push 2>&1 | tail -1; else echo LAND FAILED; git status --porcelain|head; git checkout -q HEAD -- include config research; fi
sh research/tools/landlock.sh release merge9
