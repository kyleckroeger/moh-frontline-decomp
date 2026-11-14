import sys
B=open('scratch/loop3/day2/csweep/base.cpp').read()
L='        bool skinned = mesh->m_skin != 0;\n'
S='    void* m_skin;\n'
assert L in B and S in B
V={
 'impl': ('bool skinned = mesh->m_skin;',None),
 'cast': ('bool skinned = (bool)mesh->m_skin;',None),
 'tern': ('bool skinned = mesh->m_skin ? true : false;',None),
 'tern10': ('bool skinned = mesh->m_skin ? 1 : 0;',None),
 'notnot': ('bool skinned = !!mesh->m_skin;',None),
 'eqnull': ('bool skinned = !(mesh->m_skin == 0);',None),
 'nullfirst': ('bool skinned = 0 != mesh->m_skin;',None),
 'NULLcmp': ('bool skinned = mesh->m_skin != (void*)0;',None),
 'uc': ('unsigned char skinned = mesh->m_skin != 0;',None),
 'sc': ('signed char skinned = mesh->m_skin != 0;',None),
 'int': ('int skinned = mesh->m_skin != 0;',None),
 'u32': ('unsigned int skinned = mesh->m_skin != 0;',None),
 'constb': ('const bool skinned = mesh->m_skin != 0;',None),
 'inl_ne': ('bool skinned = mesh->IsSkinned();','    bool IsSkinned() const { return m_skin != 0; }\n'),
 'inl_nn': ('bool skinned = mesh->IsSkinned();','    bool IsSkinned() const { return !!m_skin; }\n'),
 'inl_impl': ('bool skinned = mesh->IsSkinned();','    bool IsSkinned() const { return m_skin; }\n'),
 'inl_tern': ('bool skinned = mesh->IsSkinned();','    bool IsSkinned() const { return m_skin ? true : false; }\n'),
 'inl_if': ('bool skinned = mesh->IsSkinned();','    bool IsSkinned() const { if (m_skin) return true; return false; }\n'),
 'inl_int': ('bool skinned = mesh->IsSkinned();','    int IsSkinned() const { return m_skin != 0; }\n'),
 'inl_char': ('bool skinned = mesh->IsSkinned();','    char IsSkinned() const { return m_skin != 0; }\n'),
 'int_inl_int': ('int skinned = mesh->IsSkinned();','    int IsSkinned() const { return m_skin != 0; }\n'),
 'inl_get': ('bool skinned = mesh->GetSkin() != 0;','    void* GetSkin() const { return m_skin; }\n'),
 'inl_get_impl': ('bool skinned = mesh->GetSkin();','    void* GetSkin() const { return m_skin; }\n'),
 'skinptr': ('void* skin = mesh->m_skin; bool skinned = skin != 0;',None),
 'ifset': ('bool skinned = false; if (mesh->m_skin) skinned = true;',None),
 'ifelse': ('bool skinned; if (mesh->m_skin) skinned = true; else skinned = false;',None),
 'ifset_int': ('int skinned = 0; if (mesh->m_skin) skinned = 1;',None),
 'bf': ('struct { bool b : 1; } sk; sk.b = mesh->m_skin != 0; bool skinned = sk.b;',None),
 'vol': ('bool skinned = (mesh->m_skin != 0) == true;',None),
 'neq_false': ('bool skinned = (mesh->m_skin != 0) != false;',None),
 'u8cast': ('bool skinned = (unsigned char)(mesh->m_skin != 0);',None),
 'ccast': ('bool skinned = (char)(mesh->m_skin != 0);',None),
 'ulcmp': ('bool skinned = (unsigned long)mesh->m_skin != 0;',None),
 'icmp': ('bool skinned = (int)mesh->m_skin != 0;',None),
 'icmpgt': ('bool skinned = (unsigned long)mesh->m_skin > 0;',None),
}
for k,(l,s) in V.items():
  t=B.replace(L,'        '+l+'\n')
  if s: t=t.replace(S,S+s)
  open('scratch/loop3/day2/csweep/v/%s.cpp'%k,'w').write(t)
