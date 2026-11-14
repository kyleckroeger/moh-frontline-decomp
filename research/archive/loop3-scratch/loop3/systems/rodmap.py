"""rodmap.py <blockaddr> <end> <textstart> <textend>: list .rodata objects in block with referencing functions and offsets"""
import sys,subprocess,re,collections
sys.path.insert(0,'tools')
from audit import load_target
_,t=load_target()
b,e,ts,te=[int(x,16) for x in sys.argv[1:5]]
syms=sorted(t.symbols(),key=lambda s:s['address'])
objs=[s for s in syms if b<=s['address']<e and s['size']]
funcs=[s for s in syms if s['type']==2 and ts<=s['address']<te]
out=subprocess.run(['build/tools/powerpc-eabi-objdump','-dr','--start-address=%#x'%ts,'--stop-address=%#x'%te,'orig/GMFE69/Moh2RelGC.elf'],capture_output=True,text=True).stdout
refs=collections.defaultdict(set); cur=None
for l in out.splitlines():
    m=re.match(r'^\s*([0-9a-f]+):\s+R_PPC\S+\s+(\S+)',l)
    if m:
        a=int(m.group(1),16); f=max((x for x in funcs if x['address']<=a),key=lambda x:x['address'],default=None)
        refs[m.group(2)].add(f['name'][:40] if f else '?')
for o in objs: print('+%d'%(o['address']-b),o['size'],o['name'][:40],sorted(refs.get(o['name'],[]))[:4])
print('base users',sorted(refs.get('...rodata.0',[])))
