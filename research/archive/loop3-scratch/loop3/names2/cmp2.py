"""cmp2.py UNITPATH...: (e.g. game/foo.cpp) compile src/UNITPATH and names2/units/UNITPATH with the unit's manifest; SAME if objdump -dr -s equal."""
import sys,json,subprocess,tempfile,re,os
R='/home/kyle/workspace/moh-frontline/'
sys.path.insert(0,R+'research/tools'); import fast
def dump(t,text,suffix):
    os.environ['SUFFIX']=suffix
    with tempfile.TemporaryDirectory() as w:
        obj,log=fast.compile_object(t,text,w)
        if not obj: return 'COMPILE FAIL '+str(log)[-600:]
        o=subprocess.run([fast.OBJDUMP,'-dr','-s',obj],capture_output=True,text=True)
        return re.sub(r'^.*file format.*$','',o.stdout+o.stderr,flags=re.M)
for u in sys.argv[1:]:
    base=os.path.splitext(os.path.basename(u))[0]; suf=u.rsplit('.',1)[1]
    t=json.load(open(R+f'config/GMFE69/{base}.json'))
    a=dump(t,open(R+'src/'+u).read(),suf); b=dump(t,open(R+'scratch/loop3/names2/units/'+u).read(),suf)
    print(u,'SAME' if a==b else 'DIFF', len(a), (b[:600] if b.startswith('COMPILE') else ''))
