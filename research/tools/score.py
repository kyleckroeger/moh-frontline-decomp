"""score.py: canonical real-line scorer (same rules as scratch/solo/gb/real.py). real(tmpl,src_text,func,addr_hex,size,suffix=None) -> (bad,len_diff) or None."""
import sys,json,re,os,tempfile
import fast,varmap
def eq(x,y):
    if x==y: return True
    if re.sub(r'-?\d+','N',x)==re.sub(r'-?\d+','N',y) and re.search(r'^(lis|addi) r\d+,(r\d+,)?0$',y): return True
    if y.endswith('0(0)') and x.split()[0]==y.split()[0]: return True
    return False
def real(t,text,f,a,size):
    a=a.replace('0x',''); e=hex(int(a,16)+int(size))
    with tempfile.TemporaryDirectory() as w:
        obj,log=fast.compile_object(json.load(open(t)),text,w)
        if not obj: return None
        m=varmap.rows(['--disassemble='+f,obj]); o=varmap.rows(['--start-address=0x'+a,'--stop-address='+e,fast.ELF])
    if not m: return None
    return sum(1 for x,y in zip(o,m) if not eq(x,y))+abs(len(o)-len(m)),len(m)-len(o)
