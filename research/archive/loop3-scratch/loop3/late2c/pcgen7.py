import sys,os,itertools
os.chdir('/home/kyle/workspace/moh-frontline')
sys.path.insert(0,'research/tools'); sys.path.insert(0,'scratch/game')
import score
base=open('scratch/loop3/late2c/v/pch_200.c').read()
L=[l for l in base.split('\n') if 'pan(' in l and 'azimuth - az[' in l][0]
N='sndgs.speakers'; I=' '*16
X="azimuth - az[%s - 1]"%N; Y="0x10000 - azimuth + az[0]"
P0="&sndaztospkr[i][0]"; P1="&sndaztospkr[i][%s - 1]"%N
res=[]
for co,so,mo,sto,form,swap in itertools.product(range(2),range(2),range(2),range(2),range(3),range(2)):
    den=["(y + x)","(x + y)"][so]
    mx=["x * 127","127 * x"][mo]; my=["y * 127","127 * y"][mo]
    if form==0:
        comp=["int a = %s / %s;"%(mx,den),"int b = %s / %s;"%(my,den)]
        if co: comp=comp[::-1]
        stv=["*p = shape(a);","*q = shape(b);"]
        if sto: stv=stv[::-1]
        body=comp+stv
    elif form==1:
        body=["int s = %s;"%den[1:-1]]+(["int a = %s / s;"%mx,"int b = %s / s;"%my][::(-1 if co else 1)])+(["*p = shape(a);","*q = shape(b);"][::(-1 if sto else 1)])
    else:
        if co or sto: continue
        body=["signed char c = shape(%s / %s);"%(mx,den),"*q = shape(%s / %s);"%(my,den),"*p = c;"]
    H="static inline void pan2(signed char* p, signed char* q, int x, int y) {\n"+''.join('    %s\n'%z for z in body)+"}\n"
    call="pan2(%s, %s, %s, %s);"%((P0,P1,X,Y) if not swap else (P1,P0,Y,X))
    t=base.replace(L,I+call).replace("\nvoid SNDI_precalcaztospkrvol() {","\n"+H+"\nvoid SNDI_precalcaztospkrvol() {")
    r=score.real('config/GMFE69/satospkr.json',t,'SNDI_precalcaztospkrvol__Fv','8015f538','708')
    s=9999 if r is None else r[0]+abs(r[1])*5
    fn='scratch/loop3/late2c/v/pc7_%d%d%d%d%d%d.c'%(co,so,mo,sto,form,swap); open(fn,'w').write(t); res.append((s,fn))
res.sort(); print(res[:8])
