import sys
sys.path.insert(0,'.')
from pd2 import items
fs=['propdat.cpp','propdat_bsp.cpp','propdat_restart.cpp','propdat_core.cpp','propdat_load.cpp','propdat_endian_header.cpp']
A={f:items(f) for f in fs}
keys={}
for f in fs:
    for k,it,c in A[f]:
        keys.setdefault(k,{})
        keys[k].setdefault(c,[]).append(f)
for k,v in keys.items():
    if len(v)>1 and not k.startswith('d:'):
        print('CONFLICT',k,[vv for vv in v.values()])
