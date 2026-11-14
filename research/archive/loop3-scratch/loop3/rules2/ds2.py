# ds2.py json func draft L1 L2 [suffix]: move each line in [L1,L2] (1-based, a block of declarations) to every slot in the block; prints improvements
import sys,json,tempfile,subprocess,difflib,os
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/scratch/game'); import fast
t=json.load(open(sys.argv[1])); f=sys.argv[2]; src=open(sys.argv[3],errors='replace').read()
L1=int(sys.argv[4])-1; L2=int(sys.argv[5])
if len(sys.argv)>6: os.environ['SUFFIX']=sys.argv[6]
t['include_dirs']=t.get('include_dirs',[])+['src/game','src/ea','include']
a=fast.target(f); w=tempfile.mkdtemp()
def score(s):
    obj,log=fast.compile_object(t,s,w)
    if not obj: return None
    m=fast.parse(subprocess.run([fast.OBJDUMP,'-dr',obj],capture_output=True,text=True).stdout).get(f)
    if m is None: return None
    sm=difflib.SequenceMatcher(None,a,m,autojunk=False)
    return sum((i2-i1)+(j2-j1) for op,i1,i2,j1,j2 in sm.get_opcodes() if op!='equal')
base=score(src); print('base',base,flush=True)
lines=src.split('\n'); B=lines[L1:L2]; best=(base,None)
for v in range(len(B)):
    for p in range(len(B)):
        if p==v: continue
        C=B[:v]+B[v+1:]; C.insert(p,B[v])
        s='\n'.join(lines[:L1]+C+lines[L2:]); r=score(s)
        if r is not None and r<base: print(r,'move',B[v].strip(),'->',p,flush=True)
        if r is not None and r<best[0]:
            best=(r,(v,p)); open(sys.argv[3]+'.ds2best','w').write(s)
print('best',best)
