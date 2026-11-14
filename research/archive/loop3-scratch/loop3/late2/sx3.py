from sxv import *
S=open('sx/it_prex_3.cpp').read()
a=S.index('static void SweepExtents('); b=S.index('\n}\n',a)+3
fn=S[a:b]
V={
 'noext': fn.replace('    dwi::list<ISceneNode::SExtent>* const c_extents = &extents;\n',''),
 'nopairs': fn.replace('    dwi::fast_vec<CScene::UNodePair>* const c_pairs = &pairs;\n','').replace('c_pairs->','pairs.'),
 'noexcl': fn.replace('    dwi::fast_vec<CScene::SExclusionEntry>* const c_excl = &exclusions;\n','').replace('*c_excl','exclusions'),
}
V['none']=V['noext']
for k in ['nopairs','noexcl']:
  pass
n=V['noext'].replace('    dwi::fast_vec<CScene::UNodePair>* const c_pairs = &pairs;\n','').replace('c_pairs->','pairs.').replace('    dwi::fast_vec<CScene::SExclusionEntry>* const c_excl = &exclusions;\n','').replace('*c_excl','exclusions')
V['none']=n
V['pairsinl']=None
for k,v in V.items():
  if v is None: continue
  print(k,score(S[:a]+v+S[b:],'c_'+k),flush=True)
print(fn)
