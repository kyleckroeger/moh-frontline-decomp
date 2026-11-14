import sys,itertools
sys.path.insert(0,'scratch/loop3/big1'); import sc,who
base=open('scratch/loop3/big2/base.cpp').read()
O='                up.Cross(right, dir);\n'
names=[('qrx','right.x'),('qry','right.y'),('qrz','right.z'),('qdx','dir.x'),('qdy','dir.y'),('qdz','dir.z')]
for p in [tuple(map(int,a)) for a in sys.argv[1:]]:
    decl=''.join('                float %s = %s;\n'%names[i] for i in p)
    t=base.replace(O,decl+'''                up.x = qry * qdz - qrz * qdy;
                up.y = qrz * qdx - qrx * qdz;
                up.z = qrx * qdy - qry * qdx;
''')
    m=sc.asm(t); print(''.join(names[i][0][1:] for i in p), sc.regions(m)[0], who.tab(m).split('  ')[0])
