"""syms.py tmpl src: list @-symbols with section/values in our compile."""
import sys, json, tempfile, subprocess
sys.path.insert(0, '/home/kyle/workspace/moh-frontline/research/tools')
import fast
t, s = sys.argv[1:3]
w = tempfile.mkdtemp()
obj, log = fast.compile_object(json.load(open(t)), open(s, errors='replace').read(), w)
if not obj: sys.exit(log)
nm = fast.OBJDUMP.replace('objdump', 'nm')
print(subprocess.run([fast.OBJDUMP, '-t', obj], capture_output=True, text=True).stdout)
print(subprocess.run([fast.OBJDUMP, '-s', '-j', '.sdata', obj], capture_output=True, text=True).stdout)
