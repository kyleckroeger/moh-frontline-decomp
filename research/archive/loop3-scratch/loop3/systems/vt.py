import sys,subprocess,re
sys.path.insert(0,'tools')
from audit import load_target
_,t=load_target()
syms={}
for s in t.symbols():
    if s['type']==2: syms.setdefault(s['address'],s['name'])
v=next(s for s in t.symbols() if s['name']==sys.argv[1])
a0=v['address']
out=subprocess.run(['build/tools/powerpc-eabi-objdump','-s','--start-address=%#x'%a0,'--stop-address=%#x'%(a0+v['size']),'orig/GMFE69/Moh2RelGC.elf'],capture_output=True,text=True).stdout
for l in out.splitlines():
    m=re.match(r'^ ([0-9a-f]{8}) ((?:[0-9a-f]{8} ?){1,4})',l)
    if m:
        a=int(m.group(1),16)
        for i,w in enumerate(m.group(2).split()):
            w=int(w,16); print(hex(a+4*i-a0), hex(w), syms.get(w,''))
