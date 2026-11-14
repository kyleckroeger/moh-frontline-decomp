import subprocess,os
os.environ['T']='/home/kyle/workspace/moh-frontline/config/GMFE69/Soldier_object.json'
F='Draw__14CSoldierObjectFR12CDrawContext'
base=open('sd.cpp').read()
def score(src,tag):
    open(f'd_{tag}.cpp','w').write(src)
    return subprocess.run(['python3','rm.py',f'd_{tag}.cpp',F],capture_output=True,text=True).stdout.strip()
TOP="void CSoldierObject::Draw(CDrawContext& context) {\n"
RW="""        int i;
        for (i = 0; i < 9; i++) {
            if (m_weapons[i]) {
                if (g_scene.IsNodeInScene((ISceneNode*)m_weapons[i]))
                    g_scene.Remove(*(ISceneNode*)m_weapons[i]);
                g_WeaponFactory.DestroyWeapon(m_weapons[i]);
                m_weapons[i] = 0;
            }
        }
"""
assert RW in base
V={
'self_top':base.replace(TOP,TOP+"    CSoldierObject* const self = this;\n").replace("            RemoveWeapons();","            self->RemoveWeapons();"),
'self_rw':base.replace(RW,"        CSoldierObject* const self = this;\n"+RW.replace("m_weapons","self->m_weapons")),
'ctx_top':base.replace(TOP,TOP+"    CDrawContext* const c_context = &context;\n").replace("    CDrawContext local = context;","    CDrawContext local = *c_context;"),
'wp_rw':base.replace(RW,"        int i;\n        CWeapon** w;\n        w = m_weapons;\n        for (i = 0; i < 9; i++, w++) {\n            if (*w) {\n                if (g_scene.IsNodeInScene((ISceneNode*)*w))\n                    g_scene.Remove(*(ISceneNode*)*w);\n                g_WeaponFactory.DestroyWeapon(*w);\n                *w = 0;\n            }\n        }\n"),
}
V['self_both']=V['self_top'].replace(RW,"        CSoldierObject* const self = this;\n"+RW.replace("m_weapons","self->m_weapons"))
for n,s in V.items(): print(n,score(s,n)[:160])
