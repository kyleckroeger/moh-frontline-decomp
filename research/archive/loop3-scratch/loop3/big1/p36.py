import itertools,random
random.seed(1)
O='                up.Cross(right, dir);\n'
names=[('qrx','right.x'),('qry','right.y'),('qrz','right.z'),('qdx','dir.x'),('qdy','dir.y'),('qdz','dir.z')]
expr='''                up.x = qry * qdz - qrz * qdy;
                up.y = qrz * qdx - qrx * qdz;
                up.z = qrx * qdy - qry * qdx;
'''
cands=[]
for k in range(1,4):
    for S in itertools.combinations(range(6),k):
        for p in itertools.permutations(S): cands.append(p)

V=[]
for p in cands:
    decl=''.join('                float %s = %s;\n'%names[i] for i in p)
    e=expr
    for i in range(6):
        if i not in p: e=e.replace(names[i][0],names[i][1])
    V.append(('S'+''.join(map(str,p)),[(O,decl+e)]))
