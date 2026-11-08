import re,csv,subprocess,ast
from concurrent.futures import ThreadPoolExecutor as TP
real={x[5][:40]:x for x in csv.reader(open('scratch/solo/gb/real.tsv'),delimiter='\t') if x and x[0].isdigit()}
tm={x[2]:x[5] for x in csv.reader(open('scratch/solo/sweep/table3.tsv'),delimiter='\t') if x}
best={}
for lg in ('sweep.log','sweep2.log'):
    for l in open('scratch/solo/ret/'+lg):
        if not l.startswith('('): continue
        r=ast.literal_eval(l)
        if r[3] not in best or r[0]<best[r[3]][0]: best[r[3]]=r
def sc(t,p,f,a,e):
    o=subprocess.run(['python3','scratch/solo/pred/sbs.py',t,p,f,a,e],capture_output=True,text=True)
    return sum(1 for l in o.stdout.splitlines() if l.endswith('<<') and not re.search(r',0 +<<$|0\(0\) +<<$',l)) if o.stdout else None
def go(k):
    r=best[k]; x=real.get(k)
    if not x: return None
    a=x[4];e=hex(int(a,16)+int(x[3]));t=tm.get(a)
    return (sc(t,x[6],x[5],'0x'+a,e),r[0],int(x[3]),k,r[4][:50],r[5])
with TP(4) as ex: R=[r for r in ex.map(go,best) if r and r[0] is not None]
for b,n,sz,k,l,p in sorted(R,key=lambda r:r[1]-r[0]):
    if n<b: print(f'{b}->{n} {sz} {k[:36]} | {l} | {p}')
print('checked',len(R))
