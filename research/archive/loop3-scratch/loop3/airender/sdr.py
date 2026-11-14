"""sd.py tmpl src func start end: struct diff, reloc immediates masked (lis/addi/lwz/stw/lfs ... off(r0-ish))."""
import sys, re, difflib, json, tempfile, subprocess
sys.path.insert(0, '/home/kyle/workspace/moh-frontline/scratch/game'); import fast
import os; KEEPREG = os.environ.get("KEEPREG")
t = json.load(open(sys.argv[1])); obj, log = fast.compile_object(t, open(sys.argv[2]).read(), tempfile.mkdtemp())
if not obj: print(log); sys.exit(1)
def rows(args):
    out = subprocess.run([fast.OBJDUMP, '-dr'] + args, capture_output=True, text=True).stdout.splitlines()
    r = []; rel = False
    for l in out:
        if re.match(r'^\s*[0-9a-f]+: R_PPC', l):
            if r and not re.search(r'R_PPC_REL24', l): r[-1] = re.sub(r'(-?\d+)(\(R\))?$', r'X\2', r[-1]) if not r[-1].startswith('b') else r[-1]
            continue
        m = re.match(r'^\s*([0-9a-f]+):\t[0-9a-f ]+\t(.*)$', l)
        if m:
            s = re.sub(r'\s+', ' ', re.sub(r'<.*>', '', m.group(2))).strip()
            s = re.sub(r'\b(b\w*) [0-9a-f]+$', r'\1 T', s)
            s = s if KEEPREG else re.sub(r"\b[fr]\d+\b", "R", s)
            s = re.sub(r'-?\d+\(R\)$', lambda mm: mm.group(0), s)
            r.append(s)
    return r
m = rows(['--disassemble=' + sys.argv[3], obj]); o = rows(['--start-address=' + sys.argv[4], '--stop-address=' + sys.argv[5], fast.ELF])
# SDA: orig uses (r13)/(r2) offsets with relocs too; both masked above as X
n = 0; out = []
for op, i1, i2, j1, j2 in difflib.SequenceMatcher(None, o, m, autojunk=False).get_opcodes():
    if op != 'equal':
        n += max(i2 - i1, j2 - j1); out.append(f"{op} {hex(i1*4)} {o[i1:i2]} | {m[j1:j2]}")
print('struct', n, 'len', len(o), len(m))
if len(sys.argv) > 6: print('\n'.join(x[:300] for x in out))
