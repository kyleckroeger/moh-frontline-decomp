import re,sys,json,glob
sys.path.insert(0,'research/tools')
from try_header import block_end
def ns_blocks(t):
    out=[]
    for m in re.finditer(r'^namespace\s+dwi\s*\{',t,re.M):
        e=block_end(t,m.end()-1); out.append((m.start(),e))
    return out
def templates(block):
    res={}
    for m in re.finditer(r'^(template\s*<[^>]*>\s*)?(class|struct)\s+(\w+)\b[^;{]*\{',block,re.M):
        e=block_end(block,m.end()-1); body=block[m.end():e-1]
        fields=re.findall(r'^\s*((?:const\s+)?[\w:<>]+\s*\**)\s*(\w+)(?:\[[^;]*)?;',body,re.M)
        meth=re.findall(r'^\s*(?:static\s+|virtual\s+|inline\s+)*[\w:<>&\*\s]*?(~?\w+|operator\S+?)\s*\(',body,re.M)
        res[m[3]]=( [f"{t.strip()} {n}" for t,n in fields if t.strip() not in('return','typedef')], meth)
    return res
units=json.load(open('config/GMFE69/project.json'))['units']
src2u={}
for n in units:
    u=json.load(open('config/GMFE69/'+n)); src2u.setdefault(u['source'],[]).append(u['id'])
for src in sorted(src2u):
    try: t=open(src,errors='replace').read()
    except: continue
    bl=ns_blocks(t)
    if not bl: continue
    for a,b in bl:
        for name,(f,m) in templates(t[a:b]).items():
            if name in sys.argv[1:] or not sys.argv[1:]:
                print(src2u[src][0], name, '|', ', '.join(f), '|', ' '.join(sorted(set(m))))
