K1=('''        kept = 0;
        for (i = 0; i < count; i++) {''')
K2=('''        located = 0;
        for (i = 0; i < kept; i++) {''')
k1={'k_a':'        for (kept = 0, i = 0; i < count; i++) {','k_b':'        for (i = 0, kept = 0; i < count; i++) {','k_c':'        i = 0;\n        kept = 0;\n        for (; i < count; i++) {'}
k2={'l_a':'        for (located = 0, i = 0; i < kept; i++) {','l_b':'        for (i = 0, located = 0; i < kept; i++) {','l_c':'        i = 0;\n        located = 0;\n        for (; i < kept; i++) {'}
V={}
for a,x in k1.items(): V[a]=[(K1,x)]
for a,x in k2.items(): V[a]=[(K2,x)]
