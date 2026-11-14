src=open('am0.cpp').read()
old='''    CVector3 operator=(const CVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
        return *this;
    }'''
assert old in src
V={
'retctor':'''    CVector3 operator=(const CVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
        return CVector3(*this);
    }''',
'retother':'''    CVector3 operator=(const CVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
        return other;
    }''',
'outofclass':'''    CVector3 operator=(const CVector3& other);''',
}
for k,v in V.items():
    s=src.replace(old,v)
    if k=='outofclass':
        s=s.replace('inline void Assign','inline CVector3 CVector3::operator=(const CVector3& other) {\n    *(CVector3Pair*)this = *(const CVector3Pair*)&other;\n    return *this;\n}\ninline void Assign')
    open('x_'+k+'.cpp','w').write(s)
