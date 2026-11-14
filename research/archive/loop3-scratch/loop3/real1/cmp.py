"""cmp.py UNIT...: compile src/game/UNIT.cpp and units/UNIT.cpp with the unit's manifest; report whether the objects' disassembly+relocs+data are identical."""
import sys,json,subprocess,tempfile,re
R='/home/kyle/workspace/moh-frontline/'
sys.path.insert(0,R+'research/tools'); import fast
def dump(t,text):
    with tempfile.TemporaryDirectory() as w:
        obj,log=fast.compile_object(t,text,w)
        if not obj: return 'COMPILE FAIL '+str(log)[-400:]
        d=subprocess.run(['powerpc-eabi-objdump','-dr',obj],capture_output=True,text=True).stdout if False else None
        o=subprocess.run([fast.OBJDUMP if hasattr(fast,'OBJDUMP') else 'objdump','-dr','-s',obj],capture_output=True,text=True)
        return re.sub(r'^.*file format.*$','',o.stdout+o.stderr,flags=re.M)
for u in sys.argv[1:]:
    t=json.load(open(R+f'config/GMFE69/{u}.json'))
    a=dump(t,open(R+f'src/game/{u}.cpp').read()); b=dump(t,open(R+f'scratch/loop3/real1/units/{u}.cpp').read())
    print(u,'SAME' if a==b else 'DIFF', len(a), (b[:300] if b.startswith('COMPILE') else ''))
