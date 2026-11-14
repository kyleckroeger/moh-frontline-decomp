"""wcmp.py UNIT APPEND OURSYM TARGET: compile src/<UNIT> (+APPEND code forcing an out-of-line copy),
compare OURSYM's instructions with the original's TARGET (weak or any nm symbol). Prints SAME/DIFF + diff."""
import sys,json,subprocess,tempfile,difflib,os,re
R='/home/kyle/workspace/moh-frontline/'
sys.path.insert(0,R+'research/tools'); import fast
unit,app,ours,tgt=sys.argv[1:5]
base=os.path.splitext(os.path.basename(unit))[0]
t=json.load(open(R+f'config/GMFE69/{base}.json'))
if unit.endswith('.c'): os.environ['SUFFIX']='c'
src=open(R+'src/'+unit).read()+'\n'+app.replace('\\n','\n')+'\n'
sym=None
for l in open(R+'scratch/loop3/names2/fl_nm.txt'):
    p=l.split()
    if len(p)==3 and p[2]==tgt: sym=int(p[0],16)
size=None
out=subprocess.run(['nm','-S',R+'orig/GMFE69/Moh2RelGC.elf'],capture_output=True,text=True).stdout
for l in out.splitlines():
    p=l.split()
    if len(p)==4 and p[3]==tgt: sym=int(p[0],16); size=int(p[1],16)
a=fast.target(tgt,sym,sym+size)
with tempfile.TemporaryDirectory() as w:
    obj,log=fast.compile_object(t,src,w)
    if not obj: print('COMPILE FAIL',log[-600:]); sys.exit(1)
    mine=fast.parse(subprocess.run([fast.OBJDUMP,'-dr',obj],capture_output=True,text=True).stdout)
b=mine.get(ours)
if b is None: print('NOSYM; have:',[k for k in mine if any(s in k for s in ours.split('__')[:1])][:20]); sys.exit(1)
d=[l for l in difflib.unified_diff(a,b,n=0,lineterm='') if l[:1] in '+-' and not l.startswith(('+++','---'))]
print('SAME' if not d else 'DIFF %d'%len(d), tgt, len(a), ours, len(b))
for l in d[:20]: print('  ',l)
