import sys,json,re,subprocess,tempfile
sys.path.insert(0,'scratch/game'); import fast
O='build/tools/powerpc-eabi-objdump'
src=open('research/tools/brchk.py').read(); exec(src[src.index('def load'):src.index('for r in csv')])
t=json.load(open('config/GMFE69/Soldier_object_lit.json')); t['include_dirs']=t.get('include_dirs',[])+['src/game','src/ea','include']
F='UpdateAnimation__14CSoldierObjectFf'
for s in sys.argv[1:]:
  with tempfile.TemporaryDirectory() as w:
    o,log=fast.compile_object(t,open(s).read(),w)
    d=subprocess.run([O,'-d',o],capture_output=True,text=True).stdout
    m=re.search(r'<'+re.escape(F)+r'>:\n(.*?)(?:\n\n|\Z)',d,re.S); A=load(m.group(1).split('\n'))
  B=load(subprocess.run([O,'-d','--start-address=0x800acfb8','--stop-address=0x800adeac','orig/GMFE69/Moh2RelGC.elf'],capture_output=True,text=True).stdout.split('\n'))
  bd=[(i,x,y) for i,(x,y) in enumerate(zip(A,B)) if x!=y and x and y and not (x.startswith('bl') and y.startswith('bl'))]
  print(s,len(A),len(B),'branchdiffs',len(bd),bd[:3])
