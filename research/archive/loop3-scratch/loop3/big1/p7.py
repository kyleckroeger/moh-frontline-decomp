import itertools
OLD='''        x = a.y * b.z - a.z * b.y;
        y = a.z * b.x - a.x * b.z;
        z = a.x * b.y - a.y * b.x;
'''
L=OLD.splitlines(True)
V=[]
for p in itertools.permutations(range(3)):
    V.append(('ord'+''.join(map(str,p)),[(OLD,''.join(L[i] for i in p))]))
V.append(('tmp',[(OLD,'''        float tx = a.y * b.z - a.z * b.y;
        float ty = a.z * b.x - a.x * b.z;
        float tz = a.x * b.y - a.y * b.x;
        x = tx;
        y = ty;
        z = tz;
''')]))
