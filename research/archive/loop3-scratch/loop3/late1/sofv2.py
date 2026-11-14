from sofv import *
TOP="void CStaticObjectFactory::Init() {\n"
a=B.replace(TOP,TOP+"    int count;\n").replace("        int count = 0;\n","        count = 0;\n")
print('cfirst',sc(a,'cf'))
print('cfirst_cl',sc(constlate(a),'cfcl'))
b=B.replace("        int count = 0;\n","        int const zero = 0;\n        int count = zero;\n")
print('zero',sc(b,'z'))
c=B.replace(TOP,TOP+"    StaticObjectTemplate* tmpl;\n").replace("        g_pStaticObjectTemplates = NewTemplates(g_numTemplates);\n","        tmpl = NewTemplates(g_numTemplates);\n        g_pStaticObjectTemplates = tmpl;\n")
print('tmplfirst',sc(c,'tf'))
