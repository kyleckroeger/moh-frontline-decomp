"""pc.py src.cpp [func-substring]: compile with ShellMenu_levelstats flags, print disasm."""
import sys, json, tempfile, subprocess, re
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/scratch/game')
import fast
t=json.load(open('/home/kyle/workspace/moh-frontline/config/GMFE69/ShellMenu_levelstats.json'))
src=open(sys.argv[1]).read()
with tempfile.TemporaryDirectory() as d:
    o,log=fast.compile_object(t,src,d)
    if not o: print(log); sys.exit(1)
    txt=subprocess.run([fast.OBJDUMP,'-d',o],capture_output=True,text=True).stdout
pat=sys.argv[2] if len(sys.argv)>2 else ''
show=False
for l in txt.splitlines():
    m=re.match(r'^[0-9a-f]+ <(.+)>:$',l)
    if m: show = pat in m.group(1); 
    if show:
        l=re.sub(r'^\s*[0-9a-f]+:\t[0-9a-f ]+\t','  ',l)
        if l.strip(): print(l)
