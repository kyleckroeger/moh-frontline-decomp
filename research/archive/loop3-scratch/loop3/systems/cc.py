"""cc.py <template.json> <source> : compile into a temp dir, print objdump -t text symbols in order + optional disasm of names"""
import json, sys, tempfile, subprocess
sys.path.insert(0, 'scratch/game')
import fast
t = json.load(open(sys.argv[1])); src = open(sys.argv[2]).read()
with tempfile.TemporaryDirectory() as d:
    obj, log = fast.compile_object(t, src, d)
    if not obj: print(log); sys.exit(1)
    out = subprocess.run([fast.OBJDUMP, '-dr', obj], capture_output=True, text=True).stdout
    if len(sys.argv) > 3 and sys.argv[3] == "t":
        print(subprocess.run([fast.OBJDUMP, "-t", obj], capture_output=True, text=True).stdout)
    elif len(sys.argv) > 3 and sys.argv[3] == "dis":
        print(out)
    else:
        import re
        for l in out.splitlines():
            if re.match(r'^[0-9a-f]+ <', l) or 'Disassembly of section' in l: print(l)
