"""slots.py tmpl src func addr size: r1-offset sequence (stores) ours vs orig, ignoring prologue/epilogue saves"""
import sys,json,re,subprocess,tempfile
sys.path.insert(0,'scratch/game'); import fast
t,src,f,a,n=sys.argv[1:6]; a=int(a,16); n=int(n)
O='build/tools/powerpc-eabi-objdump'
def seq(txt):
    r=[]
    for l in txt.split('\n'):
        m=re.search(r'\t(st\w+|l\w+)\s+[fr]\d+,(\d+)\(r1\)',l)
        if m and not m.group(1).startswith(('stmw','lmw','psq')) : r.append(('S' if m.group(1).startswith('st') else 'L')+m.group(2))
    return r
o,log=fast.compile_object(json.load(open(t)),open(src).read(),tempfile.mkdtemp())
ours=subprocess.run([O,'-d','--disassemble='+f,o],capture_output=True,text=True).stdout
orig=subprocess.run([O,'-d','--start-address=%#x'%a,'--stop-address=%#x'%(a+n),fast.ELF],capture_output=True,text=True).stdout
print('ours',' '.join(seq(ours))); print('orig',' '.join(seq(orig)))
