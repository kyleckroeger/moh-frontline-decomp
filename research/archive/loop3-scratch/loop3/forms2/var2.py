import sys,itertools
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/research/tools'); import score
R='/home/kyle/workspace/moh-frontline/'
base=open(R+'scratch/loop3/forms2/am_c2.cpp').read()
T=R+'config/GMFE69/animmorph_dtor.json'
def sc(t): return score.real(T,t,'Draw__10CAnimMorphFR12CDrawContext','800653c0','424')
CC='    CVector3(const CVector3& other) { *(CVector3Pair*)this = *(const CVector3Pair*)&other; }'
OPEQ='''        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
        return *this;'''
P='((CVector3Pair*)this)->pair[%d] = ((const CVector3Pair*)&other)->pair[%d];'
ccs={'c0':CC,
 'c1':'    CVector3(const CVector3& other) { '+P%(0,0)+' '+P%(1,1)+' }',
 'c2':'    CVector3(const CVector3& other) { '+P%(1,1)+' '+P%(0,0)+' }',
 'c3':'    CVector3(const CVector3& other) { x = other.x; y = other.y; z = other.z; w = other.w; }',
 'c4':'    CVector3(const CVector3& other) { *(double*)&x = *(const double*)&other.x; *(double*)&z = *(const double*)&other.z; }',
}
ops={'o0':OPEQ,
 'o5':'''        CVector3Pair& d = *(CVector3Pair*)this; const CVector3Pair& s = *(const CVector3Pair*)&other;
        d = s;
        return *this;''',
 'o6':'''        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
        CVector3 r(*this); return r;''',
 'o7':'''        *(double*)&x = *(const double*)&other.x; *(double*)&z = *(const double*)&other.z;
        return *this;''',
 'o8':'''        CVector3 r(other);
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
        return r;''',
 'o9':'''        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
        return other;''',
}
for (ck,c),(ok,o) in itertools.product(ccs.items(),ops.items()):
    t=base.replace(CC,c).replace(OPEQ,o)
    n=ck+ok; open(R+'scratch/loop3/forms2/v/'+n+'.cpp','w').write(t); print(n,sc(t),flush=True)
