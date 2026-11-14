"""vtchk.py <obj> <__vt__X>: compare a compiled vtable's slot relocations with the original's slots."""
import sys,subprocess,re
R='/home/kyle/workspace/moh-frontline/'
vt=sys.argv[2]
orig=[l.split()[2:] for l in subprocess.run(['python3',R+'scratch/loop3/merge6/vt.py',vt],capture_output=True,text=True).stdout.splitlines()]
t=subprocess.run([R+'build/tools/powerpc-eabi-objdump','-t',sys.argv[1]],capture_output=True,text=True).stdout
m=[l.split() for l in t.splitlines() if l.split()[-1:]==[vt]][0]
start=int(m[0],16); size=int(m[4],16)
r=subprocess.run([R+'build/tools/powerpc-eabi-objdump','-r','-j','.data',sys.argv[1]],capture_output=True,text=True).stdout
mine={}
for l in r.splitlines():
    p=l.split()
    if len(p)==3 and re.match('^[0-9a-f]{8}$',p[0]):
        o=int(p[0],16)
        if start<=o<start+size: mine[(o-start)//4]=p[2]
bad=0
for i,s in enumerate(orig):
    if i<2: continue
    a=mine.get(i,'-'); b=s[0] if s else '?'
    if a!=b: bad+=1; print('slot %x: compiled %s original %s'%(i*4,a,b))
print(vt,'slots',len(orig),'mismatches',bad)
