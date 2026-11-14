B=open('ge_base.cpp').read()
s=B.index('void CVolBox::GetExtents'); e=B.index('}\n',s)+2
OLD=B[s:e]
def F(body): return [(OLD,'void CVolBox::GetExtents(CVector3& min, CVector3& max) const {\n'+body+'}\n')]
H=['m_halfWidth','m_halfDepth','m_halfHeight']; C='xyz'
def E(i,k,cast=''): return f'{cast}fabs({H[i]} * m_basis[{i}].d.c.{C[k]})'
V={}
for cast in ['','(float)']:
    body=''
    for k in range(3):
        body+=f'    min.d.c.{C[k]} = m_center.d.c.{C[k]} - '+' - '.join(E(i,k,cast) for i in range(3))+';\n'
    for k in range(3):
        body+=f'    max.d.c.{C[k]} = m_center.d.c.{C[k]} + '+' + '.join(E(i,k,cast) for i in range(3))+';\n'
    V['direct'+('c' if cast else '')]=F(body)
# float locals in component-major natural order
for order in ['xyz']:
    body='    float xx, xy, xz, yx, yy, yz, zx, zy, zz;\n'
    for k in range(3):
        for i in range(3):
            body+=f'    {"xyz"[i]}{C[k]} = {E(i,k)};\n'
    body+=OLD[OLD.index('    min.d.c.x'):OLD.rindex('}')]
    V['cm']=F(body)
    body='    float xx, xy, xz, yx, yy, yz, zx, zy, zz;\n'
    for i in range(3):
        for k in range(3):
            body+=f'    {"xyz"[i]}{C[k]} = {E(i,k)};\n'
    body+=OLD[OLD.index('    min.d.c.x'):OLD.rindex('}')]
    V['vm']=F(body)
