"""sweep.py: for near-misses, route each simple local's initialiser through an identity value-returning inline; report gains. Discovery only."""
import csv,re,subprocess,sys,hashlib,os
from concurrent.futures import ThreadPoolExecutor as T
MAX=int(sys.argv[1]) if len(sys.argv)>1 else 30
MIN=int(sys.argv[2]) if len(sys.argv)>2 else 1
real=[x for x in csv.reader(open('scratch/solo/gb/real.tsv'),delimiter='\t') if x and x[0].isdigit() and MIN<=int(x[0])<=MAX]
tmpl={x[2]:x[5] for x in csv.reader(open('scratch/solo/sweep/table3.tsv'),delimiter='\t') if x}
def bare(m):
    m2=re.match(r'(?:__ct|__dt)',m)
    n=re.split(r'__(?=[0-9FQ])',m)[0]
    return n.split('<')[0]
DECL=re.compile(r'^(\s*)((?:const )?(?:unsigned )?(?:int|short|float|char|long|bool|[A-Z]\w*)(?: *\*+)?(?: *const)? *)(\w+) = ([^;{}]+);\s*$')
ASG=re.compile(r'^(\s*)(\w+) = ([^;{}=]+);\s*$')
H='template <class RT> static inline RT RetV(RT v) { return v; }\n'
def body(s,n):
    for m in re.finditer(r'^[^\n;{}#/]*\b'+re.escape(n)+r'\s*\([^;{}]*\)\s*(?:const\s*)?\{',s,re.M):
        i=m.end();d=1
        while i<len(s) and d: d+= (s[i]=='{')-(s[i]=='}'); i+=1
        return m.start(),m.end(),i
def score(t,p,f,a,e):
    try:o=subprocess.run(['python3','scratch/solo/pred/sbs.py',t,p,f,a,e],capture_output=True,text=True,timeout=100)
    except Exception: return None
    if not o.stdout: return None
    return sum(1 for l in o.stdout.splitlines() if l.endswith('<<') and not re.search(r',0 +<<$|0\(0\) +<<$',l))
jobs=[]
for x in real:
    n0,addr,size,f,d=int(x[0]),x[4],int(x[3]),x[5],x[6]
    t=tmpl.get(addr)
    if not t or not os.path.isfile(d) or d.endswith('.c'): continue
    s=open(d,errors='replace').read(); b=body(s,bare(f))
    if not b: continue
    st,bs,en=b; lines=s[bs:en].split('\n'); seen=set()
    for i,l in enumerate(lines):
        m=DECL.match(l); nl=None
        if m and m.group(3) not in seen: nl=f'{m.group(1)}{m.group(2)}{m.group(3)} = RetV({m.group(4)});'; seen.add(m.group(3))
        else:
            m=ASG.match(l)
            if m and m.group(2) not in seen: nl=f'{m.group(1)}{m.group(2)} = RetV({m.group(3)});'; seen.add(m.group(2))
        if nl and 'RetV' not in l:
            jobs.append((n0,f,d,t,addr,size,s[:st]+H+s[st:bs]+'\n'.join(lines[:i]+[nl]+lines[i+1:])+s[en:],l.strip()))
print(len(real),'fns',len(jobs),'variants',flush=True)
def run(j):
    n0,f,d,t,addr,size,src,l=j
    h=hashlib.md5(src.encode()).hexdigest()[:8]; p=f'scratch/solo/ret/o/{bare(f)[:24]}.{h}.cpp'; open(p,'w').write(src)
    r=score(t,p,f,'0x'+addr,hex(int(addr,16)+size))
    if r is None or r>=n0:
        try: os.remove(p)
        except OSError: pass
        return None
    return r,n0,size,f[:40],l[:60],p
with T(4) as ex:
    for r in ex.map(run,jobs):
        if r: print(r,flush=True)
print('done')
