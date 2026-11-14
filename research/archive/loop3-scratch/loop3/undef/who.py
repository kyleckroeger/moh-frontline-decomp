import sys,struct
sys.path.insert(0,'tools')
from formats import Elf32
elf=Elf32(open(sys.argv[1],'rb').read()); syms=list(elf.symbols()); want=set(sys.argv[2:])
funcs=[s for s in syms if s['type']==2]
for sec in elf.sections:
    if sec['type']!=4: continue
    d=elf.contents(sec)
    for o in range(0,len(d),12):
        w,info=struct.unpack_from('>II',d,o); n=syms[info>>8]['name']
        if n in want:
            f=[s['name'] for s in funcs if s['section']==sec['info'] and s['address']<=w<s['address']+s['size']]
            print(n,f)
