import itertools
src=open('fao0.cpp').read()
A='    CoverPointTypes& operator&=(const int& bits) { m_bits = bits & m_bits; return *this; }'
B='    CoverPointTypes& operator&=(const CoverPointTypes& o) { m_bits = o.m_bits & m_bits; return *this; }'
O='    CoverPointTypes operator|(const int& bits) const { CoverPointTypes t = *this; t.m_bits = t.m_bits | bits; return t; }'
T='''        types &= 0xff;
        types &= CoverPointTypes(1) | 2 | 4;'''
A1='    CoverPointTypes& operator&=(const int& bits) { int t = m_bits; t &= bits; m_bits = t; return *this; }'
B1='    CoverPointTypes& operator&=(const CoverPointTypes& o) { int t = m_bits; t &= o.m_bits; m_bits = t; return *this; }'
Os=[O,'    CoverPointTypes operator|(const int& bits) const { CoverPointTypes t = *this; t.m_bits = bits | t.m_bits; return t; }',
 '    CoverPointTypes operator|(const int& bits) const { CoverPointTypes t = *this; t.m_bits |= bits; return t; }',
 '    CoverPointTypes operator|(const int& bits) const { CoverPointTypes t; t.m_bits = m_bits; t.m_bits |= bits; return t; }',
 '    CoverPointTypes operator|(const int& bits) const { int v = m_bits; v |= bits; CoverPointTypes t; t.m_bits = v; return t; }',
 '    CoverPointTypes operator|(const int& bits) const { CoverPointTypes t(*this); t |= bits; return t; }',
 '    CoverPointTypes operator|(const int& bits) const { return CoverPointTypes(*this) |= bits; }',
]
Ts=[T,'''        CoverPointTypes mask = CoverPointTypes(1) | 2 | 4;
        types &= 0xff;
        types &= mask;''','''        types &= 0xff;
        CoverPointTypes mask = CoverPointTypes(1) | 2 | 4;
        types &= mask;''','''        types &= 0xff;
        types &= (CoverPointTypes(1) | 2) | 4;''','''        types &= 0xff;
        types &= CoverPointTypes(CoverPointTypes(1) | 2) | 4;''']
n=0
for k,t in itertools.product(range(len(Os)),range(len(Ts))):
    open(f'fv_{k}{t}.cpp','w').write(src.replace(A,A1).replace(B,B1).replace(O,Os[k]).replace(T,Ts[t])); n+=1
print(n)
