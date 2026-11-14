import itertools
s=open('dc0.cpp').read()
A="""        kept = 0;
        for (i = 0; i < count; i++) {"""
B="""        located = 0;
        for (i = 0; i < kept; i++) {"""
assert A in s and B in s
As=[A,"        for (i = 0, kept = 0; i < count; i++) {","        for (kept = 0, i = 0; i < count; i++) {","        i = 0; kept = 0;\n        for (; i < count; i++) {", "        kept = i = 0;\n        for (; i < count; i++) {"]
Bs=[B,"        for (i = 0, located = 0; i < kept; i++) {","        for (located = 0, i = 0; i < kept; i++) {","        i = 0; located = 0;\n        for (; i < kept; i++) {"]
n=0
for a in range(len(As)):
  for b in range(len(Bs)):
    open('v/dc_%d_%d.cpp'%(a,b),'w').write(s.replace(A,As[a]).replace(B,Bs[b]))
