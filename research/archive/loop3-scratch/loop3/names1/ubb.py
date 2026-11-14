B=open('/home/kyle/workspace/moh-frontline/scratch/loop3/names1/ubb_base.cpp').read()
ADD=('''    CVector3& operator+=(const CVector3& v) {''','''    void Add(const CVector3& v) { d.v[0] += v.d.v[0]; d.v[1] += v.d.v[1]; d.v[2] += v.d.v[2]; }
    CVector3& operator+=(const CVector3& v) {''')
S0=B.index('        CVector3 start;\n        start.d.v[0] = t * sv.d.v[0];')
S1=B.index('        CVector3 lo = start;\n        CVector3 hi = start;')
OLD=B[S0:S1]
V={'base':[],
'mul_plain':[(OLD,OLD.replace('Mul(t2, sa.d.v, 0)','t2 * sa.d.v[0]').replace('Mul(t2, sa.d.v, 1)','t2 * sa.d.v[1]').replace('Mul(t2, sa.d.v, 2)','t2 * sa.d.v[2]'))],
'acc_scale':[ADD,(OLD.split('        start.d.v[0] += sx;')[0].split('        sx = Mul')[0] + '''        start.d.v[0] += sx;''' , '')] ,
}
V={'base':[], 'mul_plain':V['mul_plain']}
SX='''        sx = Mul(t2, sa.d.v, 0);
        sy = Mul(t2, sa.d.v, 1);
        sz = Mul(t2, sa.d.v, 2);
        start.d.v[0] += sx;
        start.d.v[1] += sy;
        start.d.v[2] += sz;
'''
V['acc_scale_add']=[ADD,(SX,'''        CVector3 acc;
        acc.Scale(sa, t2);
        start.Add(acc);
''')]
V['acc_scale_pe']=[(SX,'''        CVector3 acc;
        acc.Scale(sa, t2);
        start += acc;
''')]
FULL='''        CVector3 start;
        start.Scale(sv, t);
        CVector3 acc;
        acc.Scale(sa, t2);
        start.Add(acc);
        CVector3 vel;
        vel.Scale(sa, k);
        vel.Add(sv);
        CVector3 e;
        e.Scale(vel, age);
        e.Add(start);
        CVector3 aa;
        aa.Scale(sa, a2);
        e.Add(aa);
        float ex = e.d.v[0];
        float ey = e.d.v[1];
        float ez = e.d.v[2];
'''
V['full_vec']=[ADD,(OLD,FULL),('        float ex;\n',''),('        float ey;\n',''),('        float ez;\n','')]
