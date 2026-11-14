import sys
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/research/tools'); import score
R='/home/kyle/workspace/moh-frontline/'
base=open(R+'scratch/loop3/forms2/am3.cpp').read()
T=R+'config/GMFE69/animmorph_dtor.json'
def sc(t): return score.real(T,t,'Draw__10CAnimMorphFR12CDrawContext','800653c0','424')
A='inline void Assign(CVector3& d, const CVector3& s) { d = s; }'
M='    CVector3() {}\n'
V={
'm1':(A,M+'    void Set(const CVector3& s) { *this = s; }\n','left.Set(view)'),
'm2':(A,M+'    void Set(const CVector3& s) { operator=(s); }\n','left.Set(view)'),
'm3':('inline void Assign(CVector3& d, CVector3& s) { d = s; }',M,'Assign(left, view)'),
'm4':('inline void Assign(CVector3& d, const CVector3& s) { CVector3& r = d; r = s; }',M,'Assign(left, view)'),
'm5':('inline void Assign(CVector3& d, const CVector3& s) { const CVector3& r = s; d = r; }',M,'Assign(left, view)'),
'm6':(A,M+'    CVector3& Ref() { return *this; }\n','left.Ref() = view'),
}
for n,(a,m,call) in V.items():
    t=base.replace(A,a,1).replace(M,m,1).replace('Assign(left, view)',call,1)
    open(R+'scratch/loop3/forms2/v/'+n+'.cpp','w').write(t); print(n,sc(t),flush=True)
