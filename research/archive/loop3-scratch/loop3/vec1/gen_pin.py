import subprocess,sys
base=r'''struct Pair { double a, b; };
class V {
public:
%s
    float x, y, z, w;
} __attribute__((aligned(8)));
float R();
void Get(V&, V&);
void Out(float, float, float);
void F(int n) {
    V lo, hi, spawn;
    Get(lo, hi);
    for (int i = 0; i < n; i++) {
%s
        Out(spawn.x, spawn.y, spawn.z);
    }
}
'''
forms={
 'plain':('', '        spawn.x = lo.x + hi.x * R(); spawn.y = lo.y + hi.y * R(); spawn.z = lo.z + hi.z * R();'),
 'idx_this':('    float& operator[](int i) { return ((float*)this)[i]; }', '        spawn[0] = lo.x + hi.x * R(); spawn[1] = lo.y + hi.y * R(); spawn[2] = lo.z + hi.z * R();'),
 'idx_x':('    float& operator[](int i) { return (&x)[i]; }', '        spawn[0] = lo.x + hi.x * R(); spawn[1] = lo.y + hi.y * R(); spawn[2] = lo.z + hi.z * R();'),
 'set':('    void Set(float a, float b, float c) { x = a; y = b; z = c; }', '        float a = lo.x + hi.x * R(); float b = lo.y + hi.y * R(); float c = lo.z + hi.z * R(); spawn.Set(a, b, c);'),
 'setret':('    V& Set(float a, float b, float c) { x = a; y = b; z = c; return *this; }', '        float a = lo.x + hi.x * R(); float b = lo.y + hi.y * R(); float c = lo.z + hi.z * R(); spawn.Set(a, b, c);'),
 'ptrthis':('    V* Ptr() { return this; }', '        spawn.Ptr()->x = lo.x + hi.x * R(); spawn.Ptr()->y = lo.y + hi.y * R(); spawn.Ptr()->z = lo.z + hi.z * R();'),
 'opassign_unused':('    V& operator=(const V& o) { *(Pair*)this = *(const Pair*)&o; return *this; }', '        spawn.x = lo.x + hi.x * R(); spawn.y = lo.y + hi.y * R(); spawn.z = lo.z + hi.z * R();'),
 'pairfield':('    float* Data() { return &x; }', '        spawn.Data()[0] = lo.x + hi.x * R(); spawn.Data()[1] = lo.y + hi.y * R(); spawn.Data()[2] = lo.z + hi.z * R();'),
}
for k,(m,b) in forms.items():
    open('pp_%s.cpp'%k,'w').write(base%(m,b))
    out=subprocess.run(['python3','probe.py','pp_%s.cpp'%k],capture_output=True,text=True).stdout
    st=[l for l in out.splitlines() if l.startswith('stfs')]
    print('%-16s stfs=%d %s'%(k,len(st),' '.join(x.split()[-1] for x in st)))
