"""who.py var.py-file base.cpp [names...]: value->register tables for the cross and d<0 knots (big1)."""
import sys,re
sys.path.insert(0,'scratch/loop3/big1'); import sc
CR={'352':'dz','328':'rx','336':'rz','348':'dy','344':'dx','332':'ry'}
DL={'264':'Fx','268':'Fy','272':'Fz','280':'fx','284':'fy','288':'fz','344':'dx','348':'dy'}
def tab(m):
    out=[]
    seq=m[619:634]; d={}
    for x in seq:
        mm=re.match(r'lfs (f\d+),(\d+)\(r1\)',x)
        if mm and mm.group(2) in CR: d[CR[mm.group(2)]]=mm.group(1)
    out.append('X:'+' '.join('%s=%s'%(k,d.get(k,'?')) for k in ['rx','ry','rz','dx','dy','dz']))
    seq=m[489:513]; d={}
    for i,x in enumerate(seq):
        mm=re.match(r'lfs (f\d+),(\d+)\(r1\)',x)
        if mm and mm.group(2) in DL: d.setdefault(DL[mm.group(2)]+('2' if i>12 and DL[mm.group(2)][0]=='f' else ''),mm.group(1))
        if x.startswith('fmuls') and i in (14,): d['k']=x.split()[1].split(',')[0]
    out.append('D:'+' '.join('%s=%s'%(k,v) for k,v in sorted(d.items())))
    return '  '.join(out)
if __name__=='__main__':
    print('%-10s'%'orig',tab(sc.orig()))
    for f in sys.argv[1:]:
        m=sc.asm(open(f).read()); print('%-10s'%f.split('/')[-1][:10],*sc.regions(m)[:1],tab(m))
