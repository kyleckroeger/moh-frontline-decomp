# unitcmp.py json srcA srcB : per-function diff counts vs original for both sources
import sys, json, tempfile, subprocess
sys.path.insert(0, '/home/kyle/workspace/moh-frontline/research/tools'); import fast
t = json.load(open(sys.argv[1]))
res = []
for s in sys.argv[2:]:
    w = tempfile.mkdtemp(); obj, log = fast.compile_object(t, open(s).read(), w)
    names = list(fast.parse(subprocess.run([fast.OBJDUMP, '-dr', obj], capture_output=True, text=True).stdout).keys())
    res.append(fast.score(t, open(s).read(), [(n,) for n in names], w))
for n in res[0]:
    print(n[:60], [r.get(n) for r in res])
