"""geo.py: GetExtents statement-order search with the scheduler replay (ssim.py)."""
import itertools,sys,re,subprocess,json
import os; sys.path.insert(0,os.path.dirname(os.path.abspath(__file__))); import ssim
OFF={'w':4,'d':8,'h':12}
B={'x':(16,20,24),'y':(32,36,40),'z':(48,52,56)}
HALF={'x':'w','y':'d','z':'h'}
def prod(v): # v like 'xy' = halfWidth * basis[0].y
    i='xyz'.index(v[1]); return OFF[HALF[v[0]]], B[v[0]][i]
MIN=[('x',['xx','yx','zx']),('y',['xy','yy','zy']),('z',['xz','yz','zz'])]
def gen(order, minterms=MIN, maxterms=MIN):
    """instruction list: dict(op, defs, uses, mem: None|('L',off)|('S',base,off), sig)"""
    ins=[]; loaded={}; val={}; vr=[100]
    def new():
        vr[0]+=1; return vr[0]
    for v in order:
        a,b=prod(v); regs=[]
        for o in (a,b):
            if o not in loaded:
                r=new(); loaded[o]=r; ins.append(dict(op='LFS',defs=[r],uses=[],mem=('L',o),sig=('lfs',o)))
            regs.append(loaded[o])
        p=new(); ins.append(dict(op='FMULS',defs=[p],uses=regs,mem=None,sig=('fmuls',v)))
        q=new(); ins.append(dict(op='FABS',defs=[q],uses=[p],mem=None,sig=('fabs',v)))
        if SEP:
            q2=new(); ins.append(dict(op='FRSP',defs=[q2],uses=[q],mem=None,sig=('frsp',v))); val[v]=q2
        else:
            ins.append(dict(op='FRSP',defs=[q],uses=[q],mem=None,sig=('frsp',v))); val[v]=q
    for k,(st,terms,base) in enumerate([(s,t,4) for s,t in minterms]+[(s,t,5) for s,t in maxterms]):
        c=64+4*'xyz'.index(st); r=new(); ins.append(dict(op='LFS',defs=[r],uses=[],mem=('L',c),sig=('lfs',c)))
        for t in terms:
            n=new(); ins.append(dict(op='FSUBS' if base==4 else 'FADDS',defs=[n],uses=[r,val[t]],mem=None,sig=('fsubs' if base==4 else 'fadds',t))); r=n
        ins.append(dict(op='STFS',defs=[],uses=[r],mem=('S',base,c-64),sig=('stfs',base,c-64)))
    return ins
SEP=False
LAT={'LFS':2,'STFS':2,'FMULS':3,'FABS':3,'FRSP':3,'FSUBS':3,'FADDS':3}
def graph(ins):
    n=len(ins); succ=[dict() for _ in range(n)]
    def edge(i,j,l):
        if i==j: return
        succ[i][j]=max(succ[i].get(j,-1),l)
    for i in range(n):
        for j in range(i+1,n):
            a,b=ins[i],ins[j]
            if set(a['defs'])&set(b['uses']): edge(i,j,LAT[a['op']])      # RAW
            if set(a['uses'])&set(b['defs']): edge(i,j,0)                 # WAR
            if set(a['defs'])&set(b['defs']): edge(i,j,0)                 # WAW
            ma,mb=a['mem'],b['mem']
            if ma and mb and (ma[0]=='S' or mb[0]=='S'): edge(i,j,LAT[a['op']])
    nodes=[]
    for i,x in enumerate(ins):
        nd=ssim.Node(); nd.op=x['op']; nd.f4=False; nd.args=''; nd.lat=LAT[x['op']]
        nd.succ=sorted(succ[i].items()); nodes.append(nd)
    return nodes
def run(order,optab,**kw):
    ins=gen(order,**kw); nodes=graph(ins); out,h,sl=ssim.schedule(nodes,optab)
    return [ins[i]['sig'] for _,i in out],ins,nodes
