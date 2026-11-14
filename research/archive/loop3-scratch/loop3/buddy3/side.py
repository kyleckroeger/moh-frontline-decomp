import sys, re, tempfile, subprocess
import ms
from ms import fast
w = tempfile.mkdtemp(); obj, log = fast.compile_object(ms.t, open(sys.argv[1]).read(), w)
mine = fast.parse(subprocess.run([fast.OBJDUMP, '-dr', obj], capture_output=True, text=True).stdout)
n = sys.argv[2]; base = re.sub(r'_ZZ\w*?(__F)', r'\1', n); a, s = ms.ADDR[base]
o = fast.target(base, a, a + s); m = mine[n]
lo, hi = (int(sys.argv[3]), int(sys.argv[4])) if len(sys.argv) > 4 else (0, max(len(o), len(m)))
for i in range(lo, min(hi, max(len(o), len(m)))):
    x = o[i] if i < len(o) else ''; y = m[i] if i < len(m) else ''
    print(f'{i:4} {x:34} {y:34} {"<<" if x != y else ""}')
