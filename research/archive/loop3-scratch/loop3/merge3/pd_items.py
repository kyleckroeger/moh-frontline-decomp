import sys,re
sys.path.insert(0,'uis')
from body import split, code_of
for f in sys.argv[1:]:
    items,_=split(open('/home/kyle/workspace/moh-frontline/src/game/'+f).read())
    out=[]
    for it in items:
        c=code_of(it)
        lines=c.split('\n')
        while lines and lines[0].startswith('#'): lines.pop(0)
        c='\n'.join(lines).strip()
        m=re.match(r'(?:typedef\s+)?(struct|class|union|enum)\s+(\w+)',c)
        if m and '{' in c.split('\n')[0]: out.append('T:'+m.group(2)); continue
        m=re.match(r'(?:extern\s+"C"\s+)?(?:static\s+)?(?:inline\s+)?[\w\s\*&:<>,]+?\b([\w:~]+)\s*\(',c)
        if m:
            out.append(('D:' if c.rstrip().endswith('}') else 'd:')+m.group(1)); continue
        out.append('O:'+c[:40].replace('\n',' '))
    print(f, len(out)); print('  '+' | '.join(out))
