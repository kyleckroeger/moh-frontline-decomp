import itertools
OLD='''        x = a.y * b.z - a.z * b.y;
        y = a.z * b.x - a.x * b.z;
        z = a.x * b.y - a.y * b.x;
'''
V=[]
names=['ax','ay','az','bx','by','bz']
for p in itertools.permutations(range(6)):
    decl=''.join('        float %s = %s.%s;\n'%(names[i],names[i][0],names[i][1]) for i in p)
    V.append(('L'+''.join(map(str,p)),[(OLD,decl+'''        x = ay * bz - az * by;
        y = az * bx - ax * bz;
        z = ax * by - ay * bx;
''')]))
