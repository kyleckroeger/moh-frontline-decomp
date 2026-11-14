#!/bin/sh
# s.sh src [-v] : PSS score
cd /home/kyle/workspace/moh-frontline
F=PlayScriptedSound__FP10ISceneNode8CVector3RCQ211CSoundTable11SSoundParmsiP8BSObject
(ulimit -v 3000000; timeout 300 python3 scratch/game/fast.py scratch/loop3/struct3/t_pss_sound.json scratch/loop3/struct3/$1 $F; timeout 300 python3 scratch/loop3/scripting/tools/sdiff.py scratch/loop3/struct3/t_pss_sound.json scratch/loop3/struct3/$1 $F 800e9ba8 1336 $2)
