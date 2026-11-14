"""d.py <unit> <src> <func> [ctx]: print unified diff orig vs ours (fast.py normalisation)."""
import sys, json, tempfile, subprocess, difflib
from pathlib import Path
ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / 'scratch/game')); sys.path.insert(0, str(ROOT / 'tools'))
import fast
t = json.load(open(ROOT / f'config/GMFE69/{sys.argv[1]}.json'))
ctx = int(sys.argv[4]) if len(sys.argv) > 4 else 2
with tempfile.TemporaryDirectory() as d:
    obj, log = fast.compile_object(t, open(sys.argv[2]).read(), d)
    if not obj: print(log); sys.exit(1)
    mine = fast.parse(subprocess.run([fast.OBJDUMP, '-dr', obj], capture_output=True, text=True).stdout)
a = fast.target(sys.argv[3]); b = mine.get(sys.argv[3], [])
print('len', len(a), len(b))
for l in difflib.unified_diff(a, b, 'orig', 'ours', n=ctx, lineterm=''): print(l)
