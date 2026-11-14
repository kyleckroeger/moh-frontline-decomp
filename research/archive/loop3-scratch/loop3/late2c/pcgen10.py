import sys,os,itertools
os.chdir('/home/kyle/workspace/moh-frontline')
sys.path.insert(0,'research/tools'); sys.path.insert(0,'scratch/game')
import score
base=open('scratch/loop3/late2c/v/pch_200.c').read()
L=[l for l in base.split('\n') if 'pan(' in l and 'azimuth - az[' in l][0]
N='sndgs.speakers'; I=' '*16
X="azimuth - az[%s - 1]"%N; Y="0x10000 - azimuth + az[0]"
res=[]
for ca,cb,cbef,caft,ba,den,ab in itertools.product(range(2),range(2),range(2),range(2),range(2),range(2),range(2)):
    q=lambda c:'int const' if c else 'int'
    d=["%s before = %s;"%(q(cbef),X),"%s after = %s;"%(q(caft),Y)]
    if ba: d=d[::-1]
    dn=["(after + before)","(before + after)"][den]
    c=["%s a = before * 127 / %s;"%(q(ca),dn),"%s b = after * 127 / %s;"%(q(cb),dn)]
    if ab: c=c[::-1]
    body=d+c+["sndaztospkr[i][0] = shape(a);","sndaztospkr[i][%s - 1] = shape(b);"%N]
    t=base.replace(L,'\n'.join(I+z for z in body))
    r=score.real('config/GMFE69/satospkr.json',t,'SNDI_precalcaztospkrvol__Fv','8015f538','708')
    s=9999 if r is None else r[0]+abs(r[1])*5
    fn='scratch/loop3/late2c/v/pc10_%d%d%d%d%d%d%d.c'%(ca,cb,cbef,caft,ba,den,ab)
    if s<7: open(fn,'w').write(t)
    res.append((s,fn))
res.sort(); print(res[:8])
