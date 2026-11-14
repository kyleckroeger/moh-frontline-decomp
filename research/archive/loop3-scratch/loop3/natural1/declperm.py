"""declperm.py unit func src 'startmarker' n_lines : try moving each of n decl lines after startmarker to every position; print improvements."""
import sys, json, tempfile, itertools
from pathlib import Path
ROOT = Path('/home/kyle/workspace/moh-frontline')
sys.path.insert(0, str(ROOT / 'scratch/game')); sys.path.insert(0, str(ROOT / 'tools'))
import fast
unit, func, src, marker, n = sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4].encode().decode('unicode_escape'), int(sys.argv[5])
t = json.load(open(ROOT / f'config/GMFE69/{unit}.json'))
text = open(src).read()
i = text.index(marker) + len(marker)
lines = text[i:].split('\n')
decls, rest = lines[:n], lines[n:]
def sc(ds):
    s = text[:i] + '\n'.join(list(ds) + rest)
    with tempfile.TemporaryDirectory() as d:
        return fast.score(t, s, [(func,)], d)[func], s
best, _ = sc(decls); print('base', best, flush=True)
seen = set()
for a in range(n):
    for b in range(n):
        if a == b: continue
        ds = decls[:]; x = ds.pop(a); ds.insert(b, x)
        k = tuple(ds)
        if k in seen: continue
        seen.add(k)
        s, txt = sc(ds)
        if s < best + 1:
            print(s, a, '->', b, [d.strip() for d in ds], flush=True)
        if s == 0:
            open(src + '.perm0.cpp', 'w').write(txt); sys.exit(0)
