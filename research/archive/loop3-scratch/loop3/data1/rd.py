import sys,struct
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/tools')
from formats import Elf32
elf=Elf32(open('/home/kyle/workspace/moh-frontline/orig/GMFE69/Moh2RelGC.elf','rb').read())
def rd(a,n):
    for s in elf.sections:
        if s['address']<=a<s['address']+s['size'] and s['type']!=8:
            return elf.contents(s)[a-s['address']:a-s['address']+n]
for x in sys.argv[1:]:
    a,n=x.split(':'); b=rd(int(a,16),int(n,0)); print(a,b.hex(),repr(b))
