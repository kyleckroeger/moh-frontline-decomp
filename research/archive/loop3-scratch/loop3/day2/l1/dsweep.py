# dsweep.py json func draft [suffix]: single-move sweep of the leading local declarations of func's body
import sys,re,json,tempfile,subprocess,difflib,os
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/scratch/game'); import fast
t=json.load(open(sys.argv[1])); f=sys.argv[2]; src=open(sys.argv[3],errors='replace').read()
if len(sys.argv)>4: os.environ['SUFFIX']=sys.argv[4]
t['include_dirs']=t.get('include_dirs',[])+['src/game','src/ea','include']
name=re.sub(r'^(__ct|__dt)__','',f); name=name.split('__')[0] if not f.startswith(('__ct','__dt')) else name
name=re.sub(r'<.*','',name)
a=fast.target(f); w=tempfile.mkdtemp()
def score(s):
    obj,log=fast.compile_object(t,s,w)
    if not obj: return None
    m=fast.parse(subprocess.run([fast.OBJDUMP,'-dr',obj],capture_output=True,text=True).stdout).get(f)
    if m is None: return None
    sm=difflib.SequenceMatcher(None,a,m,autojunk=False)
    return sum((i2-i1)+(j2-j1) for op,i1,i2,j1,j2 in sm.get_opcodes() if op!='equal')
base=score(src); print('base',base,flush=True)
lines=src.split('\n')
decl=re.compile(r'^\s+(const\s+)?(unsigned\s+|signed\s+)?[A-Za-z_][\w:<>,\s]*?[\s\*&]+(const\s+)?\*?\s*[A-Za-z_]\w*(\[\d*\])?(\s*=\s*[-\w\.\s\*&\(\)]+)?\s*;\s*(//.*)?$')
best=(base,None)
for i,l in enumerate(lines):
    if re.search(r'\b'+re.escape(name)+r'\s*\(',l) and not l.rstrip().endswith(';') and not l.startswith((' ','\t')):
        j=i
        while '{' not in lines[j]: j+=1
        k=j+1; blk=[]
        while k<len(lines) and decl.match(lines[k]) and not re.match(r'^\s+(return|delete|goto)\b',lines[k]): blk.append(k); k+=1
        if len(blk)<2: continue
        print('func at',i+1,'decls',len(blk),flush=True)
        B=[lines[x] for x in blk]
        for v in range(len(B)):
            for p in range(len(B)):
                if p==v: continue
                r=B[:v]+B[v+1:]; nb=r[:p]+[B[v]]+r[p:]
                s='\n'.join(lines[:blk[0]]+nb+lines[blk[-1]+1:])
                n=score(s)
                if n is not None and n<best[0]:
                    best=(n,s); print('improved',n,'move',B[v].strip(),'->',p,flush=True)
if best[1] and len(sys.argv)>0: open('/home/kyle/workspace/moh-frontline/scratch/loop3/day2/l1/out_'+os.path.basename(sys.argv[3])+'.dsw','w').write(best[1])
print('best',best[0])
