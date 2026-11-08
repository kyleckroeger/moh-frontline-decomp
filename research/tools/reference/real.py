"""real.py [tsv]: for each open fn with a draft: real differing lines (relocs/SDA normalised), register-only count. Parallel 4. Output: real regonly size addr name draft"""
import sys,json,re,csv,os,tempfile,difflib,concurrent.futures as cf
sys.path.insert(0,'scratch/game'); sys.path.insert(0,'scratch/solo/pred')
import fast,varmap
def norm(x):
    x=re.sub(r'^lis (r\d+),-?\d+$',r'lis \1,H',x)
    x=re.sub(r'^addi (r\d+),(r\d+),-?\d+$',lambda m:f'addi {m[1]},{m[2]},L',x) if 0 else x
    return x
def one(r):
    raw,st,a,sz,f,t,d=r
    e=hex(int(a,16)+int(sz)); os.environ
    try:
        with tempfile.TemporaryDirectory() as w:
            obj,log=fast.compile_object(json.load(open(t)),open(d,errors='replace').read(),w)
            if not obj: return None
            m=varmap.rows(['--disassemble='+f,obj]); o=varmap.rows(['--start-address=0x'+a,'--stop-address='+e,fast.ELF])
    except Exception as ex: return None
    if not m: return None
    def eq(x,y):
        if x==y: return True
        if re.sub(r'-?\d+','N',x)==re.sub(r'-?\d+','N',y) and re.search(r'^(lis|addi) r\d+,(r\d+,)?0$',y): return True
        if y.endswith('0(0)') and x.split()[0]==y.split()[0]: return True
        return False
    bad=sum(1 for x,y in zip(o,m) if not eq(x,y))+abs(len(o)-len(m))
    blind=lambda s:re.sub(r'\b[fr]\d+\b','R',s)
    rg=sum(1 for x,y in zip(o,m) if not eq(x,y) and eq(blind(x),blind(y)))
    return bad,rg,len(m)-len(o),int(sz),a,f,d
rows=[r for r in csv.reader(open(sys.argv[1] if len(sys.argv)>1 else 'scratch/solo/sweep/table3.tsv'),delimiter='\t') if r[6]!='-' and r[6].endswith(('.cpp','.c'))]
rows=[r for r in rows if r[6].endswith('.cpp') and os.path.exists(r[6])]
with cf.ThreadPoolExecutor(4) as x:
    res=[v for v in x.map(one,rows) if v]
res.sort(key=lambda v:(v[0]))
for v in res: print(*v,sep='\t')
