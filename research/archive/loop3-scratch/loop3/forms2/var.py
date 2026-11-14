# var.py: score replacement variants of am_c2.cpp
import sys,itertools
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/research/tools'); import score
R='/home/kyle/workspace/moh-frontline/'
base=open(R+'scratch/loop3/forms2/am_c2.cpp').read()
T=R+'config/GMFE69/animmorph_dtor.json'
def sc(t): r=score.real(T,t,'Draw__10CAnimMorphFR12CDrawContext','800653c0','424'); return r
OPEQ='''        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
        return *this;'''
ASG='inline void Assign(CVector3& d, const CVector3& s) { d = s; }'
BLK='''        CVector3 left;
        Assign(left, view);
        left.x -= 1.0f;
        CVector3 right = view;
        right.x += 1.0f;'''
opeqs={
 'o0':OPEQ,
 'o1':'''        ((CVector3Pair*)this)->pair[0] = ((const CVector3Pair*)&other)->pair[0];
        ((CVector3Pair*)this)->pair[1] = ((const CVector3Pair*)&other)->pair[1];
        return *this;''',
 'o2':'''        ((CVector3Pair*)this)->pair[1] = ((const CVector3Pair*)&other)->pair[1];
        ((CVector3Pair*)this)->pair[0] = ((const CVector3Pair*)&other)->pair[0];
        return *this;''',
 'o3':'''        CVector3Pair& d = *(CVector3Pair*)this;
        d = *(const CVector3Pair*)&other;
        return *this;''',
}
asgs={
 'a0':ASG,
 'a1':'inline void Assign(CVector3& d, const CVector3& s) { d.operator=(s); }',
 'a2':'inline void Assign(CVector3& d, const CVector3& s) { (void)(d = s); }',
 'a3':'inline void Assign(CVector3& d, CVector3 s) { d = s; }',
 'a4':'inline void Assign(CVector3* d, const CVector3& s) { *d = s; }',
 'a5':'inline void Assign(const CVector3& s, CVector3& d) { d = s; }',
}
blks={
 'b0':BLK,
 'b1':'''        CVector3 left;
        Assign(left, view);
        CVector3 right = view;
        left.x -= 1.0f;
        right.x += 1.0f;''',
 'b2':'''        CVector3 right = view;
        CVector3 left;
        Assign(left, view);
        left.x -= 1.0f;
        right.x += 1.0f;''',
 'b3':'''        CVector3 left;
        CVector3 right = view;
        Assign(left, view);
        left.x -= 1.0f;
        right.x += 1.0f;''',
 'b4':'''        CVector3 left;
        Assign(left, view);
        CVector3 right = view;
        right.x += 1.0f;
        left.x -= 1.0f;''',
 'b5':'''        CVector3 left;
        Assign(left, view);
        left.x -= 1.0f;
        CVector3 right(view);
        right.x += 1.0f;''',
 'b6':'''        CVector3 left;
        CVector3 right;
        Assign(left, view);
        left.x -= 1.0f;
        right = view;
        right.x += 1.0f;''',
}
for (ok,o),(ak,a),(bk,b) in itertools.product(opeqs.items(),asgs.items(),blks.items()):
    t=base.replace(OPEQ,o).replace(ASG,a).replace(BLK,b)
    if ak=='a4': t=t.replace('Assign(left, view)','Assign(&left, view)')
    if ak=='a5': t=t.replace('Assign(left, view)','Assign(view, left)')
    n=ok+ak+bk; open(R+'scratch/loop3/forms2/v/'+n+'.cpp','w').write(t)
    print(n,sc(t),flush=True)
