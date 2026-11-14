"""fs.py config func addr size variants.py [names]: V name -> whole source; score.real."""
import sys
sys.path.insert(0, '/home/kyle/workspace/moh-frontline/research/tools')
import os; os.chdir('/home/kyle/workspace/moh-frontline')
import score
from pathlib import Path
cfg, func, addr, size, vf = sys.argv[1:6]
only = sys.argv[6:]
ns = {}; exec(open(vf).read(), ns)
outdir = Path(vf).with_suffix(''); outdir.mkdir(exist_ok=True)
for name, s in ns['V'].items():
    if only and name not in only: continue
    (outdir / f'{name}.cpp').write_text(s)
    print(score.real(cfg, s, func, addr, size), name, flush=True)
