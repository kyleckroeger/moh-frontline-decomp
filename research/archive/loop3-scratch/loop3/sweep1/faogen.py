import itertools
src=open('fao0.cpp').read()
A='    CoverPointTypes& operator&=(const int& bits) { m_bits = bits & m_bits; return *this; }'
B='    CoverPointTypes& operator&=(const CoverPointTypes& o) { m_bits = o.m_bits & m_bits; return *this; }'
O='    CoverPointTypes operator|(const int& bits) const { CoverPointTypes t = *this; t.m_bits = t.m_bits | bits; return t; }'
T='''        types &= 0xff;
        types &= CoverPointTypes(1) | 2 | 4;'''
for x in (A,B,O,T): assert x in src,x
As=[A,'    CoverPointTypes& operator&=(const int& bits) { int t = m_bits; t &= bits; m_bits = t; return *this; }',
      '    CoverPointTypes& operator&=(const int& bits) { int t = bits; t &= m_bits; m_bits = t; return *this; }',
      '    CoverPointTypes& operator&=(const int& bits) { int const b = bits; m_bits &= b; return *this; }']
Bs=[B,'    CoverPointTypes& operator&=(const CoverPointTypes& o) { int t = m_bits; t &= o.m_bits; m_bits = t; return *this; }',
      '    CoverPointTypes& operator&=(const CoverPointTypes& o) { int t = o.m_bits; t &= m_bits; m_bits = t; return *this; }',
      '    CoverPointTypes& operator&=(CoverPointTypes o) { m_bits &= o.m_bits; return *this; }']
Os=[O,'    CoverPointTypes operator|(const int& bits) const { CoverPointTypes t = *this; t.m_bits |= bits; return t; }',
      '    CoverPointTypes operator|(const int& bits) const { int v = m_bits; v |= bits; return CoverPointTypes(v); }',
      '    CoverPointTypes operator|(const int& bits) const { int v = bits; v |= m_bits; return CoverPointTypes(v); }']
n=0
for i,j,k in itertools.product(range(4),range(4),range(4)):
    open(f'fv_{i}{j}{k}.cpp','w').write(src.replace(A,As[i]).replace(B,Bs[j]).replace(O,Os[k])); n+=1
print(n)
