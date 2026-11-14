"""scan.py: list inline functions (free 'inline' and in-class member bodies) in src/game, src/ea."""
import re,sys,glob,os
R='/home/kyle/workspace/moh-frontline/'
def strip(s):
    # blank comments and strings, keep length
    out=list(s); i=0;n=len(s)
    while i<n:
        if s.startswith('//',i):
            j=s.find('\n',i); j=n if j<0 else j
            for k in range(i,j): out[k]=' '
            i=j
        elif s.startswith('/*',i):
            j=s.find('*/',i+2); j=n if j<0 else j+2
            for k in range(i,j):
                if s[k]!='\n': out[k]=' '
            i=j
        elif s[i] in '"\'':
            q=s[i]; j=i+1
            while j<n and s[j]!=q:
                if s[j]=='\\': j+=1
                j+=1
            for k in range(i+1,min(j,n)): 
                if s[k]!='\n': out[k]=' '
            i=j+1
        elif s[i]=='#':
            # preprocessor line
            j=i
            while True:
                e=s.find('\n',j); e=n if e<0 else e
                if e>0 and s[e-1]=='\\': j=e+1; continue
                break
            for k in range(i,e): out[k]=' '
            i=e
        else: i+=1
    return ''.join(out)
def match(t,i):
    d=0
    for j in range(i,len(t)):
        if t[j]=='{': d+=1
        elif t[j]=='}':
            d-=1
            if d==0: return j
    return len(t)-1
hdr=re.compile(r'([~\w:<>,\*& \t\n]+?)\s*(\b[\w:~]+(?:<[^<>(){};]*>)?(?:\s*operator\s*[^\s(]+)?)\s*\(([^;{}]*)\)\s*(const)?\s*(?::[^;{}]*)?\{$',re.S)
rows=[]
def scan(path,unit):
    s=open(path,errors='replace').read(); t=strip(s)
    def walk(a,b,cls,depth):
        i=a
        while i<b:
            j=t.find('{',i,b)
            if j<0: return
            # header text = from previous ; or } or { to j
            k=max(t.rfind(';',a,j),t.rfind('}',a,j),t.rfind('{',a,j))+1
            # skip access specifiers
            h=t[k:j+1]
            h2=re.sub(r'^\s*(public|private|protected)\s*:','',h.strip())
            e=match(t,j)
            m=re.match(r'^\s*(?:template\s*<[^>]*>\s*)?(class|struct|union|namespace|extern\s+"C")\b\s*([\w:]*)',h2)
            if m or h2.startswith('extern'):
                kind=m.group(1) if m else 'extern'
                name=m.group(2) if m else ''
                if kind in ('class','struct','union') and name:
                    walk(j+1,e,name,depth+1)
                else:
                    walk(j+1,e,cls,depth+1)
                i=e+1; continue
            mm=hdr.search(h2)
            if mm and '=' not in h2.split('(')[0]:
                pre=mm.group(1); name=mm.group(2); 
                isinl=bool(re.search(r'\binline\b',pre)) or cls is not None
                if isinl and not re.search(r'\b(if|while|for|switch|return|else)\b',name):
                    body=' '.join(s[j:e+1].split())
                    q=name if (cls is None or '::' in name) else cls+'::'+name
                    line=s.count('\n',0,j)+1
                    rows.append((unit,line,q,' '.join(mm.group(3).split()),'const' if mm.group(4) else '',' '.join(pre.split()),body))
            i=e+1
    walk(0,len(t),None,0)
for d in ('game','ea'):
    for p in sorted(glob.glob(R+f'src/{d}/*.c*')):
        scan(p,d+'/'+os.path.basename(p))
with open(R+'scratch/loop3/names2/inlines.tsv','w') as f:
    f.write('unit\tline\tname\tparams\tconst\tprefix\tbody\n')
    for r in rows: f.write('\t'.join(str(x).replace('\t',' ') for x in r)+'\n')
print(len(rows))
