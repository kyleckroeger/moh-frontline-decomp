import sys,json,tempfile,subprocess,re
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/scratch/game'); import fast
t=json.load(open('/home/kyle/workspace/moh-frontline/scratch/loop3/systems4/t.json'))
for fn in sys.argv[1:]:
    d=tempfile.mkdtemp(); obj,log=fast.compile_object(t,open(fn).read(),d)
    if not obj: print(fn,'ERR'); continue
    m=fast.parse(subprocess.run([fast.OBJDUMP,'-dr',obj],capture_output=True,text=True).stdout)['CreateCollisionVolume__FP20TriggerObject_structi']
    txt='\n'.join(m[:60])
    g=lambda p: (re.search(p,txt) or [None,'?'])[1]
    print(fn, 'trig',g(r'mr (r\d+),r3'),'param',g(r'mr (r\d+),r4'),'base',g(r'addi (r\d+),r3,X'),'core',g(r'lwz (r\d+),4\(r'),'node',g(r'mr\.? (r\d+),r0'))
