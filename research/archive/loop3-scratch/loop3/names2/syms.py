import re
def parse_class(s,i):
    # returns (qualified class, next index) for CW mangling
    if s[i]=='Q':
        n=int(s[i+1]); i+=2; parts=[]
        for _ in range(n):
            m=re.match(r'\d+',s[i:]); L=int(m.group()); i+=len(m.group()); parts.append(s[i:i+L]); i+=L
        return '::'.join(parts),i
    m=re.match(r'\d+',s[i:])
    if not m: return None,i
    L=int(m.group()); i+=len(m.group()); return s[i:i+L],i+L
def demangle(sym):
    # returns (cls or '', name, rest)
    # find '__' that splits name from class/F, skipping leading __ (ct/dt)
    start=2 if sym.startswith('__') else 0
    # handle templates name<...>
    depth=0;i=start
    while i<len(sym)-1:
        c=sym[i]
        if c=='<': depth+=1
        elif c=='>': depth-=1
        elif depth==0 and sym.startswith('__',i) and i>start:
            name=sym[:i]; j=i+2
            if j+1<len(sym) and sym[j]=='C' and (sym[j+1].isdigit() or sym[j+1]=='Q'): j+=1
            if j<len(sym) and (sym[j].isdigit() or sym[j]=='Q'):
                try: cls,k=parse_class(sym,j)
                except Exception: return '',name,sym[j:]
                return cls or '',name,sym[k:]
            return '',name,sym[j:]
        i+=1
    return '',sym,''
def load(path):
    out=[]
    for l in open(path):
        p=l.split()
        if len(p)<3: continue
        addr,typ,sym=p[0],p[1],p[2]
        if sym.startswith('@') or sym.startswith('lbl_') or sym.startswith('.'): continue
        cls,name,rest=demangle(sym)
        out.append((addr,typ,sym,cls,name,rest))
    return out
