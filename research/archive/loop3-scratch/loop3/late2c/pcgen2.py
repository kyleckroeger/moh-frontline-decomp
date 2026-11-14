import itertools,random,sys,os
os.chdir('/home/kyle/workspace/moh-frontline')
sys.path.insert(0,'research/tools'); sys.path.insert(0,'scratch/game')
import score
base=open('scratch/loop3/late2c/v/pcs_b.c').read()
B1=base[base.index("                int before = az[0] - azimuth;"):base.index("            } else if (azimuth >= az")]
B2s=base.index("                int before = azimuth - az[sndgs.speakers - 1];")
B2=base[B2s:base.index("            } else {",B2s)]
N='sndgs.speakers'
def block(kind,k):
    I=' '*16
    if kind==1:
        bef="az[0] - azimuth"; aft=["azimuth + (0x10000 - az[%s - 1])"%N,"(0x10000 - az[%s - 1]) + azimuth"%N,"0x10000 - az[%s - 1] + azimuth"%N][k['aft']]
        na,nb=('after','before')
    else:
        bef="azimuth - az[%s - 1]"%N; aft=["0x10000 - azimuth + az[0]","az[0] + (0x10000 - azimuth)","az[0] - azimuth + 0x10000"][k['aft']]
        na,nb=('before','after')
    d=["int before = %s;"%bef,"int after = %s;"%aft]
    if k['ba']: d=d[::-1]
    den=['(before + after)','(after + before)'][k['den']]
    if k['sum']:
        d.append("int sum = %s;"%den[1:-1]); den='sum'
    ab=["int a = %s * 127 / %s;"%(na,den),"int b = %s * 127 / %s;"%(nb,den)]
    if k['ab']: ab=ab[::-1]
    st=["sndaztospkr[i][0] = shape(a);","sndaztospkr[i][%s - 1] = shape(b);"%N]
    if k['st']: st=st[::-1]
    return ''.join(I+x+'\n' for x in d+ab)+'\n'+''.join(I+x+'\n' for x in st)
KN=['aft','ba','den','sum','ab','st']
R={'aft':3}
def gen(k1,k2):
    return base.replace(B1,block(1,k1)).replace(B2,block(2,k2))
def sc(k1,k2):
    r=score.real('config/GMFE69/satospkr.json',gen(k1,k2),'SNDI_precalcaztospkrvol__Fv','8015f538','708')
    return 9999 if r is None else r[0]+abs(r[1])*5
z={x:0 for x in KN}
if len(sys.argv)==1: print("base",sc(z,z))
best=None
for which in ((1,2) if len(sys.argv)==1 else ()):
    res=[]
    for vals in itertools.product(*[range(R.get(x,2)) for x in KN]):
        k=dict(zip(KN,vals))
        s=sc(k,z) if which==1 else sc(z,k)
        res.append((s,vals))
    res.sort(); print(which,res[:6],flush=True)
if len(sys.argv)>1:
    import json
    k1=json.loads(sys.argv[1]); k2=json.loads(sys.argv[2])
    open(sys.argv[3],'w').write(gen({**z,**k1},{**z,**k2}))
