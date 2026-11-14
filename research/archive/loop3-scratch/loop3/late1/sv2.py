from sv import *
import itertools
B=gen('AEPSNX')
PH="""static inline void Pairs(dwi::fast_vec<ISceneNode*>& active, ISceneNode* sceneNode, dwi::fast_vec<CScene::UNodePair>& pairs,
                         dwi::fast_vec<CScene::SExclusionEntry>& exclusions) {
    for (ISceneNode** it = active.begin(); it != active.end(); it++) {
        ISceneNode* other;
"""
assert PH in B
SIGP=PH[:PH.index('{\n')+2]
parts={'I':"    ISceneNode** it;\n",'O':"    ISceneNode* other;\n",'S':"    ISceneNode* const c_node = sceneNode;\n",'A':"    dwi::fast_vec<ISceneNode*>* const c_active = &active;\n"}
res=[]
for k in (2,3,4):
  for perm in itertools.permutations('IOSA',k):
    if 'I' not in perm or 'O' not in perm: continue
    act='c_active->' if 'A' in perm else 'active.'
    h=SIGP+''.join(parts[p] for p in perm)+f"    for (it = {act}begin(); it != {act}end(); it++) {{\n"
    s=B.replace(PH,h)
    if 'S' in perm:
        pi=s.index(h); pe=s.index('\n}\n',pi)
        s=s[:pi+len(h)]+s[pi+len(h):pe].replace('sceneNode','c_node')+s[pe:]
    r=score(s,'p2'); res.append((int(r.split()[0]) if r[:1].isdigit() else 999,''.join(perm),r[:150]))
res.sort()
for r in res[:8]: print(r)
