# regs.sh file : first 30 instrs of ours vs orig (gcw)
cd /home/kyle/workspace/moh-frontline/scratch/loop3
python3 struct4/tools/asm.py struct6/gcw/t.json struct6/gcw/$1 GetClosestWaypointByType__15CAIFilterGlobalCFPC12_PropBSPLeafRC21CAIFilterRealPosition19DWI_OBJECT_CRC_ENUMbf 0 ${2:-30} | cut -d' ' -f2- | tr '\n' ';'; echo
