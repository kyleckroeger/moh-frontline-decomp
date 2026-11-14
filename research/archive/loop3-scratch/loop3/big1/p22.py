import itertools
O='''        x = a.y * b.z - a.z * b.y;
        y = a.z * b.x - a.x * b.z;
        z = a.x * b.y - a.y * b.x;
'''
P=[('a.y','b.z'),('a.z','b.y'),('a.z','b.x'),('a.x','b.z'),('a.x','b.y'),('a.y','b.x')]
V=[]
for bits in itertools.product([0,1],repeat=6):
    pr=['%s * %s'%((a,b) if s==0 else (b,a)) for (a,b),s in zip(P,bits)]
    txt='        x = %s - %s;\n        y = %s - %s;\n        z = %s - %s;\n'%tuple(pr)
    V.append(('c'+''.join(map(str,bits)),[(O,txt)]))
