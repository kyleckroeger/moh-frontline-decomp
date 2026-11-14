import re,sys,random
sys.argv=[sys.argv[0]]+sys.argv[1:]
exec(open('hc_rs.py').read().split("src=open(sys.argv[1]).read()")[0])
src=open(sys.argv[1]).read()
a=src.index('    float spin = '); b=src.index('    CMatrix toCamera(')
pre,blk,post=src[:a],src[a:b],src[b:]
lines=[l for l in blk.split('\n') if l.strip()]
ID=re.compile(r'[A-Za-z_]\w*')
def du(l):
    lhs,_,rhs=l.partition('=') if re.search(r'[^=!<>]=[^=]',l) else ('',None,l)
    m=re.match(r'\s*(\w+)\.(Sub)\((.*)\);',l)
    if m: return {m.group(1)},set(ID.findall(m.group(3)))
    lw=ID.findall(lhs)
    d={lw[-1]} if lw else set()
    u=set(ID.findall(rhs))
    if re.search(r'[*+-]=',l): u|=d
    return d,u
def ok(x,y):
    dx,ux=du(x); dy,uy=du(y)
    if dx&uy or dy&ux: return False
    if dx&dy: return False
    return True
def build(ls): return pre+'\n'.join(ls)+'\n\n'+post
best=sc(build(lines)); print('base',best,flush=True)
for it in range(int(sys.argv[2])):
    imp=False
    for i in random.sample(range(len(lines)-1),len(lines)-1):
        if not ok(lines[i],lines[i+1]): continue
        ls=lines[:]; ls[i],ls[i+1]=ls[i+1],ls[i]
        r=sc(build(ls))
        if r<best:
            best,lines=r,ls; imp=True; print('it',it,i,best,flush=True)
            open('rs_ord_best.cpp','w').write(build(lines))
    if not imp: break
print('final',best)
