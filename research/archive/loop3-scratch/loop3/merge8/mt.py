"""mt.py <obj> <origfile> <idx> <weakfile>: run port_unit.match read-only, print section anchors."""
import sys; sys.path.insert(0,'tools')
from pathlib import Path
import importlib.util as iu; sp=iu.spec_from_file_location("pu","tools/port_unit.py"); pu=iu.module_from_spec(sp); sys.modules["pu"]=pu; sp.loader.exec_module(pu)
from formats import Elf32
from audit import load_target
obj=Elf32(Path(sys.argv[1]).read_bytes())
_,elf=load_target(); orig=pu.Original(elf)
fi=orig.file_index(sys.argv[2], int(sys.argv[3]))
dups=set(open(sys.argv[4]).read().split())
a,e,l,d,p=pu.match(obj,orig,fi,dups)
print({k:hex(v) for k,v in a.items()})
