import sys,os,itertools
os.chdir('/home/kyle/workspace/moh-frontline')
sys.path.insert(0,'research/tools'); sys.path.insert(0,'scratch/game')
import score
base=open('scratch/loop3/late2c/v/pch_200.c').read()
L=[l for l in base.split('\n') if 'pan(' in l and 'azimuth - az[' in l][0]
N='sndgs.speakers'; I=' '*16
X="azimuth - az[%s - 1]"%N; Y="0x10000 - azimuth + az[0]"
res=[]
for ba,sf,ab,st,sumpos in itertools.product(range(2),range(2),range(2),range(2),range(2)):
    d=["int before = %s;"%X,"int after = %s;"%Y]
    if ba: d=d[::-1]
    sm="int sum = %s;"%(["after + before","before + after"][sf])
    ab_=["int a = before * 127 / sum;","int b = after * 127 / sum;"]
    if ab: ab_=ab_[::-1]
    body=d+[sm]+ab_ if sumpos==0 else d+[sm]+ab_
    st_=["sndaztospkr[i][0] = shape(a);","sndaztospkr[i][%s - 1] = shape(b);"%N]
    if st: st_=st_[::-1]
    v='\n'.join(I+z for z in body+st_)
    t=base.replace(L,v)
    r=score.real('config/GMFE69/satospkr.json',t,'SNDI_precalcaztospkrvol__Fv','8015f538','708')
    s=9999 if r is None else r[0]+abs(r[1])*5
    fn='scratch/loop3/late2c/v/pc6_%d%d%d%d.c'%(ba,sf,ab,st); open(fn,'w').write(t); res.append((s,fn))
# helper with sum first
H="""static inline void pan2(signed char* p, signed char* q, int x, int y) {
    int s = y + x;
    *p = shape(x * 127 / s);
    *q = shape(y * 127 / s);
}
"""
for k,(x,y) in enumerate([(X,Y)]):
    t=base.replace(L,I+"pan2(&sndaztospkr[i][0], &sndaztospkr[i][%s - 1], %s, %s);"%(N,x,y)).replace("\nvoid SNDI_precalcaztospkrvol() {","\n"+H+"\nvoid SNDI_precalcaztospkrvol() {")
    r=score.real('config/GMFE69/satospkr.json',t,'SNDI_precalcaztospkrvol__Fv','8015f538','708')
    s=9999 if r is None else r[0]+abs(r[1])*5
    fn='scratch/loop3/late2c/v/pc6_h%d.c'%k; open(fn,'w').write(t); res.append((s,fn))
res.sort(); print(res[:8])
