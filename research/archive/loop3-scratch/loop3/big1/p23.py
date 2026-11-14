import itertools
O='                up.Cross(right, dir);\n'
names=[('qrx','right.x'),('qry','right.y'),('qrz','right.z'),('qdx','dir.x'),('qdy','dir.y'),('qdz','dir.z')]
V=[]
for p in itertools.permutations(range(6)):
    decl=''.join('                float %s = %s;\n'%names[i] for i in p)
    V.append(('L'+''.join(map(str,p)),[(O,decl+'''                up.x = qry * qdz - qrz * qdy;
                up.y = qrz * qdx - qrx * qdz;
                up.z = qrx * qdy - qry * qdx;
''')]))
