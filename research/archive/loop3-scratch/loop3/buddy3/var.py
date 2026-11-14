# var.py base.cpp outdir : write variants (dict name -> list of (old,new)) defined in VARS file arg3
import sys, os
base = open(sys.argv[1]).read(); out = sys.argv[2]; os.makedirs(out, exist_ok=True)
ns = {}; exec(open(sys.argv[3]).read(), ns)
for name, reps in ns['VARS'].items():
    s = base; ok = True
    for o, n in reps:
        if s.count(o) != 1: print('BAD', name, s.count(o), o[:60]); ok = False; break
        s = s.replace(o, n)
    if ok: open(f'{out}/{name}.cpp', 'w').write(s)
