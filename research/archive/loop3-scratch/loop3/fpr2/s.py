import sys,json,tempfile
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/research/tools'); import score,fast,varmap
R='/home/kyle/workspace/moh-frontline/'
ROWS={'cv':('config/GMFE69/anim_obj_collide.json','ConstrainVelocity__11CAnimObjectFv','80093e64','1268'),
'ubb':('config/GMFE69/particlesystem_visible.json','UpdateBoundingBox__15CParticleSystemFv','8007fbe8','2948')}
def run(key,text,show=False):
    t,f,a,size=ROWS[key]; e=hex(int(a,16)+int(size))
    with tempfile.TemporaryDirectory() as w:
        obj,log=fast.compile_object(json.load(open(R+t)),text,w)
        if not obj: return None,log
        m=varmap.rows(['--disassemble='+f,obj]); o=varmap.rows(['--start-address=0x'+a,'--stop-address='+e,fast.ELF])
    bad=[(i,x,y) for i,(x,y) in enumerate(zip(o,m)) if not score.eq(x,y)]
    if show:
        for i,x,y in bad: print(i,'|',x,'|',y)
    return len(bad)+abs(len(o)-len(m)),len(m)-len(o)
if __name__=='__main__':
    print(run(sys.argv[1],open(sys.argv[2]).read(),len(sys.argv)>3))
