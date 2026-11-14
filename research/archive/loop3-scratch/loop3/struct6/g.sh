# g.sh file.cpp [-v]
cd /home/kyle/workspace/moh-frontline
F=GetClosestWaypointByType__15CAIFilterGlobalCFPC12_PropBSPLeafRC21CAIFilterRealPosition19DWI_OBJECT_CRC_ENUMbf
D=scratch/loop3/struct6/gcw
(ulimit -v 3000000; timeout 300 python3 scratch/game/fast.py $D/t.json $D/$1 $F 2>&1 | tail -1; timeout 300 python3 scratch/loop3/scripting/tools/sdiff.py $D/t.json $D/$1 $F 8005c6b8 784 $2) | tr '\n' ' '; echo
