"""extab.py <func addr hex>: print extabindex entry and the extab record words + symbols (nm)."""
import sys,subprocess,struct
E='/home/kyle/workspace/moh-frontline/orig/GMFE69/Moh2RelGC.elf'
NM='/home/kyle/workspace/moh-frontline/build/tools/powerpc-eabi-nm'
b=open(E,'rb').read()
IA,IO,IS=0x8000ffe0,0x90c0,0x6e84; XA,XO,XS=0x80009600,0x26e0,0x69e0
syms={}
for l in subprocess.run([NM,E],capture_output=True,text=True).stdout.splitlines():
    p=l.split()
    if len(p)==3: syms.setdefault(int(p[0],16),p[2])
a=int(sys.argv[1],16)
ents=[struct.unpack('>III',b[IO+i:IO+i+12]) for i in range(0,IS,12)]
starts=sorted(e[2] for e in ents)
for fa,sz,ea in ents:
    if fa==a:
        print('index',hex(fa),hex(sz),hex(ea))
        nxt=[s for s in starts if s>ea]; end=nxt[0] if nxt else XA+XS
        for k in range(ea,end,4):
            w=struct.unpack('>I',b[XO+k-XA:XO+k-XA+4])[0]
            print(hex(k),'%08x'%w,syms.get(w,''))
