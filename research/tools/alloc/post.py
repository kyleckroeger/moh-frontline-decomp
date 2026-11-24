"""post.py: build a post-RA dependency graph from real-register instruction list and replay ssim."""
import sys,re
sys.path.insert(0,__import__('os').path.dirname(__import__('os').path.abspath(__file__))); import ssim
LAT={'ADDI':1,'LIS':1,'RLWINM':1,'SUBF':1,'LWZ':2,'XORIS':1,'MULLW':5,'STW':2,'LFS':2,'LFD':2,'FMULS':3,'FSUBS':3,'MR':1,'BL':0,'LHA':2,'ADD':1,'NEG':1,'OR':1,'CMPI':3,'LI':1}
def ops(op,args):
    a=args.split(','); defs=set(); uses=set(); mem=None
    for x in a:
        m=re.match(r'^(=?)([rf]\d+)$',x)
        if m: (defs if m.group(1) else uses).add(m.group(2))
    if op in ('LWZ','LFS','LFD','STW','LHA'):
        if len(a)>2 and a[2].startswith('@'):
            mem=a[2]
            if a[1]=='r0': uses.discard('r0')
    if op in('STW',): mem=('S',mem)
    elif mem: mem=('L',mem)
    return defs,uses,mem
SZ={'LWZ':4,'LFS':4,'LFD':8,'STW':4,'LHA':2}
def ov(a,oa,b,ob):
    na,xa=a.split('+'); nb,xb=b.split('+')
    if na!=nb: return False
    xa=int(xa); xb=int(xb); return xa<xb+SZ[ob] and xb<xa+SZ[oa]
def graph(ins):
    """ins: list of (op,args,f4); returns ssim nodes"""
    n=len(ins); P=[ops(o,a) for o,a,_ in ins]; succ=[dict() for _ in range(n)]
    def e(i,j,l): succ[i][j]=max(succ[i].get(j,-1),l)
    for i in range(n):
        for j in range(i+1,n):
            di,ui,mi=P[i]; dj,uj,mj=P[j]
            if di&uj: e(i,j,LAT[ins[i][0]])
            if ui&dj: e(i,j,0)
            if di&dj: e(i,j,0)
            if mi and mj and (mi[0]=='S' or mj[0]=='S') and ov(mi[1],ins[i][0],mj[1],ins[j][0]): e(i,j,LAT[ins[i][0]])
    nodes=[]
    for i,(o,a,f4) in enumerate(ins):
        nd=ssim.Node(); nd.op=o; nd.f4=f4; nd.args=a; nd.lat=LAT[o]; nd.succ=sorted(succ[i].items()); nodes.append(nd)
    return nodes
def optab(path):
    return ssim.parse_optab(open(path).read().splitlines())
if __name__=='__main__':
    # validate against a dump block (start line) : python3 post.py dump line
    txt=open(sys.argv[1]).read().splitlines(); st=int(sys.argv[2])
    blk=[];
    for l in txt[st:]:
        if l.startswith('---') or l.startswith('==='): break
        blk.append(l)
    nodes,picks=ssim.parse(blk)
    ins=[(x.op,x.args,x.f4) for x in nodes]
    g=graph(ins[:-1])
    bad=0
    for i,x in enumerate(g):
        want=sorted(t for t in nodes[i].succ if t[0]!=len(nodes)-1)
        if want!=x.succ: bad+=1; print(i,ins[i],'\n want',want,'\n got ',x.succ)
    print('bad',bad)
