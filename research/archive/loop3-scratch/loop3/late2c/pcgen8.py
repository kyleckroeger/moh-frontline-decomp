import sys,os,itertools
os.chdir('/home/kyle/workspace/moh-frontline')
sys.path.insert(0,'research/tools'); sys.path.insert(0,'scratch/game')
import score
base=open('scratch/loop3/late2c/v/pch_200.c').read()
L=[l for l in base.split('\n') if 'pan(' in l and 'azimuth - az[' in l][0]
N='sndgs.speakers'; I=' '*16
X="azimuth - az[%s - 1]"%N; Y="0x10000 - azimuth + az[0]"
P0="&sndaztospkr[i][0]"; P1="&sndaztospkr[i][%s - 1]"%N
B={
'f1':["*q = shape(y * 127 / (x + y));","*p = shape(x * 127 / (x + y));"],
'f2':["int s = x + y;","*q = shape(y * 127 / s);","*p = shape(x * 127 / s);"],
'f3':["int b = y * 127 / (x + y);","int a = x * 127 / (x + y);","*q = shape(b);","*p = shape(a);"],
'f4':["int s = x + y;","int b = y * 127 / s;","int a = x * 127 / s;","*q = shape(b);","*p = shape(a);"],
'f5':["int a = x * 127 / (x + y);","int b = y * 127 / (x + y);","*q = shape(b);","*p = shape(a);"],
'f6':["int s = x + y;","int a = x * 127 / s;","int b = y * 127 / s;","*q = shape(b);","*p = shape(a);"],
'f7':["int s = x + y;","int a = x * 127 / s;","*q = shape(y * 127 / s);","*p = shape(a);"],
'f8':["int a = x * 127 / (x + y);","*q = shape(y * 127 / (x + y));","*p = shape(a);"],
}
res=[]
for k,body in B.items():
  for swap in (1,):
    H="static inline void pan2(signed char* p, signed char* q, int x, int y) {\n"+''.join('    %s\n'%z for z in body)+"}\n"
    call="pan2(%s, %s, %s, %s);"%(P1,P0,Y,X)
    t=base.replace(L,I+call).replace("\nvoid SNDI_precalcaztospkrvol() {","\n"+H+"\nvoid SNDI_precalcaztospkrvol() {")
    r=score.real('config/GMFE69/satospkr.json',t,'SNDI_precalcaztospkrvol__Fv','8015f538','708')
    s=9999 if r is None else r[0]+abs(r[1])*5
    fn='scratch/loop3/late2c/v/pc8_%s.c'%k; open(fn,'w').write(t); res.append((s,fn))
res.sort(); print(res)
