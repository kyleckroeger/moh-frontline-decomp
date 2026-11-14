import os
B=open('scratch/loop3/day2/csweep/base.cpp').read()
out='scratch/loop3/day2/csweep/v4'; os.makedirs(out,exist_ok=True)
L='        bool skinned = mesh->m_skin != 0;\n'
F=['bool skinned = (bool)(mesh->m_skin != 0);',
'bool skinned = (int)(mesh->m_skin != 0);',
'bool skinned = (char)(mesh->m_skin != 0) != 0;',
'bool skinned = (mesh->m_skin != 0) && true;',
'bool skinned = true && mesh->m_skin != 0;',
'bool skinned = (mesh->m_skin != 0) ? true : false;',
'bool skinned = !(mesh->m_skin == 0);',
'bool skinned = mesh->m_skin != 0 ? 1 : 0;',
'bool skinned = (bool)(int)(mesh->m_skin != 0);',
'int s = mesh->m_skin != 0; bool skinned = s;',
'bool s = mesh->m_skin != 0; bool skinned = s;',
'char s = mesh->m_skin != 0; bool skinned = s;',
'bool skinned = (bool)mesh->m_skin != 0;',
'bool skinned = !!(mesh->m_skin != 0);',
'bool skinned = !(mesh->m_skin != 0) == false;',
'bool skinned = (mesh->m_skin != 0) | false;',
'bool skinned = (mesh->m_skin != 0) & true;',
'bool skinned = (mesh->m_skin != 0) ^ false;',
'bool skinned = (unsigned long)mesh->m_skin != 0UL;',
'bool skinned = (char*)mesh->m_skin != (char*)0;',
'bool skinned = mesh->m_skin > (void*)0;',
'bool skinned = (mesh->m_skin != 0) == 1;',
'bool skinned = 1 == (mesh->m_skin != 0);',
'bool skinned(mesh->m_skin != 0);',
'bool skinned = bool(mesh->m_skin != 0);',
'bool skinned = static_cast<bool>(mesh->m_skin != 0);',
'bool skinned = mesh->m_skin != 0 || false;',
'bool skinned = false || mesh->m_skin != 0;',
'bool skinned = (mesh->m_skin != 0) != 0;',
'bool skinned = (mesh->m_skin != 0) > 0;',
]
for n,f in enumerate(F):
  open('%s/f%02d.cpp'%(out,n),'w').write(B.replace(L,'        '+f+'\n'))
  print(n,f)
