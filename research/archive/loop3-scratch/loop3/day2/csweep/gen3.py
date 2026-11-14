import os,itertools
B=open('scratch/loop3/day2/csweep/base.cpp').read()
out='scratch/loop3/day2/csweep/v3'; os.makedirs(out,exist_ok=True)
C0='        int* count = mesh->m_matrixCount;\n'
U0='        DCStoreRangeNoSync(matrices, *count * 48);\n'
G0='        DMeshGeometryView* geometry = mesh->m_geometry;\n'
SK='        bool skinned = mesh->m_skin != 0;\n'
V={}
V['nocount']=B.replace(C0,'').replace(U0,'        DCStoreRangeNoSync(matrices, *mesh->m_matrixCount * 48);\n')
V['intcount']=B.replace(C0,'        int n = *mesh->m_matrixCount;\n').replace(U0,'        DCStoreRangeNoSync(matrices, n * 48);\n')
V['sizecount']=B.replace(C0,'        int n = *mesh->m_matrixCount * 48;\n').replace(U0,'        DCStoreRangeNoSync(matrices, n);\n')
V['nogeom']=B.replace(G0,'').replace('geometry->','mesh->m_geometry->')
V['count_after_sk']=B.replace(C0,'').replace(SK,SK+C0)
V['count_first']=B.replace(C0,'').replace('        mesh = tag->m_mesh;\n','        mesh = tag->m_mesh;\n'+C0)
V['sk_first']=B.replace(SK,'').replace('        mesh = tag->m_mesh;\n','        mesh = tag->m_mesh;\n'+SK)
V['sk_after_geom']=B.replace(SK,'').replace(G0,G0+SK)
V['sk_after_count']=B.replace(SK,'').replace(C0,C0+SK)
V['count_before_geom']=B.replace(C0,'').replace(G0,C0+G0)
V['nocount_skfirst']=V['nocount'].replace(SK,'').replace('        mesh = tag->m_mesh;\n','        mesh = tag->m_mesh;\n'+SK)
for k,v in V.items():
  assert v!=B,k
  open('%s/%s.cpp'%(out,k),'w').write(v)
