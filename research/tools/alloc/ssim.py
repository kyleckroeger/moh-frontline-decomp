"""ssim.py dump.txt [line]: replay MWCC 1.3.2's list scheduler (Scheduler.c) with the gekko/750 machine model
(table 0x5d3a18: width 2; can_issue 0x578ec0, issue 0x578e30, advance 0x578aa0) on a sched.py dump; checks
every block of the dump (or the one starting at line) against the real picks."""
import re,sys
def parse_optab(txt):
    t={}
    for l in txt:
        if l.startswith('optab '):
            for x in l.split()[1:]:
                nm,i,b=x.split(':'); t[nm]=(int(i),bytes.fromhex(b))
    return t
class Node:
    pass
def regs(args):
    uses=set(); defs=set()
    for a in args.split(','):
        m=re.match(r'^(=?)r(\d+)$',a)
        if m:
            (defs if m.group(1) else uses).add(int(m.group(2)))
    return uses,defs
def depends(n,p):
    # 0x506d90: p's first operand is a GPR def; n has a GPR operand (use or def) with that number
    if p is None or not p.args: return False
    a0=p.args.split(',')[0]
    m=re.match(r'^=r(\d+)$',a0)
    if not m: return False
    r=int(m.group(1)); u,d=regs(n.args)
    return r in u or r in d
class M750:
    def __init__(s,optab): s.t=optab; s.slot=[None]*9; s.cd=[0]*9; s.cq=[]; s.dc=None; s.dd=None
    def tab(s,n): return s.t[n.op][1]
    def can(s,n):
        if len(s.cq)>=6: return False
        c=s.tab(n)[0]
        if c==2:
            f1=s.slot[1] is None; f2=s.slot[2] is None
            if not f1 and not f2: return False
            if f1 and f2: return True
            busy=s.slot[2] if f1 else s.slot[1]
            if depends(n,busy) or depends(n,s.dc) or depends(n,s.dd): return False
        elif s.slot[c] is not None: return False
        if n.f4 and s.slot[4] is not None and s.slot[4].f4: return False
        return True
    def issue(s,n):
        b=s.tab(n); c=b[0]; s.cq.append([n,False])
        if c==2 and s.slot[1] is None: c=1
        elif c==2: c=2
        s.slot[c]=n; s.cd[c]=b[2]
    def done(s,n):
        for e in s.cq:
            if e[0] is n: e[1]=True; return
    def adv(s):
        s.dc=None; s.dd=None
        for i in range(9):
            if s.slot[i] is not None and s.cd[i]: s.cd[i]-=1
        for _ in range(2):
            if s.cq and s.cq[0][1]: s.cq.pop(0)
            else: break
        def fin(i):
            n=s.slot[i]; s.done(n); s.slot[i]=None; return n
        if s.slot[1] is not None and s.cd[1]==0: s.dc=fin(1)
        for i in (4,7,8,0):
            if s.slot[i] is not None and s.cd[i]==0: fin(i)
        if s.slot[2] is not None and s.cd[2]==0: s.dd=fin(2)
        if s.slot[5] is not None and s.cd[5]==0 and s.t[s.slot[5].op][0] in (0xa8,0xa9): fin(5)
        for a,b,k in ((6,7,4),(5,6,3),(3,4,3)):
            if s.slot[a] is not None and s.cd[a]==0 and s.slot[b] is None:
                n=s.slot[a]; s.slot[a]=None; s.slot[b]=n; s.cd[b]=s.tab(n)[k]
def schedule(nodes, optab, width=2, prera=True):
    n=len(nodes); h=[0]*n
    for i in range(n-1,-1,-1):
        h[i]=max([nodes[i].lat]+[l+h[j] for j,l in nodes[i].succ])
    H=max(h); slack=[H-x for x in h]
    np_=[0]*n
    for i in range(n):
        for j,l in nodes[i].succ: np_[j]+=1
    m=M750(optab); early=[0]*n; lst=list(range(n)); out=[]; cyc=0
    while lst:
        cnt=0
        while cnt<width and lst:
            def ready(i): return np_[i]==0 and early[i]<=cyc and m.can(nodes[i])
            best=None
            for k,i in enumerate(lst):
                if ready(i): best=i; start=k; break
            if best is None: break
            for i in lst[start+1:]:
                if not ready(i): continue
                if not (cyc<slack[best] or slack[i]<=cyc): continue
                if cyc<slack[best] and slack[i]<=cyc: best=i; continue
                cc=sum(1 for j,_ in nodes[i].succ if np_[j]==1); cb=sum(1 for j,_ in nodes[best].succ if np_[j]==1)
                if cb<cc: best=i
                elif cb==cc and h[best]<h[i]: best=i
                elif prera and cb==cc and h[best]==h[i] and optab[nodes[i].op][1][6]<optab[nodes[best].op][1][6]: best=i
            for j,l in nodes[best].succ:
                np_[j]-=1; early[j]=max(early[j],cyc+l)
            m.issue(nodes[best]); lst.remove(best); out.append((cyc,best)); cnt+=1
        m.adv(); cyc+=1
        if cyc>10000: raise Exception('loop')
    return out,h,slack
RX=re.compile(r'\s+n(\d+)\s+lat=(\d+) slack=(\d+) h=(\d+) np=(\d+)\s+(\S+?)(!?) (.*?)\s+->\s*(.*)$')
def parse(lines):
    nodes=[]; picks=[]
    for l in lines:
        m=RX.match(l)
        if m:
            x=Node(); x.op=m.group(6); x.f4=bool(m.group(7)); x.args=m.group(8).strip(); x.lat=int(m.group(2))
            x.succ=[tuple(map(int,y.split('/'))) for y in m.group(9).split()]; x.h=int(m.group(4)); x.slack=int(m.group(3))
            nodes.append(x)
        m=re.match(r'\s+pick c(\d+) n(-?\d+)',l)
        if m and int(m.group(2))>=0: picks.append((int(m.group(1)),int(m.group(2))))
    return nodes,picks
def blocks(txt):
    cur=None; pre=True
    for i,l in enumerate(txt):
        if l.startswith('=== PASS'): pre='pre-RA' in l
        if l.startswith('machine') or l.startswith('flag5e'): continue
        if l.startswith('---') or l.startswith('===') or l.startswith('optab'):
            if cur: yield cur
            cur=[i,[],pre] if l.startswith('---') else None
        elif cur is not None: cur[1].append(l)
    if cur: yield cur
if __name__=='__main__':
    txt=open(sys.argv[1]).read().split('\n'); optab=parse_optab(txt)
    ok=bad=0
    for start,blk,pre in blocks(txt):
        if len(sys.argv)>2 and start!=int(sys.argv[2]): continue
        nodes,picks=parse(blk)
        if not nodes: continue
        out,h,sl=schedule(nodes,optab,prera=pre)
        if out==picks: ok+=1
        else:
            bad+=1
            d=next((a,b) for a,b in zip(out+[None]*99,picks+[None]*99) if a!=b)
            print('block at line',start+1,'first diff sim',d[0],'real',d[1])
    print('ok',ok,'bad',bad)
