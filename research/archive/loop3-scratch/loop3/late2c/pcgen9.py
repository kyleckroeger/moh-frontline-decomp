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
H="static inline void pan3(signed char* p, signed char* q, int x, int y, int t) {\n    int a = x * 127 / t;\n    int b = y * 127 / t;\n    *p = shape(a);\n    *q = shape(b);\n}\n"
H2="static inline void pan3(signed char* p, signed char* q, int x, int y, int t) {\n    *p = shape(x * 127 / t);\n    *q = shape(y * 127 / t);\n}\n"
calls={
 'c1':["int before = %s;"%X,"int after = %s;"%Y,"pan3(%s, %s, before, after, after + before);"%(P0,P1)],
 'c2':["int after = %s;"%Y,"int before = %s;"%X,"pan3(%s, %s, before, after, after + before);"%(P0,P1)],
 'c3':["int before = %s;"%X,"int after = %s;"%Y,"pan3(%s, %s, before, after, before + after);"%(P0,P1)],
 'c4':["int before = %s;"%X,"int after = %s;"%Y,"int sum = after + before;","pan3(%s, %s, before, after, sum);"%(P0,P1)],
}
for hn,h in (('h1',H),('h2',H2)):
  for k,c in calls.items():
    t=base.replace(L,'\n'.join(I+z for z in c)).replace("\nvoid SNDI_precalcaztospkrvol() {","\n"+h+"\nvoid SNDI_precalcaztospkrvol() {")
    r=score.real('config/GMFE69/satospkr.json',t,'SNDI_precalcaztospkrvol__Fv','8015f538','708')
    s=9999 if r is None else r[0]+abs(r[1])*5
    fn='scratch/loop3/late2c/v/pc9_%s%s.c'%(hn,k); open(fn,'w').write(t); res.append((s,fn))
res.sort(); print(res)
