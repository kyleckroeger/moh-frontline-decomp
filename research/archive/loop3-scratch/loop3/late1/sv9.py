from sv import *
B=open('sx50.cpp').read()
P0=B.index('static inline void Pairs('); P1=B.index('\n}\n',P0)+3
pairs_body=B[P0:P1]
inner=pairs_body[pairs_body.index('    for (it ='):pairs_body.rindex('}')]
inl=inner.replace('active.','active.').replace('    ','            ',0)
# indent inner by 8 extra
inl=''.join('        '+l+'\n' if l else '\n' for l in inner.rstrip('\n').split('\n')).replace('pairs.push_back','c_pairs->push_back').replace('IsExcluded(exclusions,','IsExcluded(*c_excl,')
CALL="            Pairs(active, sceneNode, *c_pairs, *c_excl, it);\n"
assert CALL in B
V={
 'inl':B.replace(CALL,inl),
 'late':B.replace("    for (node = (*c_extents).m_head.m_next;","    node = (*c_extents).m_head.m_next;\n    for (;"),
 'late2':B.replace("    active.UseBuffer(SPR_BEGIN, 250);\n    for (node = (*c_extents).m_head.m_next;","    node = (*c_extents).m_head.m_next;\n    active.UseBuffer(SPR_BEGIN, 250);\n    for (;"),
}
for n,s in V.items(): print(n,score(s,'v9'+n)[:130])
