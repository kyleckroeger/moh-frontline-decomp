import re,sys,glob
sys.path.insert(0,'research/tools')
from try_header import type_span
cls=sys.argv[1]
pat=re.compile(r"^(?:class|struct)\s+"+re.escape(cls)+r"\b[^;{]*\{",re.M)
for f in sorted(glob.glob('src/**/*.c*',recursive=True)):
    t=open(f,errors='replace').read()
    if not pat.search(t): continue
    s=type_span(t,cls)
    v=t[s[0]:s[1]]
    if len(sys.argv)>2 and sys.argv[2]=='full':
        print('=====',f); print(v)
    else:
        body=' '.join(l.strip() for l in v.splitlines()[1:-1] if l.strip() and not l.strip().startswith(('/*','*','//')))
        print(f'{f}: {body[:400]}')
