# objchk.py tmpl src func addr size: branch-target-offset aligned listing (only differing rows)
import sys,json,re,subprocess,tempfile,os
sys.path.insert(0,'scratch/game'); import fast
O='build/tools/powerpc-eabi-objdump'
t=json.load(open(sys.argv[1])); src=sys.argv[2]; fn=sys.argv[3]; a=int(sys.argv[4],16); n=int(sys.argv[5])
if src.endswith('.c'): os.environ['SUFFIX']='c'
o,log=fast.compile_object(t,open(src).read(),tempfile.mkdtemp())
def rows(txt):
    L=[l for l in txt.split('\n') if re.match(r'^\s*[0-9a-f]+:\t',l)]
    base=int(L[0].split(':')[0],16); r=[]
    for l in L:
        p=l.split('\t'); ins=re.sub(r'\s+',' ',p[2].split(' <')[0].strip()) if len(p)>2 else ''
        m=re.match(r'(b\w*) (?:(cr\d),)?([0-9a-f]+)$',ins)
        if m and not ins.startswith('bl '): ins='%s %s+%d'%(m.group(1),(m.group(2)+',') if m.group(2) else '',int(m.group(3),16)-base)
        r.append(ins)
    return r
d=subprocess.run([O,'-d',o],capture_output=True,text=True).stdout
m=re.search(r'<'+re.escape(fn)+r'>:\n(.*?)(?:\n\n|\Z)',d,re.S)
A=rows(m.group(1))
B=rows(subprocess.run([O,'-d','--start-address=%#x'%a,'--stop-address=%#x'%(a+n),'orig/GMFE69/Moh2RelGC.elf'],capture_output=True,text=True).stdout)
lo=int(sys.argv[6]) if len(sys.argv)>6 else 0; hi=int(sys.argv[7]) if len(sys.argv)>7 else 99999
for i in range(lo,min(hi,max(len(A),len(B)))):
    x=B[i] if i<len(B) else ''; y=A[i] if i<len(A) else ''
    print('%3d %s %-34s %s'%(i,'  ' if x==y else '**',x,y))
