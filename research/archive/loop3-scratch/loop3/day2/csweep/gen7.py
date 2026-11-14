import os
B=open('scratch/loop3/day2/csweep/base.cpp').read()
out='scratch/loop3/day2/csweep/v7'; os.makedirs(out,exist_ok=True)
SKB='''        if (skinned)
            DCStoreRangeNoSync(mesh->m_skin, mesh->m_skinSize);
        if (skinned) {
            GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
            GXSetVtxDesc(GX_VA_NRM, GX_DIRECT);
        } else {
            GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
            GXSetVtxDesc(GX_VA_NRM, GX_INDEX16);
        }
'''
assert SKB in B
INL1='''static inline void SetupSkin(CDMeshLinkView* mesh, bool skinned) {
SKB}
'''.replace('SKB',SKB)
FN='void CPartBin::Link(CDmaTag* tag, CDmaPacket& packet) {'
V={}
V['inl_setup']=B.replace(SKB,'        SetupSkin(mesh, skinned);\n').replace(FN,INL1+FN)
INL2='''static inline void SetupSkin2(CDMeshLinkView* mesh, bool skinned, float (*matrices)[3][4], int* count) {
        DCStoreRangeNoSync(matrices, *count * 48);
SKB}
'''.replace('SKB',SKB)
V['inl_setup2']=B.replace('        DCStoreRangeNoSync(matrices, *count * 48);\n'+SKB,'        SetupSkin2(mesh, skinned, matrices, count);\n').replace(FN,INL2+FN)
# p loop inline
PL_start='        u32 p;\n'
i0=B.index(PL_start); i1=B.index('        tag = tag->m_next;\n')
PL=B[i0:i1]
INL3='static inline void DrawParts(CPartBin* bin, CDMeshLinkView* mesh, float (*matrices)[3][4], bool skinned, Mtx normal) {\n    int slot;\n'+PL.replace('this->','bin->')+'}\n'
V['inl_parts']=B.replace(PL,'        DrawParts(this, mesh, matrices, skinned, normal);\n').replace(FN,INL3+FN)
INL4=INL3.replace('Mtx normal) {','Mtx normal) {\n    Mtx n2;').replace('normal)','n2)').replace('(normal','(n2').replace(', normal,',', n2,')
for k,v in V.items():
  open('%s/%s.cpp'%(out,k),'w').write(v)
