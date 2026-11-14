import sys,json,tempfile
sys.path.insert(0,'scratch/loop3/big1'); sys.path.insert(0,'scratch/loop3/big2'); sys.path.insert(0,'research/tools'); sys.path.insert(0,'scratch/game')
import sc,fast,varmap,who2
t=open(sys.argv[1]).read()
for ver in sys.argv[2:]:
    cfg=json.load(open(sc.T)); cfg['compiler_version']=ver
    with tempfile.TemporaryDirectory() as w:
        try: obj,log=fast.compile_object(cfg,t,w)
        except Exception as e: print(ver,'ERR',e); continue
        if not obj: print(ver,'FAIL'); continue
        m=varmap.rows(['--disassemble='+sc.F,obj])
    tb=who2.tab(m); print(ver,*sc.regions(m),tb[-1] if tb else '')
