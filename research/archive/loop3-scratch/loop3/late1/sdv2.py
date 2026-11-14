from sdv import *
RW2=RW
V={
 'i_top':base.replace(RW,RW.replace("        int i;\n","")).replace("    void RemoveWeapons() {\n","    void RemoveWeapons() {\n        int i;\n"),
 'while':base.replace(RW,RW.replace("        for (i = 0; i < 9; i++) {","        i = 0;\n        while (i < 9) {").replace("                m_weapons[i] = 0;\n            }\n        }\n","                m_weapons[i] = 0;\n            }\n            i++;\n        }\n")),
 'wtmp':base.replace(RW,RW.replace("            if (m_weapons[i]) {","            CWeapon* w = m_weapons[i];\n            if (w) {").replace("(ISceneNode*)m_weapons[i]","(ISceneNode*)w").replace("DestroyWeapon(m_weapons[i])","DestroyWeapon(w)")),
 'ctx_self':base.replace(TOP,TOP+"    CDrawContext* const c_context = &context;\n    CSoldierObject* const self = this;\n").replace("    CDrawContext local = context;","    CDrawContext local = *c_context;"),
}
for n,s in V.items(): print(n,score(s,n)[:160])
