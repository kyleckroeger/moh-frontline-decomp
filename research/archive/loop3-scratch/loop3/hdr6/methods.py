import re,sys,json
sys.path.insert(0,'research/tools'); sys.path.insert(0,'scratch/loop3/hdr6')
from try_header import block_end
from survey import ns_blocks, src2u
from collections import defaultdict
want=sys.argv[1]
groups=defaultdict(lambda: defaultdict(list))
for src in sorted(src2u):
    try: t=open(src,errors='replace').read()
    except: continue
    for a,b in ns_blocks(t):
        blk=t[a:b]
        for m in re.finditer(r'^(template\s*<[^>]*>\s*)?(class|struct)\s+(\w+)\b[^;{]*\{',blk,re.M):
            if m[3]!=want: continue
            e=block_end(blk,m.end()-1); body=blk[m.end():e-1]
            # member functions with bodies
            for f in re.finditer(r'^[ \t]+((?:template\s*<[^>]*>\s*)?(?:static\s+|inline\s+)*[\w:<>&\*\s,]*?(~?\w+|operator\S+?)\s*\(([^;{]*)\)\s*(?:const\s*)?(?::[^{;]*)?)\{',body,re.M):
                fe=block_end(body,f.end()-1)
                txt=' '.join(body[f.start():fe].split())
                groups[(f[2],len([p for p in f[3].split(',') if p.strip()]))][txt].append(src2u[src][0])
            for f in re.finditer(r'^[ \t]+([^{};]*?(~?\w+|operator\S+?)\s*\(([^;{]*)\)\s*(?:const\s*)?);',body,re.M):
                groups[(f[2],-1)]['DECL '+' '.join(f[1].split())].append(src2u[src][0])
for k in sorted(groups):
    print('##',k)
    for txt,us in sorted(groups[k].items(),key=lambda x:-len(x[1])):
        print(f'  [{len(us)}] {txt[:400]}  <- {" ".join(us[:6])}')
