# Rebuilds CProfileInitClass's table from the original's linked instructions.
import sys,re,subprocess
sys.path.insert(0,'research/tools')
import fast
o=subprocess.run([fast.OBJDUMP,'-d','--start-address=0x800f4754','--stop-address=0x800f4a98',fast.ELF],capture_output=True,text=True).stdout
nm=subprocess.run(['build/tools/powerpc-eabi-nm',fast.ELF],capture_output=True,text=True).stdout
sy={l.split()[-1]:int(l.split()[0],16) for l in nm.splitlines() if len(l.split())==3}
reg={'r2':sy['_SDA2_BASE_'],'r13':sy['_SDA_BASE_']};names={};parents={}
def s16(x): return x-0x10000 if x>=0x8000 else x
data=open(fast.ELF,'rb').read()
import struct
d=open(fast.ELF,'rb').read()
phoff=struct.unpack('>I',d[28:32])[0];phnum=struct.unpack('>H',d[44:46])[0]
ph=[struct.unpack('>8I',d[phoff+i*32:phoff+i*32+32]) for i in range(phnum)]
def rd(a):
    for p in ph:
        if p[2]<=a<p[2]+p[4]:
            o=p[1]+a-p[2]; return d[o:o+40].split(b'\0')[0].decode()
    return '?'
order=[]
for l in o.splitlines():
    m=re.match(r'^\s*[0-9a-f]+:\t[0-9a-f ]+\t(\S+)\s*(.*)$',l)
    if not m: continue
    op,a=m[1],[x.strip() for x in m[2].split(',')]
    if op=='lis': reg[a[0]]=int(a[1],0)<<16
    elif op=='li': reg[a[0]]=int(a[1],0)
    elif op=='addi' and a[1] in reg: reg[a[0]]=(reg[a[1]]+int(a[2],0))&0xffffffff
    elif op=='stw':
        mm=re.match(r'(-?\d+)\((r\d+)\)',a[1])
        if mm and mm[2] in reg and reg[mm[2]] is not None and reg.get(a[0]) is not None:
            ad=(reg[mm[2]]+int(mm[1]))&0xffffffff; order.append((ad,reg[a[0]]))
base=0x802daca0
seq=[];ents={}
for ad,v in order:
    k=(ad-base)//24; f=(ad-base)%24
    if k>=0 and f==4: ents.setdefault(k,{})['name']=rd(v); seq.append(k)
    if k>=0 and f==0: ents.setdefault(k,{})['parent']=v-(1<<32) if v>=1<<31 else v
out=[]
for k in seq: out.append('    g_ProfileBins[%d].Set("%s", %d);'%(k,ents[k]['name'],ents[k]['parent']))
print('\n'.join(out))
