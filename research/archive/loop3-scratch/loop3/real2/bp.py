B=open('/home/kyle/workspace/moh-frontline/scratch/loop3/real2/bp_base.cpp').read()
s=B.index('    CVector3 a;\n    CVector3 b;\n    CVector3 c;\n'); e=B.index('    {\n        float distance = plane.Project2(corner, point);')
OLD=B[s:e]
ADD=('    void Negate(const CVector3& v) {','''    void Sub(const CVector3& a, const CVector3& b) { d.c.x = a.d.c.x - b.d.c.x; d.c.y = a.d.c.y - b.d.c.y; d.c.z = a.d.c.z - b.d.c.z; }
    void Add(const CVector3& o) { d.c.x += o.d.c.x; d.c.y += o.d.c.y; d.c.z += o.d.c.z; }
    void Negate(const CVector3& v) {''')
P2=('        out += p;','        out.Add(p);')
def mk(decl_first, neg_pos, corner_sub):
    lines=[]
    pre='    SClsnContact contact;\n    CVector3 point;\n'
    abc='    CVector3 a;\n    CVector3 b;\n    CVector3 c;\n'
    sc='    a.Scale(m_axes[0], m_half[0]);\n    b.Scale(m_axes[1], m_half[1]);\n    c.Scale(m_axes[2], m_half[2]);\n'
    if corner_sub=='inplace':
        cs='    CVector3 corner = m_center;\n    corner.Sub(corner, a);\n    corner.Sub(corner, b);\n    corner.Sub(corner, c);\n'
    else:
        cs='    CVector3 corner = m_center;\n    corner -= a;\n    corner -= b;\n    corner -= c;\n'
    neg='    contact.m_vector0.Negate(plane.m_normal);\n'
    body=(pre+abc if decl_first else abc+pre)+sc
    if neg_pos==0: body+=neg+cs
    else: body+=cs+neg
    body+='    contact.m_separation = plane.Project2(corner, point);\n    contact.m_vector1 = point;\n'
    body+='    corner.Add(a);\n    corner.Add(a);\n' if corner_sub=='inplace' else '    corner += a;\n    corner += a;\n'
    return body
V={'base':[]}
for df in (0,1):
  for np in (0,1):
    for cs in ('inplace','op'):
      V[f'd{df}n{np}{cs}']=[ADD,P2,(OLD,mk(df,np,cs))]
V['scaleonly']=[ADD]+[(f'    {v}.d.c.{c} = MulV(m_half[{i}], m_axes[{i}].d.c.{c});',f'    {v}.d.c.{c} = m_half[{i}] * m_axes[{i}].d.c.{c};') for i,v in enumerate('abc') for c in 'xyz']
F=[ADD,P2,(OLD,mk(0,0,'inplace'))]
V2={}
V2['full_mac']=F+[('    corner op v; \\\n    corner op v; \\\n','    corner.op; \\\n    corner.op; \\\n'),('STEP(b, +=)\n    STEP(a, -=)\n    STEP(c, +=)\n    STEP(b, -=)\n    STEP(a, +=)\n    STEP(b, +=)','STEP(b, Add(b))\n    STEP(a, Sub(corner, a))\n    STEP(c, Add(c))\n    STEP(b, Sub(corner, b))\n    STEP(a, Add(a))\n    STEP(b, Add(b))')]
V=V2
