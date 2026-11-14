# climb.py FUNC draft L1 L2 out: hill-climb single-line moves within lines [L1,L2] (1-based) by (real, difflib)
import sys, os, csv, json, tempfile, subprocess, difflib
from concurrent.futures import ThreadPoolExecutor
ROOT = '/home/kyle/workspace/moh-frontline'
sys.path.insert(0, ROOT + '/scratch/game'); sys.path.insert(0, ROOT + '/research/tools')
os.chdir(ROOT)
import fast, score
func, draft, L1, L2, out = sys.argv[1], sys.argv[2], int(sys.argv[3]) - 1, int(sys.argv[4]), sys.argv[5]
r = {x['function']: x for x in csv.DictReader(open('research/drafts/index.tsv'), delimiter='\t')}[func]
t = r['compile_settings_from']; tj = json.load(open(t)); tgt = fast.target(func)
def sc(text):
    v = score.real(t, text, func, r['address'], r['size'])
    if v is None: return None
    w = tempfile.mkdtemp(); obj, _ = fast.compile_object(tj, text, w)
    m = fast.parse(subprocess.run([fast.OBJDUMP, '-dr', obj], capture_output=True, text=True).stdout).get(func, [])
    sm = difflib.SequenceMatcher(None, tgt, m, autojunk=False)
    subprocess.run(['rm', '-rf', w])
    return (v[0] + abs(v[1]), sum((i2 - i1) + (j2 - j1) for op, i1, i2, j1, j2 in sm.get_opcodes() if op != 'equal'))
lines = open(draft).read().split('\n'); B = lines[L1:L2]
mk = lambda C: '\n'.join(lines[:L1] + C + lines[L2:])
best = sc(mk(B)); print('base', best, flush=True)
while True:
    cands = []
    for v in range(len(B)):
        for p in range(len(B)):
            if p == v: continue
            C = B[:v] + B[v + 1:]; C.insert(p, B[v]); cands.append(C)
    with ThreadPoolExecutor(4) as ex:
        res = list(ex.map(lambda C: (sc(mk(C)), C), cands))
    res = [x for x in res if x[0] is not None]
    s, C = min(res, key=lambda x: x[0])
    if s >= best: break
    best, B = s, C; print('step', best, [b.strip() for b in B], flush=True)
    open(out, 'w').write(mk(B))
print('final', best)
