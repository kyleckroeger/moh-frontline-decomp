# autods.py FUNC draft [out]: find runs (>=2) of consecutive declaration lines outside class bodies,
# try moving each line to every slot of its run (ds2 rule), greedy-apply improvements, rescore with score.real.
import sys, os, re, csv, json, tempfile, subprocess, difflib
ROOT = '/home/kyle/workspace/moh-frontline'; os.chdir(ROOT)
sys.path.insert(0, ROOT + '/scratch/game'); sys.path.insert(0, ROOT + '/research/tools')
sys.path.insert(0, ROOT + '/scratch/loop3/opswap')
import fast, score, opsweep
func, draft = sys.argv[1], sys.argv[2]
out = sys.argv[3] if len(sys.argv) > 3 else None
r = {x['function']: x for x in csv.DictReader(open('research/drafts/index.tsv'), delimiter='\t')}[func]
t = json.load(open(r['compile_settings_from']))
tgt = fast.target(func); w = tempfile.mkdtemp()
def sc(s):
    if os.environ.get('REAL'):
        v = score.real(r['compile_settings_from'], s, func, r['address'], r['size'])
        return None if v is None else v[0] + abs(v[1])
    obj, log = fast.compile_object(t, s, w)
    if not obj: return None
    m = fast.parse(subprocess.run([fast.OBJDUMP, '-dr', obj], capture_output=True, text=True).stdout).get(func)
    if m is None: return None
    sm = difflib.SequenceMatcher(None, tgt, m, autojunk=False)
    return sum((i2 - i1) + (j2 - j1) for op, i1, i2, j1, j2 in sm.get_opcodes() if op != 'equal')
src = open(draft, errors='replace').read()
DECL = re.compile(r'^\s*(const\s+|unsigned\s+|signed\s+|struct\s+|register\s+)*[A-Za-z_][\w:]*(<[^;()]*>)?[\s*&]+\**\s*[A-Za-z_]\w*\s*(\[[^\]]*\])?\s*(=[^;]*)?;\s*(//.*)?$')
BAD = re.compile(r'^\s*(return|delete|goto|case|else|throw)\b')
def runs(s):
    lines = s.split('\n'); cr = opsweep.class_ranges(s); pos = 0; offs = []
    for l in lines: offs.append(pos); pos += len(l) + 1
    ok = [bool(DECL.match(l)) and not BAD.match(l) and not any(a <= offs[n] < b for a, b in cr) for n, l in enumerate(lines)]
    res = []; n = 0
    while n < len(lines):
        if ok[n]:
            m = n
            while m + 1 < len(lines) and ok[m + 1]: m += 1
            if m > n: res.append((n, m + 1))
            n = m + 1
        else: n += 1
    return res
base = sc(src); best = base; cur = src
print('base', base, 'runs', runs(src), flush=True)
improved = True
while improved and best is not None:
    improved = False
    for L1, L2 in runs(cur):
        lines = cur.split('\n'); B = lines[L1:L2]
        for v in range(len(B)):
            for p in range(len(B)):
                if p == v: continue
                C = B[:v] + B[v + 1:]; C.insert(p, B[v])
                s = '\n'.join(lines[:L1] + C + lines[L2:]); x = sc(s)
                if x is not None and x < best:
                    print(x, 'move', B[v].strip(), '->', p, flush=True)
                    best, cur, improved = x, s, True
                    break
            if improved: break
        if improved: break
ro = score.real(r['compile_settings_from'], src, func, r['address'], r['size'])
rn = score.real(r['compile_settings_from'], cur, func, r['address'], r['size'])
print('FINAL', func, 'difflib', base, '->', best, 'real', ro, '->', rn, flush=True)
if out and best is not None and best < base: open(out, 'w').write(cur)
subprocess.run(['rm', '-rf', w])
