"""pc.py src.cpp [func-substring] [json]: compile with game flags (default player_begincrouch), print disasm."""
import sys, json, tempfile, subprocess, re, os
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/scratch/game')
import fast
j=sys.argv[3] if len(sys.argv)>3 else os.environ.get('J','/home/kyle/workspace/moh-frontline/config/GMFE69/player_begincrouch.json')
t=json.load(open(j))
src=open(sys.argv[1]).read()
with tempfile.TemporaryDirectory() as d:
    o,log=fast.compile_object(t,src,d)
    if not o: print(log); sys.exit(1)
    txt=subprocess.run([fast.OBJDUMP,'-dr',o],capture_output=True,text=True).stdout
pat=sys.argv[2] if len(sys.argv)>2 else ''
show=False
for l in txt.splitlines():
    m=re.match(r'^[0-9a-f]+ <(.+)>:$',l)
    if m: show = pat in m.group(1); print(l) if show else None; continue
    if show:
        l=re.sub(r'^\s*[0-9a-f]+:\t[0-9a-f ]+\t','  ',l)
        if l.strip(): print(l)
