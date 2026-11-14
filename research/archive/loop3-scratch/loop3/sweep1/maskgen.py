import sys
src=open(sys.argv[1]).read(); pre=sys.argv[2]
O='    CoverPointTypes operator|(const int& bits) const { CoverPointTypes t; t.m_bits = m_bits | bits; return t; }'
if O not in src:
    O='    CoverPointTypes operator|(const int& bits) const { CoverPointTypes t = *this; t.m_bits = t.m_bits | bits; return t; }'
assert O in src
OCs={'r':'    CoverPointTypes operator|(const CoverPointTypes& o) const { CoverPointTypes t; t.m_bits = m_bits | o.m_bits; return t; }',
     'v':'    CoverPointTypes operator|(CoverPointTypes o) const { CoverPointTypes t; t.m_bits = m_bits | o.m_bits; return t; }'}
import re
T=re.search(r'        types &= 0xff;\n.*?return types;',src,re.S).group(0)
Ms={
'm1':'types &= 0xff;\n        CoverPointTypes mask = CoverPointTypes(1) | CoverPointTypes(2) | CoverPointTypes(4);\n        types &= mask;',
'm2':'types &= 0xff;\n        CoverPointTypes mask = CoverPointTypes(1) | 2 | CoverPointTypes(4);\n        types &= mask;',
'm3':'types &= 0xff;\n        CoverPointTypes a(1); CoverPointTypes b(2); CoverPointTypes c(4);\n        types &= a | b | c;',
'm4':'types &= 0xff;\n        CoverPointTypes c(4); CoverPointTypes b(2); CoverPointTypes a(1);\n        types &= a | b | c;',
'm5':'types &= 0xff;\n        CoverPointTypes a(1); CoverPointTypes b(2); CoverPointTypes c(4);\n        CoverPointTypes mask = a | b | c;\n        types &= mask;',
'm6':'types &= 0xff;\n        types &= CoverPointTypes(1) | CoverPointTypes(2) | CoverPointTypes(4);',
'm7':'CoverPointTypes mask = CoverPointTypes(1) | CoverPointTypes(2) | CoverPointTypes(4);\n        types &= 0xff;\n        types &= mask;',
}
for ok,oc in OCs.items():
  for mk,m in Ms.items():
    open(f'{pre}_{ok}{mk}.cpp','w').write(src.replace(O,O+'\n'+oc).replace(T,'        '+m+'\n        return types;'))
