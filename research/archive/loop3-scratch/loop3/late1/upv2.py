from upv import *
US=base[base.index("inline void UpdateStatus("):]
US=US[:US.index("\n}\n")+3]
US2=US.replace("inline void UpdateStatus(BSGO_Basic* user, bool a, bool b) {\n    BSGO_Basic* const object = user;\n","inline void UpdateStatus2(BSGO_Basic* object, bool a, bool b) {\n")
V={
 'plain2':base.replace(US,US+US2).replace(GL,GL.replace("UpdateStatus(","UpdateStatus2(")),
 'gwhile':base.replace(GL,"    user = g_pGermanList;\n    while (user) {\n        UpdateStatus(user, true, false);\n        user = user->m_next;\n    }\n"),
 'gnext':base.replace(GL,"    for (user = g_pGermanList; user; ) {\n        UpdateStatus(user, true, false);\n        user = user->m_next;\n    }\n"),
 'gcast':base.replace(GL,"    for (user = g_pGermanList; user; user = user->m_next)\n        UpdateStatus((BSGO_Basic*)user, true, false);\n"),
 'gtrue':base.replace(GL,"    for (user = g_pGermanList; user; user = user->m_next)\n        if (true) UpdateStatus(user, true, false);\n"),
 'gdo':base.replace(GL,"    if ((user = g_pGermanList) != 0) {\n        do {\n            UpdateStatus(user, true, false);\n        } while ((user = user->m_next) != 0);\n    }\n"),
}
for n,s in V.items(): print(n,score(s,n)[:140])
