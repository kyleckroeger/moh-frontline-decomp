import itertools,glob
for f in glob.glob('grid2/*_ctor_*.cpp'):
    s=open(f).read()
    i=s.index('    g_screenflash.Set(')
    line=s[i:s.index('\n',i)+1]
    ft='_ft_' in f
    names1=['r1','g1','b1','a1'];names2=['r2','g2','b2','a2']
    decl=''.join('    unsigned char %s = *(*stack - (count - %d));\n'%(n,k) for k,n in zip(range(1,9),names1+names2))
    decl+='    float time = *(float*)(*stack - (count - 9));\n'
    cf='CColor(r1, g1, b1, a1)'; ct='CColor(r2, g2, b2, a2)'
    call='    g_screenflash.Set(%s, %s, time);\n'%((cf,ct) if ft else (ct,cf))
    s2=s.replace(line,decl+call)
    open(f.replace('grid2','grid3').replace('_ctor_','_named_'),'w').write(s2)
    # implicit op=
    for src,tag in ((s,'ctor'),(s2,'named')):
        t=src.replace('    CColor& operator=(const CColor& c) { r = c.r; g = c.g; b = c.b; a = c.a; return *this; }\n','').replace('    CColor& operator=(CColor c) { r = c.r; g = c.g; b = c.b; a = c.a; return *this; }\n','')
        if t!=src: open(f.replace('grid2','grid3').replace('_ctor_','_%simp_'%tag),'w').write(t)
