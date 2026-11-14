#!/bin/sh
cd /home/kyle/workspace/moh-frontline; M=scratch/loop3/merge2
sleep 60
sh $M/landone.sh stretch $M/tpl_stretch.json stretch.c 13202 "stretch_head" "- 2026-11-13 [loop merge2] merged stretch.c (stretch_head + stretch, 3,036 B): crossfade/determineaction/applyaction static again; '#pragma auto_inline off' around them replaces stretch_head's -inline on; crossfade sets the weight before the step so 1.0f leads the .sdata2 pool (the fragment's pooled constants hid the order)." "Merge stretch.c: 2 fragments into one unit" "The time-stretch filter's helpers and functions in one unit; the helpers are file-static again, kept out of line with auto_inline off. Complete image identical."
sleep 60
sh $M/sn.sh
echo BATCH2 DONE
