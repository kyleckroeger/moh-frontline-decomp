# dd2.py src func_exact [ctx]: diff of an (alias) function vs original
import sys, re, tempfile, subprocess, difflib
import ms
from ms import fast
w = tempfile.mkdtemp(); obj, log = fast.compile_object(ms.t, open(sys.argv[1]).read(), w)
if not obj: sys.exit(log)
mine = fast.parse(subprocess.run([fast.OBJDUMP, '-dr', obj], capture_output=True, text=True).stdout)
n = sys.argv[2]; base = re.sub(r'_ZZ\w*?(__F)', r'\1', n); a, s = ms.ADDR[base]
for l in difflib.unified_diff(fast.target(base, a, a + s), mine[n], n=int(sys.argv[3]) if len(sys.argv) > 3 else 1, lineterm=''): print(l)
