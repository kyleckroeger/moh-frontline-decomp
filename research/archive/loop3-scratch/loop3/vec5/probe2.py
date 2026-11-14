import sys,re
sys.path.insert(0,'.'); from probe import run
def gen(order):
    s='void probe(float* g, float* h) {\n'
    for i in order: s+='    float v%d = g[%d];\n'%(i,i)
    # use all: pairwise products so all live till end
    s+='    float s = v0;\n'
    for i in range(1,8): s+='    s = s * v%d;\n'%i
    for i in range(8): s+='    h[%d] = s - v%d;\n'%(i,i)
    s+='}\n'
    return s
for o in [list(range(8)),list(range(8))[::-1],[3,1,4,0,7,5,2,6]]:
    m=run(gen(o),'probe__FPfPf')
    d={}
    for x in m:
        mm=re.match(r'lfs (f\d+),(\d+)\(r3\)',x)
        if mm: d[int(mm.group(2))//4]=mm.group(1)
    print(o,' '.join('v%d:%s'%(k,d[k]) for k in sorted(d)))
