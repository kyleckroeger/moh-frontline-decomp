# tsweep.py json func draft [suffix]: single integer-type substitutions in declarations/prototypes; reports improvements
import sys,re,json,tempfile,subprocess,difflib,os
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/scratch/game'); import fast
t=json.load(open(sys.argv[1])); f=sys.argv[2]; src=open(sys.argv[3],errors='replace').read()
if len(sys.argv)>4: os.environ['SUFFIX']=sys.argv[4]
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
T=['unsigned int','unsigned long','unsigned short','unsigned char','signed char','int','long','short','char','bool','unsigned']
pat=re.compile(r'\b(unsigned int|unsigned long|unsigned short|unsigned char|signed char|unsigned|int|long|short|char|bool)\b')
lines=src.split('\n'); best=(base,None); n=0
name=re.sub(r'^(__ct|__dt)__','',f); name=name.split('__')[0] if not f.startswith(('__ct','__dt')) else name
name=re.sub(r'<.*','',name)
inbody=set()
for i,l in enumerate(lines):
    if re.search(r'\b'+re.escape(name)+r'\s*\(',l) and not l.rstrip().endswith(';') and not l.startswith((' ','\t')):
        j=i
        while j<len(lines) and not lines[j].startswith('}'): inbody.add(j); j+=1
for i,l in enumerate(lines):
    s=l.strip()
    if i not in inbody and not (l.lstrip().startswith(('inline','static inline')) or ('(' in l and s.endswith(';'))): continue
    if s.startswith(('//','/*','*','#','return','case')) or '"' in l: continue
    if not (s.endswith(';') or s.endswith('{') or s.endswith(')') or s.endswith(',')): continue
    for m in pat.finditer(l):
        for alt in T:
            if alt==m.group(1): continue
            nl=l[:m.start()]+alt+l[m.end():]
            ss='\n'.join(lines[:i]+[nl]+lines[i+1:]); r=score(ss); n+=1
            if r is not None and r<base:
                print('better',r,'line',i+1,repr(s[:70]),m.group(1),'->',alt,flush=True)
                if r<best[0]: best=(r,ss)
print('tried',n,'best',best[0])
if best[1]: open('/home/kyle/workspace/moh-frontline/scratch/loop3/day2/l1/out_'+os.path.basename(sys.argv[3])+'.tsw','w').write(best[1])
