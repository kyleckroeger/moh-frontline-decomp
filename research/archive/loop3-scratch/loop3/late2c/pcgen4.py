import itertools,sys,os
os.chdir('/home/kyle/workspace/moh-frontline')
sys.path.insert(0,'research/tools'); sys.path.insert(0,'scratch/game')
import score
base=open('scratch/loop3/late2c/v/pcs_b.c').read()
B1=base[base.index("                int before = az[0] - azimuth;"):base.index("            } else if (azimuth >= az")]
B2s=base.index("                int before = azimuth - az[sndgs.speakers - 1];")
B2=base[B2s:base.index("            } else {",B2s)]
N='sndgs.speakers'
HS=[
"""static inline void pan(signed char* p, signed char* q, int x, int y) {
    int a = x * 127 / (y + x);
    int b = y * 127 / (y + x);
    *p = shape(a);
    *q = shape(b);
}
""",
"""static inline void pan(signed char* p, signed char* q, int x, int y) {
    int a = x * 127 / (x + y);
    int b = y * 127 / (x + y);
    *p = shape(a);
    *q = shape(b);
}
""",
"""static inline void pan(signed char* p, signed char* q, int x, int y) {
    *p = shape(x * 127 / (y + x));
    *q = shape(y * 127 / (y + x));
}
""",
"""static inline void pan(signed char* p, signed char* q, int x, int y) {
    *p = shape(x * 127 / (x + y));
    *q = shape(y * 127 / (x + y));
}
"""]
res=[]
for hi,H in enumerate(HS):
  for argform in range(3):
    for order in range(2):
        # b1: a on after -> x=after, y=before ; b2: x=before, y=after
        b1x,b1y="azimuth + (0x10000 - az[%s - 1])"%N,"az[0] - azimuth"
        b2x,b2y="azimuth - az[%s - 1]"%N,"0x10000 - azimuth + az[0]"
        I=' '*16
        def mk(x,y,bx,by):
            if argform==0:
                return I+"pan(&sndaztospkr[i][0], &sndaztospkr[i][%s - 1], %s, %s);\n"%(N,x,y)
            d=["int before = %s;"%(by if bx=='after' else bx), "int after = %s;"%(bx if bx=='after' else by)]
            if order: d=d[::-1]
            call="pan(&sndaztospkr[i][0], &sndaztospkr[i][%s - 1], %s, %s);"%(N,bx,by) if argform==1 else "pan(sndaztospkr[i], &sndaztospkr[i][%s - 1], %s, %s);"%(N,bx,by)
            return ''.join(I+z+'\n' for z in d)+I+call+'\n'
        nb1=mk(b1x,b1y,'after','before')
        # for b1, before = b1y, after = b1x
        if argform: nb1=nb1.replace("int before = %s;"%b1y,"int before = %s;"%b1y)
        nb2=mk(b2x,b2y,'before','after')
        t=base.replace(B1,nb1).replace(B2,nb2)
        t=t.replace("void SNDI_precalcaztospkrvol() {",H+"\nvoid SNDI_precalcaztospkrvol() {")
        r=score.real('config/GMFE69/satospkr.json',t,'SNDI_precalcaztospkrvol__Fv','8015f538','708')
        s=9999 if r is None else r[0]+abs(r[1])*5
        fn='scratch/loop3/late2c/v/pch_%d%d%d.c'%(hi,argform,order); open(fn,'w').write(t)
        res.append((s,fn)); print(s,fn,flush=True)
