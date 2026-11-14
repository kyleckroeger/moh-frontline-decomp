ADD=('    void Negate(const CVector3& v) {','''    void Sub(const CVector3& a, const CVector3& b) { d.c.x = a.d.c.x - b.d.c.x; d.c.y = a.d.c.y - b.d.c.y; d.c.z = a.d.c.z - b.d.c.z; }
    void Add(const CVector3& o) { d.c.x += o.d.c.x; d.c.y += o.d.c.y; d.c.z += o.d.c.z; }
    void Abs(const CVector3& o) { d.c.x = (float)fabs(o.d.c.x); d.c.y = (float)fabs(o.d.c.y); d.c.z = (float)fabs(o.d.c.z); }
    void Negate(const CVector3& v) {''')
SUB=('''    diff.d.c.x = center.d.c.x - m_center.d.c.x;
    diff.d.c.y = center.d.c.y - m_center.d.c.y;
    diff.d.c.z = center.d.c.z - m_center.d.c.z;''','    diff.Sub(center, m_center);')
ADDC=('contact.m_vector1 += m_center;','contact.m_vector1.Add(m_center);')
NADD=[('normal += CVector3(s * d, 0.0f, 0.0f);','normal.Add(CVector3(s * d, 0.0f, 0.0f));'),('normal += CVector3(0.0f, s * d, 0.0f);','normal.Add(CVector3(0.0f, s * d, 0.0f));'),('normal += CVector3(0.0f, 0.0f, s * d);','normal.Add(CVector3(0.0f, 0.0f, s * d));')]
V={'base':[],'sub':[ADD,SUB],'addc':[ADD,ADDC],'nadd':[ADD]+NADD,'all3':[ADD,SUB,ADDC]+NADD}
B=V['all3']
L0='''    local.d.c.z = m_axes[2].Dot(diff);
    local.d.c.y = m_axes[1].Dot(diff);
    local.d.c.x = m_axes[0].Dot(diff);'''
L1='''    local.d.c.x = m_axes[0].Dot(diff);
    local.d.c.y = m_axes[1].Dot(diff);
    local.d.c.z = m_axes[2].Dot(diff);'''
A0='''    a0 = (float)fabs(local.d.c.x);
    a1 = (float)fabs(local.d.c.y);
    a2 = (float)fabs(local.d.c.z);'''
A1='''    CVector3 abs;
    abs.Abs(local);
    a0 = abs.d.c.x;
    a1 = abs.d.c.y;
    a2 = abs.d.c.z;'''
V={'xyz':B+[(L0,L1)],'abs':B+[(A0,A1)],'xyz_abs':B+[(L0,L1),(A0,A1)]}
import itertools
for p in itertools.permutations('xyz'):
    idx={'x':0,'y':1,'z':2}
    V['ord_'+''.join(p)]=B+[(L0,'\n'.join(f'    local.d.c.{c} = m_axes[{idx[c]}].Dot(diff);' for c in p))]
import re
def direct(pairs):
    return pairs
V={}
base_txt=open('bs/abs.cpp').read()
s=base_txt.replace('''    a0 = abs.d.c.x;
    a1 = abs.d.c.y;
    a2 = abs.d.c.z;
''','')
for decl in ['    float a1;\n','    float a2;\n','    float a0;\n']: s=s.replace(decl,'',1)
fn=s.index('int CVolBox::TestCollision(const CVolSphere')
s=s[:fn]+re.sub(r'\ba([012]) - m_half',lambda m:f'abs.d.c.{"xyz"[int(m.group(1))]} - m_half',s[fn:])
open('bs_direct.cpp','w').write(s)
V['self']=[]
