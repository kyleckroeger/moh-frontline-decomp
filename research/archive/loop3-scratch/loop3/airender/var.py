"""var.py <tmpl> <base.cpp> <func> <addr> <size> <variants.py>: variants.py defines VARIANTS = [(name, [(old,new),...])]."""
import sys, os
sys.path.insert(0, '/home/kyle/workspace/moh-frontline/research/tools')
os.chdir('/home/kyle/workspace/moh-frontline')
import score
t, base, f, a, size, vp = sys.argv[1:7]
text = open(base).read()
ns = {}; exec(open(vp).read(), ns)
for name, subs in ns['VARIANTS']:
    s = text
    ok = True
    for o, n in subs:
        if o not in s: print(name, 'MISSING', o[:40]); ok = False; break
        s = s.replace(o, n)
    if not ok: continue
    out = os.path.join(os.path.dirname(vp), name + '.cpp'); open(out, 'w').write(s)
    print(name, score.real(t, s, f, a, size), flush=True)
