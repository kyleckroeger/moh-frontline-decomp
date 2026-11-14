import sys,os
os.chdir('/home/kyle/workspace/moh-frontline')
sys.path.insert(0,'research/tools'); sys.path.insert(0,'scratch/game')
import score
base=open('scratch/loop3/late2c/v/pch_200.c').read()
L=[l for l in base.split('\n') if 'pan(' in l and 'azimuth - az[' in l][0]
N='sndgs.speakers'; I=' '*16
X="azimuth - az[%s - 1]"%N; Y="0x10000 - azimuth + az[0]"
Ys=[Y,"az[0] + (0x10000 - azimuth)","az[0] - azimuth + 0x10000","0x10000 + az[0] - azimuth","(0x10000 - azimuth) + az[0]"]
P0="&sndaztospkr[i][0]"; P1="&sndaztospkr[i][%s - 1]"%N
V={}
for j,y in enumerate(Ys):
    V['a%d'%j]=I+"pan(%s, %s, %s, %s);"%(P0,P1,X,y)
    V['s%d'%j]=I+"pan(%s, %s, %s, %s);"%(P1,P0,y,X)
V['loc1']=I+"int before = %s;\n%sint after = %s;\n%span(%s, %s, before, after);"%(X,I,Y,I,P0,P1)
V['loc2']=I+"int after = %s;\n%sint before = %s;\n%span(%s, %s, before, after);"%(Y,I,X,I,P0,P1)
V['loc3']=I+"int before = %s;\n%sint after = %s;\n%span(%s, %s, after, before);"%(X,I,Y,I,P1,P0)
V['loc4']=I+"int after = %s;\n%sint before = %s;\n%span(%s, %s, after, before);"%(Y,I,X,I,P1,P0)
for k,v in V.items():
    t=base.replace(L,v)
    r=score.real('config/GMFE69/satospkr.json',t,'SNDI_precalcaztospkrvol__Fv','8015f538','708')
    s=9999 if r is None else r[0]+abs(r[1])*5
    fn='scratch/loop3/late2c/v/pc5_%s.c'%k; open(fn,'w').write(t); print(s,fn,flush=True)
