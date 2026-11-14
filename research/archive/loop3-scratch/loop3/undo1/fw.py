"""fw.py unit func variants.py: V maps name -> whole source."""
import sys, json, tempfile
from pathlib import Path
ROOT = Path('/home/kyle/workspace/moh-frontline')
sys.path.insert(0, str(ROOT / 'scratch/game')); sys.path.insert(0, str(ROOT / 'tools'))
import fast
unit, func, vf = sys.argv[1:4]
t = json.load(open(ROOT / f'config/GMFE69/{unit}.json'))
ns = {}; exec(open(vf).read(), ns)
outdir = Path(vf).with_suffix(''); outdir.mkdir(exist_ok=True)
for name, s in ns['V'].items():
    (outdir / f'{name}.cpp').write_text(s)
    with tempfile.TemporaryDirectory() as d:
        r = fast.score(t, s, [(f,) for f in func.split(',')], d); print(sum(r.values()), name, flush=True)
