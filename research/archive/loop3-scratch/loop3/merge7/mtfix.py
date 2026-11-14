import sys; sys.path.insert(0,'tools')
from pathlib import Path
import importlib.util as iu; sp=iu.spec_from_file_location("pu","scratch/loop3/merge7/port_unit_fix.py"); pu=iu.module_from_spec(sp); sys.modules["pu"]=pu; sp.loader.exec_module(pu)
from formats import Elf32
from audit import load_target
import traceback
orig_anchor=pu.anchor
def dbg(addresses,name,value):
    if name in('extab','extabindex') and name not in addresses:
        print('ANCHOR',name,hex(value)); traceback.print_stack(limit=3)
    return orig_anchor(addresses,name,value)
pu.anchor=dbg
obj=Elf32(Path(sys.argv[1]).read_bytes())
_,elf=load_target(); orig=pu.Original(elf)
fi=orig.file_index(sys.argv[2], int(sys.argv[3]))
dups=set(open(sys.argv[4]).read().split())
a,e,l,d,p=pu.match(obj,orig,fi,dups)
print({k:hex(v) for k,v in a.items()})
