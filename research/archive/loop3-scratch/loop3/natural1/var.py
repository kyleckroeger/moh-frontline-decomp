"""var.py unit func base.cpp variants.py : variants.py defines V = {name: [(old,new),...]}; prints scores."""
import sys, json, tempfile
from pathlib import Path
ROOT = Path('/home/kyle/workspace/moh-frontline')
sys.path.insert(0, str(ROOT / 'scratch/game')); sys.path.insert(0, str(ROOT / 'tools'))
import fast
unit, func, base, vf = sys.argv[1:5]
t = json.load(open(ROOT / f'config/GMFE69/{unit}.json'))
text = open(base).read()
ns = {}; exec(open(vf).read(), ns)
outdir = Path(vf).with_suffix('')
outdir.mkdir(exist_ok=True)
for name, pairs in ns['V'].items():
    s = text
    bad = None
    for o, n in pairs:
        if s.count(o) < 1: bad = o; break
        s = s.replace(o, n, 1) if not o.startswith('ALL:') else s
    if bad: print('MISSING', name, repr(bad[:60])); continue
    (outdir / f'{name}.cpp').write_text(s)
    with tempfile.TemporaryDirectory() as d:
        r = fast.score(t, s, [(f,) for f in func.split(',')], d); print(sum(r.values()), [r[f] for f in func.split(',')], name, flush=True)
