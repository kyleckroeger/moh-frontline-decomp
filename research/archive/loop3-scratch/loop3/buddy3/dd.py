# dd.py json src func_substr : unified diff orig vs ours (normalized)
import sys, json, tempfile, subprocess, difflib
sys.path.insert(0, '/home/kyle/workspace/moh-frontline/research/tools'); import fast
t = json.load(open(sys.argv[1])); w = tempfile.mkdtemp()
obj, log = fast.compile_object(t, open(sys.argv[2]).read(), w)
if not obj: sys.exit(log)
mine = fast.parse(subprocess.run([fast.OBJDUMP, '-dr', obj], capture_output=True, text=True).stdout)
for n in mine:
    if sys.argv[3] in n:
        a = fast.target(n); b = mine[n]
        for l in difflib.unified_diff(a, b, n=int(sys.argv[4]) if len(sys.argv) > 4 else 2, lineterm=''): print(l)
