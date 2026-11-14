from sv import *
import itertools
B=open('sx29.cpp').read()
PH="""static inline void Pairs(dwi::fast_vec<ISceneNode*>& active, ISceneNode* sceneNode, dwi::fast_vec<CScene::UNodePair>& pairs,
                         dwi::fast_vec<CScene::SExclusionEntry>& exclusions) {
    for (ISceneNode** it = active.begin(); it != active.end(); it++) {
"""
NPH="""static inline void Pairs(dwi::fast_vec<ISceneNode*>& active, ISceneNode* sceneNode, dwi::fast_vec<CScene::UNodePair>& pairs,
                         dwi::fast_vec<CScene::SExclusionEntry>& exclusions, ISceneNode**& it) {
    for (it = active.begin(); it != active.end(); it++) {
"""
assert PH in B
s0=B.index('static void SweepExtents('); s1=B.index('\n}\n',s0)+3
fn=B[s0:s1]
hdr=fn[:fn.index('    active.UseBuffer')]
body=fn[len(hdr):].replace("Pairs(active, sceneNode, *c_pairs, *c_excl);","Pairs(active, sceneNode, *c_pairs, *c_excl, it);")
assert 'it);' in body, body
lines=hdr.split('\n'); sig='\n'.join(lines[:2])+'\n'; decls=[l+'\n' for l in lines[2:] if l.strip()]
decls.append("    ISceneNode** it;\n")
res=[]
for perm in [ (0,)+p for p in itertools.permutations((1,2,3,4,5,6)) ]:
    s=B[:s0].replace(PH,NPH)+sig+''.join(decls[k] for k in perm)+body+B[s1:]
    r=score(s,'s7'); print(perm,r[:70],flush=True); res.append((int(r.split()[0]) if r[:1].isdigit() else 999,perm,r[:120]))
res.sort()
print(decls)
for r in res[:6]: print(r)
