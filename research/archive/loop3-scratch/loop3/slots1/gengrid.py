import itertools
base=open('psf_p1.cpp').read()
pre=base[:base.index('inline CColor BSArgColor')]
setold=pre[pre.index('    void Set('):pre.index('    CColor m_from;')]
A=lambda i:', '.join('*(*stack - (count - %d))'%k for k in range(i,i+4))
F='*(float*)(*stack - (count - 9))'
bsa_local='''inline CColor BSArgColor(int** stack, int count, int i) {
    CColor c;
    c.r = *(*stack - (count - i));
    c.g = *(*stack - (count - (i + 1)));
    c.b = *(*stack - (count - (i + 2)));
    c.a = *(*stack - (count - (i + 3)));
    return c;
}
'''
bsa_ctor='''inline CColor BSArgColor(int** stack, int count, int i) {
    return CColor(*(*stack - (count - i)), *(*stack - (count - (i + 1))), *(*stack - (count - (i + 2))), *(*stack - (count - (i + 3))));
}
'''
head='''void BIFunc_PlayerScreenFlash(int** stack, void*) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
'''
tail='''    CFader::SetControl(&g_screenflash);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
'''
n=0
for order,ref,opv,form,body in itertools.product(['tf','ft'],[0,1],[0,1],['ctor','bsal','bsac','locFT','locTF'],[0,1]):
    T='const CColor&' if ref else 'CColor'
    params='%s to, %s from'%(T,T) if order=='tf' else '%s from, %s to'%(T,T)
    b='        m_from = from;\n        m_to = to;\n' if body==0 else '        m_to = to;\n        m_from = from;\n'
    s=pre.replace(setold,'    void Set(%s, float duration) {\n        m_time = 0.0f;\n%s        m_duration = duration;\n    }\n\n'%(params,b))
    if opv: s=s.replace('CColor& operator=(const CColor& c)','CColor& operator=(CColor c)')
    def call(fe,te):
        return '    g_screenflash.Set(%s, %s, %s);\n'%((te,fe) if order=='tf' else (fe,te))+'' if False else '    g_screenflash.Set(%s, %s, %s);\n'%(((te,fe) if order=='tf' else (fe,te))+(F,))
    if form=='ctor': s+=head+call('CColor(%s)'%A(1),'CColor(%s)'%A(5))+tail
    elif form in('bsal','bsac'): s+=(bsa_local if form=='bsal' else bsa_ctor)+head+call('BSArgColor(stack, count, 1)','BSArgColor(stack, count, 5)')+tail
    else:
        d1='    CColor from(%s);\n'%A(1); d2='    CColor to(%s);\n'%A(5)
        s+=head+(d1+d2 if form=='locFT' else d2+d1)+call('from','to')+tail
    nm='grid/g_%s_%d%d_%s_%d.cpp'%(order,ref,opv,form,body)
    open(nm,'w').write(s); n+=1
print(n)
