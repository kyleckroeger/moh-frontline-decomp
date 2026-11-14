from sv import *
import itertools
B=open('sx29.cpp').read()
s0=B.index('static inline bool IsExcluded'); s1=B.index('\n}\n',s0)+3
SIG="static inline bool IsExcluded(dwi::fast_vec<CScene::SExclusionEntry>& exclusions, ISceneNode* a, ISceneNode* b) {\n"
IFB="""    if (A < B) {
        lo = A;
        hi = B;
    } else {
        lo = B;
        hi = A;
    }
"""
TAIL="""    for (e = EX begin(); e != EX end(); e++) {
        if (e->Matches(lo, hi))
            break;
    }
    return e != EX end();
}
"""
parts={'L':"    ISceneNode* lo;\n",'H':"    ISceneNode* hi;\n",'E':"    CScene::SExclusionEntry* e;\n",
 'a':"    ISceneNode* const ca = a;\n",'b':"    ISceneNode* const cb = b;\n",'x':"    dwi::fast_vec<CScene::SExclusionEntry>* const cx = &exclusions;\n"}
res=[]
for k in (3,4,5):
  for perm in itertools.permutations('LHEab',k):
    if not set('LHE')<=set(perm): continue
    A='ca' if 'a' in perm else 'a'; Bv='cb' if 'b' in perm else 'b'; EX='cx->' if 'x' in perm else 'exclusions.'
    body=SIG+''.join(parts[p] for p in perm)+IFB.replace('A',A).replace('B',Bv)+TAIL.replace('EX ',EX)
    s=B[:s0]+body+B[s1:]
    r=score(s,'i5'); print(''.join(perm),r[:60],flush=True); res.append((int(r.split()[0]) if r[:1].isdigit() else 999,''.join(perm),r[:170]))
res.sort()
for r in res[:8]: print(r)
