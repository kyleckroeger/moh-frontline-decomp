exec(open('ge_o.py').read().split("BODY='''")[0])
MM='''    min.d.c.x = m_center.d.c.x - a.d.c.x - b.d.c.x - c.d.c.x;
    min.d.c.y = m_center.d.c.y - a.d.c.y - b.d.c.y - c.d.c.y;
    min.d.c.z = m_center.d.c.z - a.d.c.z - b.d.c.z - c.d.c.z;
    max.d.c.x = m_center.d.c.x + a.d.c.x + b.d.c.x + c.d.c.x;
    max.d.c.y = m_center.d.c.y + a.d.c.y + b.d.c.y + c.d.c.y;
    max.d.c.z = m_center.d.c.z + a.d.c.z + b.d.c.z + c.d.c.z;
'''
B3=[('m_basis[0]','m_halfWidth','a'),('m_basis[1]','m_halfDepth','b'),('m_basis[2]','m_halfHeight','c')]
forms={}
forms['tmp1']='    CVector3 t;\n    CVector3 a;\n    CVector3 b;\n    CVector3 c;\n'+''.join(f'    t.Scale({m}, {h});\n    {v}.Abs(t);\n' for m,h,v in B3)
forms['tmp1b']='    CVector3 a;\n    CVector3 b;\n    CVector3 c;\n    CVector3 t;\n'+''.join(f'    t.Scale({m}, {h});\n    {v}.Abs(t);\n' for m,h,v in B3)
forms['tmp3']='    CVector3 ta;\n    CVector3 tb;\n    CVector3 tc;\n    CVector3 a;\n    CVector3 b;\n    CVector3 c;\n'+''.join(f'    t{v}.Scale({m}, {h});\n' for m,h,v in B3)+''.join(f'    {v}.Abs(t{v});\n' for m,h,v in B3)
forms['tmp3i']='    CVector3 ta;\n    CVector3 tb;\n    CVector3 tc;\n    CVector3 a;\n    CVector3 b;\n    CVector3 c;\n'+''.join(f'    t{v}.Scale({m}, {h});\n    {v}.Abs(t{v});\n' for m,h,v in B3)
forms['inplace_i']='    CVector3 a;\n    CVector3 b;\n    CVector3 c;\n'+''.join(f'    {v}.Scale({m}, {h});\n    {v}.Abs({v});\n' for m,h,v in B3)
forms['abs1']='    CVector3 a;\n    CVector3 b;\n    CVector3 c;\n'+''.join(f'    {v}.Scale({m}, {h});\n' for m,h,v in B3)+''.join(f'    {v}.Abs({v});\n' for m,h,v in B3)
V={}
for k,f in forms.items():
    V[k]=PRE+[(OLD,'void CVolBox::GetExtents(CVector3& min, CVector3& max) const {\n'+f+MM+'}\n')]
