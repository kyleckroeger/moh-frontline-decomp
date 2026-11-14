import sys,difflib
sys.path.insert(0,'.')
from pd2 import items
fs=['propdat.cpp','propdat_bsp.cpp','propdat_restart.cpp','propdat_core.cpp','propdat_load.cpp','propdat_endian_header.cpp']
A={f:{k:c for k,it,c in items(f)} for f in fs}
for k,a,b in [('T:MOH_core_Struct','propdat_bsp.cpp','propdat.cpp'),('T:TriggerObject_struct','propdat_bsp.cpp','propdat.cpp'),('T:MGTriggerObjectView','propdat_bsp.cpp','propdat_load.cpp'),
              ('T:BPDLight','propdat_bsp.cpp','propdat_core.cpp'),('T:BPDLightVolume','propdat_bsp.cpp','propdat_core.cpp'),('D:EndianSwap(BPDLight&)','propdat_bsp.cpp','propdat_core.cpp'),
              ('D:EndianSwap(BPDLightVolume&)','propdat_bsp.cpp','propdat_core.cpp'),('D:EndianSwap(BPDHeader&)','propdat_bsp.cpp','propdat_endian_header.cpp')]:
    print('=====',k,a,'vs',b)
    for l in difflib.unified_diff(A[a][k].split('\n'),A[b][k].split('\n'),lineterm='',n=0): print(l)
