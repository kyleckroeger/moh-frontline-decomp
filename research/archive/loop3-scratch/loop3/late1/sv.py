import subprocess,os,itertools,sys
os.environ['T']='/home/kyle/workspace/moh-frontline/config/GMFE69/scene_cmppairs.json'
F='SweepExtents__FRQ23dwi28list<Q210ISceneNode7SExtent>RQ23dwi29fast_vec<Q26CScene9UNodePair>RQ23dwi36fast_vec<Q26CScene15SExclusionEntry>'
base=open('sx.cpp').read()
i=base.index('static void SweepExtents('); j=base.index('\n}\n',i)+3
def score(src,tag):
    open(f's_{tag}.cpp','w').write(src)
    return subprocess.run(['python3','rm.py',f's_{tag}.cpp',F],capture_output=True,text=True).stdout.strip()
SIG="""static void SweepExtents(dwi::list<ISceneNode::SExtent>& extents, dwi::fast_vec<CScene::UNodePair>& pairs,
                         dwi::fast_vec<CScene::SExclusionEntry>& exclusions) {
"""
D={'A':"    dwi::fast_vec<ISceneNode*> active;\n",'N':"    dwi::list_node<ISceneNode::SExtent>* node;\n",'S':"    ISceneNode* sceneNode;\n",
   'E':"    dwi::list<ISceneNode::SExtent>* const c_extents = &extents;\n",
   'P':"    dwi::fast_vec<CScene::UNodePair>* const c_pairs = &pairs;\n",
   'X':"    dwi::fast_vec<CScene::SExclusionEntry>* const c_excl = &exclusions;\n"}
BODY="""    active.UseBuffer(SPR_BEGIN, 250);
    for (node = EXT.m_head.m_next; node != &EXT.m_head; node = node->m_next) {
        sceneNode = node->m_data.node;
        if (sceneNode->GetCollisionId() == -1)
            continue;
        if (node->m_data.start) {
            Pairs(active, sceneNode, PAI, EXC);
            active.push_back(sceneNode);
        } else {
            active.erase(active.find(sceneNode));
        }
    }
}
"""
def gen(perm):
    b=BODY.replace('EXT','(*c_extents)' if 'E' in perm else 'extents').replace('PAI','*c_pairs' if 'P' in perm else 'pairs').replace('EXC','*c_excl' if 'X' in perm else 'exclusions')
    return base[:i]+SIG+''.join(D[p] for p in perm)+b+base[j:]
if __name__=='__main__':
    res=[]
    keys=sys.argv[1]
    for perm in itertools.permutations(keys):
        r=score(gen(perm),'sv'); res.append((int(r.split()[0]),''.join(perm),r[:150]))
    res.sort()
    for r in res[:10]: print(r)
