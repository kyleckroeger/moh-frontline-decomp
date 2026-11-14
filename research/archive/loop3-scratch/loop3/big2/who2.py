import sys,re
sys.path.insert(0,'scratch/loop3/big1'); import sc
CR={'352':'dz','328':'rx','336':'rz','348':'dy','344':'dx','332':'ry'}
def tab(m):
    out=[]
    for i in range(len(m)-15):
        seq=m[i:i+15]; d={}
        for x in seq:
            mm=re.match(r'lfs (f\d+),(\d+)\(r1\)',str(x))
            if mm and mm.group(2) in CR and CR[mm.group(2)] not in d: d[CR[mm.group(2)]]=mm.group(1)
        if len(d)==6 and str(m[i]).startswith('lfs'):
            out.append(' '.join('%s=%s'%(k,d[k]) for k in ['rx','ry','rz','dx','dy','dz'])); 
    return out
if __name__=='__main__':
    print('orig',tab(sc.orig()))
    for f in sys.argv[1:]:
        m=sc.asm(open(f).read()); print(f.split('/')[-1],sc.regions(m)[0],tab(m))
