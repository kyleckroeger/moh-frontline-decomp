import sys,os,itertools
os.chdir('/home/kyle/workspace/moh-frontline')
sys.path.insert(0,'research/tools'); sys.path.insert(0,'scratch/game')
import score
part=int(sys.argv[1])
base=open('scratch/loop3/late2c/v/pch_200.c').read()
L=[l for l in base.split('\n') if 'pan(' in l and 'azimuth - az[' in l][0]
N='sndgs.speakers'; I=' '*16
Xs=["azimuth - az[%s - 1]"%N,"-az[%s - 1] + azimuth"%N]
Ys=["0x10000 - azimuth + az[0]","az[0] + (0x10000 - azimuth)","az[0] - azimuth + 0x10000","0x10000 + az[0] - azimuth","(0x10000 - azimuth) + az[0]"]
P0="&sndaztospkr[i][0]"; P1="&sndaztospkr[i][%s - 1]"%N
res=[]
n=0
for co,so,sto,mo,yi,xi,cf in itertools.product(range(2),range(2),range(2),range(2),range(5),range(2),range(3)):
    n+=1
    if n%2!=part: continue
    den=["(y + x)","(x + y)"][so]
    mx=["x * 127","127 * x"][mo]; my=["y * 127","127 * y"][mo]
    comp=["int a = %s / %s;"%(mx,den),"int b = %s / %s;"%(my,den)]
    if co: comp=comp[::-1]
    stv=["*p = shape(a);","*q = shape(b);"]
    if sto: stv=stv[::-1]
    H="static inline void pan2(signed char* p, signed char* q, int x, int y) {\n"+''.join('    %s\n'%z for z in comp+stv)+"}\n"
    X=Xs[xi]; Y=Ys[yi]
    if cf==0: call=[ "pan2(%s, %s, %s, %s);"%(P1,P0,Y,X)]
    elif cf==1: call=["int before = %s;"%X,"int after = %s;"%Y,"pan2(%s, %s, after, before);"%(P1,P0)]
    else: call=["int after = %s;"%Y,"int before = %s;"%X,"pan2(%s, %s, after, before);"%(P1,P0)]
    t=base.replace(L,'\n'.join(I+z for z in call)).replace("\nvoid SNDI_precalcaztospkrvol() {","\n"+H+"\nvoid SNDI_precalcaztospkrvol() {")
    r=score.real('config/GMFE69/satospkr.json',t,'SNDI_precalcaztospkrvol__Fv','8015f538','708')
    s=9999 if r is None else r[0]+abs(r[1])*5
    fn='scratch/loop3/late2c/v/pc11_%d%d%d%d%d%d%d.c'%(co,so,sto,mo,yi,xi,cf)
    if s<4: open(fn,'w').write(t); print('HIT',s,fn,flush=True)
    res.append((s,fn))
res.sort(); print(res[:5])
