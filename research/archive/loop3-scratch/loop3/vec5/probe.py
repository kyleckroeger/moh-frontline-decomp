import sys,json,tempfile,re
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/research/tools')
import fast,varmap
T=json.load(open('/home/kyle/workspace/moh-frontline/config/GMFE69/particlesystem_visible.json'))
def run(src,fn='probe__Fv'):
    with tempfile.TemporaryDirectory() as w:
        obj,log=fast.compile_object(T,src,w)
        if not obj: print(log[-500:]); return None
        return varmap.rows(['--disassemble='+fn,obj])
N=17
def gen(declorder, useorder, loadorder=None):
    loadorder=loadorder or declorder
    s='extern float g[64];\nvoid probe() {\n'
    for i in declorder: s+='    float v%d;\n'%i
    for i in loadorder: s+='    v%d = g[%d];\n'%(i,i)
    s+='    float s = 0.0f;\n'
    for i in useorder: s+='    s = s * v%d;\n'%i  # not commutative-reassoc
    for i in range(N): s+='    g[%d] = v%d * s;\n'%(32+i,i)
    s+='}\n'
    return s
def regs(m):
    d={}
    for x in m:
        mm=re.match(r'lfs (f\d+),(-?\d+)\(r\d+\)',x)
        if mm and mm.group(2)!='0': d[int(mm.group(2))//4]=mm.group(1)
    return d
if __name__=='__main__':
    import random
    for name,do,uo,lo in [('id',list(range(N)),list(range(N)),None),('revdecl',list(range(N))[::-1],list(range(N)),list(range(N))),('revload',list(range(N)),list(range(N)),list(range(N))[::-1]),('revuse',list(range(N)),list(range(N))[::-1],None)]:
        m=run(gen(do,uo,lo))
        d=regs(m); print(name,' '.join('%d:%s'%(k,d[k]) for k in sorted(d)))
