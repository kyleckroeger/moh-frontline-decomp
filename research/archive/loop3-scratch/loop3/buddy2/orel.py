import sys
sys.path.insert(0,'tools')
from port_unit import relocations
from formats import Elf32
e=Elf32(open('orig/GMFE69/Moh2RelGC.elf','rb').read())
lo,hi=int(sys.argv[1],16),int(sys.argv[2],16)
for sec,where,kind,sym,add in sorted(relocations(e),key=lambda r:r[1]):
    if lo<=where<hi and kind in (4,6,109,5):
        print(hex(where),kind,sym['name'],hex(add))
