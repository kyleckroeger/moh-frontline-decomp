"""fv.py unit func base.cpp start_marker end_marker variants.py : replace region [start, end) of base with each V[name] body."""
import sys, json, tempfile
from pathlib import Path
ROOT = Path('/home/kyle/workspace/moh-frontline')
sys.path.insert(0, str(ROOT / 'scratch/game')); sys.path.insert(0, str(ROOT / 'tools'))
import fast
unit, func, base, sm, em, vf = sys.argv[1:7]
t = json.load(open(ROOT / f'config/GMFE69/{unit}.json'))
text = open(base).read()
i = text.index(sm); j = text.index(em, i)
ns = {}; exec(open(vf).read(), ns)
outdir = Path(vf).with_suffix(''); outdir.mkdir(exist_ok=True)
only = sys.argv[7:] 
for name, body in ns['V'].items():
    if only and name not in only: continue
    s = text[:i] + body + text[j:]
    (outdir / f'{name}.cpp').write_text(s)
    with tempfile.TemporaryDirectory() as d:
        r = fast.score(t, s, [(f,) for f in func.split(',')], d); print(sum(r.values()), name, flush=True)
