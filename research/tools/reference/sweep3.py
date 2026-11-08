"""sweep3.py <max> : RetV routing over rank.tsv with the canonical scorer and a fresh baseline. C drafts via SUFFIX=c run (argv[2]='c'). Discovery only."""
import csv,re,sys,os,hashlib
sys.path.insert(0,'scratch/solo/ret')
MAX=int(sys.argv[1]); C=len(sys.argv)>2 and sys.argv[2]=='c'
if C: os.environ['SUFFIX']='c'
import score
from concurrent.futures import ThreadPoolExecutor as TP
if C:
    rows=[(x[2],x[3],x[4],x[5],x[6]) for x in csv.reader(open('scratch/solo/sweep/table3.tsv'),delimiter='\t') if x and x[6].endswith('.c')]
else:
    rows=[(x[3],x[2],x[4],x[5],x[6]) for x in csv.reader(open('scratch/solo/ret/rank.tsv'),delimiter='\t') if x and int(x[0])<=MAX]
def names(m):
    c=re.match(r'__ct__\d*([A-Za-z_]\w*?)F',m)
    if m.startswith('__ct'):
        k=re.search(r'__ct.*?__(?:Q\d)?(?:\d+)([A-Za-z_]\w*)',m); return k.group(1) if k else None
    if m.startswith('__dt') or m.startswith('__'): return None
    return re.split(r'__(?=[0-9FQ])',m)[0].split('<')[0]
TY=r'(?:const )?(?:unsigned )?(?:int|short|float|char|long|bool|u8|u16|u32|s8|s16|s32|f32|[A-Z]\w*)(?: *\*+)?(?: *const)?'
DECL=re.compile(r'^(\s*)('+TY+r') +(\w+) = ([^;{}]+);\s*$')
ASG=re.compile(r'^(\s*)(\w+) = ([^;{}=]+);\s*$')
def body(s,n):
    for m in re.finditer(r'^[^\n;{}#/]*\b'+re.escape(n)+r'\s*\([^;{}]*\)\s*(?:const\s*)?(?::[^;{}]*)?\{',s,re.M):
        i=m.end();d=1
        while i<len(s) and d: d+=(s[i]=='{')-(s[i]=='}'); i+=1
        return m.start(),m.end(),i
    return None
T='template <class RT> static inline RT RetV(RT v) { return v; }\n'
def variants(s,n):
    b=body(s,n)
    if not b: return
    st,bs,en=b; L=s[bs:en].split('\n'); seen=set(); types={}
    for i,l in enumerate(L):
        m=DECL.match(l)
        if m: types[m.group(3)]=m.group(2)
    for i,l in enumerate(L):
        m=DECL.match(l); nl=None; ty=None
        if m and m.group(3) not in seen:
            seen.add(m.group(3)); ty=m.group(2); nl=f'{m.group(1)}{m.group(2)} {m.group(3)} = RetV({m.group(4)});'
        else:
            m=ASG.match(l)
            if m and m.group(2) not in seen:
                seen.add(m.group(2)); ty=types.get(m.group(2)); nl=f'{m.group(1)}{m.group(2)} = RetV({m.group(3)});'
        if not nl: continue
        if C:
            if not ty: continue
            h=f'static inline {ty.replace("const ","")} RetV({ty.replace("const ","")} v) {{ return v; }}\n'
        else: h=T
        yield l.strip(),s[:st]+h+s[st:bs]+'\n'.join(L[:i]+[nl]+L[i+1:])+s[en:]
def one(r):
    a,sz,f,t,d=r
    try:
        n=names(f)
        if not n or not os.path.isfile(d) or not os.path.isfile(t): return f'SKIP {f[:50]} name={n}'
        s=open(d,errors='replace').read(); b0=score.real(t,s,f,a,sz)
        if not b0: return f'NOBASE {f[:50]} {d}'
        best=None;cnt=0
        for line,src in variants(s,n):
            cnt+=1
            try:r2=score.real(t,src,f,a,sz)
            except Exception: r2=None
            if r2 and (best is None or r2[0]<best[0]): best=(r2[0],line,src)
        if not cnt: return f'NOVARS {f[:50]} base={b0[0]}'
        if best and best[0]<b0[0]:
            p=f'scratch/solo/ret/o3/{n[:28]}.{hashlib.md5(best[2].encode()).hexdigest()[:8]}.hit'; open(p,'w').write(best[2])
            return f'GAIN {b0[0]}->{best[0]} {sz} {f[:44]} vars={cnt} | {best[1][:70]} | {p}'
        return f'same base={b0[0]} vars={cnt} {f[:44]}'
    except Exception as e: return f'ERR {f[:50]} {e}'
with TP(4) as ex:
    for x in ex.map(one,rows): print(x,flush=True)
print('done')
