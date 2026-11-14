"""nm.py tmpl src: objdump -t / -h of our compile."""
import sys, json, tempfile, subprocess
sys.path.insert(0, '/home/kyle/workspace/moh-frontline/research/tools')
import fast
t, s = sys.argv[1:3]
obj, log = fast.compile_object(json.load(open(t)), open(s, errors='replace').read(), tempfile.mkdtemp())
if not obj: sys.exit(log)
print(subprocess.run([fast.OBJDUMP, '-t', '-h', obj], capture_output=True, text=True).stdout)
