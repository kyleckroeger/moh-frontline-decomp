# v.py: build variants of cd_o4.cpp header/box block and score
import sys,subprocess,itertools,re
base=open('cd_o4.cpp').read()
H_OLD=base[base.index('    int const c_param = param;'):base.index('    BSGO_Destructable* script = new')]
B_OLD="""    void* mem = g_pVolumeAllocator.GetFreeElement(true);
    box = (CVolBox*)mem;
    if (box) new (mem) CVolBox;
"""
def score(src,tag):
    open(f'w_{tag}.cpp','w').write(src)
    out=subprocess.run(['python3','rm.py',f'w_{tag}.cpp'],capture_output=True,text=True).stdout.strip()
    return out
