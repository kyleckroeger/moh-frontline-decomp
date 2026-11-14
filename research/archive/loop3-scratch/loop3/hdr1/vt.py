import struct,subprocess,sys
f=open('orig/GMFE69/Moh2RelGC.elf','rb').read()
e_phoff=struct.unpack('>I',f[0x1c:0x20])[0]; ph=struct.unpack('>H',f[0x2c:0x2e])[0]
segs=[]
for i in range(ph):
    o=e_phoff+i*32; t,off,va,pa,fs,ms=struct.unpack('>6I',f[o:o+24]); segs.append((va,off,fs))
def rd(a):
    for va,off,fs in segs:
        if va<=a<va+fs: return struct.unpack('>I',f[off+a-va:off+a-va+4])[0]
syms={};byname={}
for l in subprocess.run(['nm','-S','orig/GMFE69/Moh2RelGC.elf'],capture_output=True,text=True).stdout.splitlines():
    p=l.split()
    if len(p)>=4 and len(p[2])==1 and len(p[1])==8: syms.setdefault(int(p[0],16),p[3]); byname[p[3]]=(int(p[0],16),int(p[1],16))
for n in sys.argv[1:]:
    a,sz=byname[n]; print('==',n)
    for i in range(sz//4):
        v=rd(a+4*i); print(' ',hex(i*4), hex(v), syms.get(v,''))
def rtti(a,depth=0):
    nm_=rd(a); bases=rd(a+4)
    # name string
    for va,off,fs in segs:
        if va<=nm_<va+fs: s=f[off+nm_-va:off+nm_-va+60].split(b'\0')[0]
    print('  '*depth, hex(a), s, hex(bases or 0))
    if bases:
        i=0
        while True:
            b=rd(bases+8*i)
            if not b: break
            print('  '*depth,' base', hex(b), syms.get(b,''), 'off', rd(bases+8*i+4)); i+=1
            if i>6: break
import os
if os.environ.get('RTTI'): rtti(int(os.environ['RTTI'],16))
