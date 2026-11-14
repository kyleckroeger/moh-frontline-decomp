import itertools
O='                up.Cross(right, dir);\n'
P=[('right.y','dir.z'),('right.z','dir.y'),('right.z','dir.x'),('right.x','dir.z'),('right.x','dir.y'),('right.y','dir.x')]
V=[]
for bits in itertools.product([0,1],repeat=6):
    pr=['%s * %s'%((a,b) if s==0 else (b,a)) for (a,b),s in zip(P,bits)]
    txt='                up.x = %s - %s;\n                up.y = %s - %s;\n                up.z = %s - %s;\n'%tuple(pr)
    V.append(('s'+''.join(map(str,bits)),[(O,txt)]))
