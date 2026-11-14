src=open('psf0.cpp').read()
ctor='    CColor(unsigned char ar, unsigned char ag, unsigned char ab, unsigned char aa) : r(ar), g(ag), b(ab), a(aa) {}\n'
src=src.replace('struct CColor {\n','struct CColor {\n    CColor() {}\n'+ctor)
fb=src[src.index('void BIFunc_PlayerScreenFlash'):]
pre=src[:src.index('void BIFunc_PlayerScreenFlash')]
A='*(*stack - (count - %d))'
def args(i): return ', '.join(A%(i+k) for k in range(4))
tail='''    CFader::SetControl(&g_screenflash);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
'''
head='''void BIFunc_PlayerScreenFlash(int** stack, void*) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
'''
F='*(float*)(*stack - (count - 9))'
V={}
V['p1']=pre+head+'    g_screenflash.Set(CColor(%s), CColor(%s), %s);\n'%(args(5),args(1),F)+tail
V['p2']=pre+head+'    CColor from(%s);\n    CColor to(%s);\n    g_screenflash.Set(to, from, %s);\n'%(args(1),args(5),F)+tail
V['p3']=pre.replace('''    CColor c;
    c.r = *(*stack - (count - i));
    c.g = *(*stack - (count - (i + 1)));
    c.b = *(*stack - (count - (i + 2)));
    c.a = *(*stack - (count - (i + 3)));
    return c;''','''    return CColor(*(*stack - (count - i)), *(*stack - (count - (i + 1))), *(*stack - (count - (i + 2))), *(*stack - (count - (i + 3))));''')+fb
V['p4']=V['p1'].replace('void Set(CColor to, CColor from, float duration)','void Set(const CColor& to, const CColor& from, float duration)')
V['p5']=V['p2'].replace('void Set(CColor to, CColor from, float duration)','void Set(const CColor& to, const CColor& from, float duration)')
V['p6']=V['p1'].replace('CColor& operator=(const CColor& c)','CColor& operator=(CColor c)').replace('void Set(CColor to, CColor from, float duration)','void Set(const CColor& to, const CColor& from, float duration)')
V['p7']=pre+head+'    CColor to(%s);\n    CColor from(%s);\n    g_screenflash.Set(to, from, %s);\n'%(args(5),args(1),F)+tail
for k,v in V.items(): open('psf_'+k+'.cpp','w').write(v)
