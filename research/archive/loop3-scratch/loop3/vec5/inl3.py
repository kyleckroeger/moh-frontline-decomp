import sys
sys.path.insert(0,'.'); import sc
exec(open('inl2.py').read().split("HEAD='''")[0])
H={}
H['I']='''        CVector3 start;
        float vx, vy, vz;
        Kin(start, vx, vy, vz, sv, sa, (m_updateTicks - m_startTicks) / 60.0f);
        float age = (g_ParticleSystemManager.GetCurrentTicks() - m_updateTicks) / 60.0f;
'''+END1
H['J']=H['I'].replace(END1,END2)
H['K']='''        float age;
        CVector3 start;
        float vx, vy, vz;
        Kin(start, vx, vy, vz, sv, sa, (m_updateTicks - m_startTicks) / 60.0f);
        age = (g_ParticleSystemManager.GetCurrentTicks() - m_updateTicks) / 60.0f;
'''+END1
H['L']='''        float age;
        float a2;
        float vx, vy, vz;
        CVector3 start;
        Kin(start, vx, vy, vz, sv, sa, (m_updateTicks - m_startTicks) / 60.0f);
        age = (g_ParticleSystemManager.GetCurrentTicks() - m_updateTicks) / 60.0f;
'''+END1.replace('float a2','a2')
for nm,blk in H.items():
    src=s[:fa]+KIN+ADV+s[fa:a]+blk+s[b:]
    open('inl_%s.cpp'%nm,'w').write(src)
