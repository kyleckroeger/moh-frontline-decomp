"""dr.py tmpl src func [grep]: objdump -dr of our compile of func."""
import sys, json, tempfile, subprocess
sys.path.insert(0, '/home/kyle/workspace/moh-frontline/research/tools')
import fast
t, s, f = sys.argv[1:4]
w = tempfile.mkdtemp()
obj, log = fast.compile_object(json.load(open(t)), open(s, errors='replace').read(), w)
if not obj: sys.exit(log)
print(subprocess.run([fast.OBJDUMP, '-dr', '--disassemble=' + f, obj], capture_output=True, text=True).stdout)
