import itertools,sys,os
os.chdir('/home/kyle/workspace/moh-frontline')
sys.path.insert(0,'research/tools'); sys.path.insert(0,'scratch/game')
import score
base=open(sys.argv[2] if len(sys.argv)>2 else 'scratch/loop3/late2c/v/pcs_b.c').read()
which=int(sys.argv[1])
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
    dens=['(before + after)','(after + before)']
    ma=["%s * 127"%na,"127 * %s"%na][k['mA']]; mb=["%s * 127"%nb,"127 * %s"%nb][k['mB']]
    ab=["int a = %s / %s;"%(ma,dens[k['dA']]),"int b = %s / %s;"%(mb,dens[k['dB']])]
    if k['ab']: ab=ab[::-1]
    st=["sndaztospkr[i][0] = shape(a);","sndaztospkr[i][%s - 1] = shape(b);"%N]
    if k['st']: st=st[::-1]
    return ''.join(I+x+'\n' for x in d+ab)+'\n'+''.join(I+x+'\n' for x in st)
KN=['aft','ba','dA','dB','mA','mB','ab','st']
R={'aft':3}
z={x:0 for x in KN}
def gen(k1,k2): return base.replace(B1,block(1,k1)).replace(B2,block(2,k2))
def sc(t):
    r=score.real('config/GMFE69/satospkr.json',t,'SNDI_precalcaztospkrvol__Fv','8015f538','708')
    return 9999 if r is None else r[0]+abs(r[1])*5
res=[]
for vals in itertools.product(*[range(R.get(x,2)) for x in KN]):
    k=dict(zip(KN,vals))
    t=gen(k,z) if which==1 else gen(z,k)
    s=sc(t); res.append((s,vals))
    if s<20: open('scratch/loop3/late2c/v/pcg%d_%s.c'%(which,''.join(map(str,vals))),'w').write(t)
res.sort(); print(which,res[:8],flush=True)
