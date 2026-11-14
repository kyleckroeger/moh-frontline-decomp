import itertools
O='''            x *= 1.0f / length;
            y *= 1.0f / length;
            z *= 1.0f / length;
'''
L=O.splitlines(True)
V=[]
for p in itertools.permutations(range(3)):
    if p!=(0,1,2): V.append(('N'+''.join(map(str,p)),[(O,''.join(L[i] for i in p))]))
V.append(('inv',[(O,'''            float inv = 1.0f / length;
            x *= inv;
            y *= inv;
            z *= inv;
''')]))
V.append(('opmul',[(O,'''            *this *= 1.0f / length;
''')]))
O2='    float Length() const { return sqrtf(x * x + y * y + z * z); }\n'
V.append(('lenyx',[(O2,'    float Length() const { return sqrtf(y * y + x * x + z * z); }\n')]))
V.append(('lenDot',[(O2,'    float Length() const { return sqrtf(Dot(*this)); }\n')]))
