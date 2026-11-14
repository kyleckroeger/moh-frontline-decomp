"""dd.py template source func : unified diff of normalized instructions"""
import sys, json, tempfile, difflib, subprocess
sys.path.insert(0, 'scratch/game')
import fast
t = json.load(open(sys.argv[1])); src = open(sys.argv[2]).read(); f = sys.argv[3]
with tempfile.TemporaryDirectory() as d:
    obj, log = fast.compile_object(t, src, d)
    mine = fast.parse(subprocess.run([fast.OBJDUMP, '-dr', obj], capture_output=True, text=True).stdout)
    a = fast.target(f); b = mine.get(f, [])
    for l in difflib.unified_diff(a, b, 'orig', 'mine', n=2, lineterm=''): print(l)
