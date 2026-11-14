import itertools
O='                up.Cross(right, dir);\n'
S=['                up.x = right.y * dir.z - right.z * dir.y;\n','                up.y = right.z * dir.x - right.x * dir.z;\n','                up.z = right.x * dir.y - right.y * dir.x;\n']
V=[]
for p in itertools.permutations(range(3)):
    V.append(('e'+''.join(map(str,p)),[(O,''.join(S[i] for i in p))]))
T=['                float ux = right.y * dir.z - right.z * dir.y;\n','                float uy = right.z * dir.x - right.x * dir.z;\n','                float uz = right.x * dir.y - right.y * dir.x;\n']
for p in itertools.permutations(range(3)):
    V.append(('t'+''.join(map(str,p)),[(O,''.join(T[i] for i in p)+'                up.x = ux;\n                up.y = uy;\n                up.z = uz;\n')]))
