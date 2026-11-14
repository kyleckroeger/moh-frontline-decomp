#!/bin/sh
R=/home/kyle/workspace/moh-frontline; cd $R; M=scratch/loop3/merge8
sh research/tools/landlock.sh acquire merge8 || exit 1
if [ -n "$(git status --porcelain | grep -v '^ M research/plans/three-day-loop.md$')" ]; then echo DIRTY; git status --porcelain | head; sh research/tools/landlock.sh release merge8; exit 1; fi
git pull --ff-only -q
cp $M/fixtools/port_unit.py tools/port_unit.py
echo "- 2026-11-13 [loop merge8] tools/port_unit.py: 9bd2207e's data-section close-up also subtracted LEADING dropped records, moving 16 units' extab/extabindex anchors (Tank_object, alarm_list_ctor, bstimer, ...) up by their size on a fresh draft; only spans after the section's first placed object close up now (the address already counts the trimmed leading run). Sweep of 302 strip_unused units old vs fixed: only rcmp_mpc_codec differs (the intended correction); scratch/loop3/merge8/regress.txt." >> research/log/autopilot.md
(ulimit -v 6000000; timeout 3000 sh research/tools/land.sh "port_unit: close up only dropped records after a section's first placed object" "9bd2207e made data-section positions close up over dropped records, but a section's address is its compiled start plus the trimmed leading run (linked_offset), so leading dropped records must not be subtracted. With 9bd2207e a fresh draft of 16 units (Tank_object, alarm_list_ctor, bstimer, decal, the bsbifunc prompt units, objcreate_*, pumhandler_ctor, thrown_obj, ...) put extab/extabindex 8-0x30 bytes high. Now only dropped spans after the first placed object close up. Sweep of the 302 strip_unused units (pre-9bd2207e vs fixed): only rcmp_mpc_codec changes, to its committed (verified) addresses.") 2>&1 | tail -3
if [ "$(git log -1 --format=%s)" = "port_unit: close up only dropped records after a section's first placed object" ]; then git push 2>&1 | tail -1; else echo LAND FAILED; git checkout -q HEAD -- tools research/log; fi
sh research/tools/landlock.sh release merge8
