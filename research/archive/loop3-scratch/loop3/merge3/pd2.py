import sys,re
sys.path.insert(0,'uis')
from body import split, code_of
def items(f):
    src=open('/home/kyle/workspace/moh-frontline/src/game/'+f).read()
    its,tail=split(src)
    out=[]
    for it in its:
        c=code_of(it)
        lines=c.split('\n')
        while lines and lines[0].startswith('#'): lines.pop(0)
        c='\n'.join(lines).strip()
        m=re.match(r'(?:typedef\s+)?(struct|class|union|enum)\s+(\w+)',c)
        if m and '{' in c.split('\n')[0]: out.append(('T:'+m.group(2),it,c)); continue
        m=re.match(r'(?:extern\s+"C"\s+)?(?:static\s+)?(?:inline\s+)?(?:__declspec\(weak\)\s+)?[\w\s\*&:<>,]+?\b([\w:~]+)\s*\(([^)]*)\)',c)
        if m:
            params=re.sub(r'\b(\w+)\s*(?=[,)]|$)','',m.group(2)) if False else m.group(2)
            ptypes=','.join(re.sub(r'\s*\b\w+$','',p.strip()) if len(p.strip().split())>1 and not p.strip().endswith('*') and not p.strip().endswith('&') else p.strip() for p in m.group(2).split(','))
            kind='D' if c.rstrip().endswith('}') else 'd'
            out.append((kind+':'+m.group(1)+'('+ptypes+')',it,c)); continue
        out.append(('O:'+re.sub(r'\s+',' ',c),it,c))
    return out
if __name__=='__main__':
    for f in sys.argv[1:]:
        print('==',f)
        for k,it,c in items(f):
            if k.startswith(('D:','d:')): print('  ',k)
