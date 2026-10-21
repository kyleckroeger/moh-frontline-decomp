// A fragment of thrownbullet.cpp (0x800d301c): CreateExplosionEffect starts
// a grenade explosion's particle systems at the given matrix (three for a
// normal explosion, a fourth when asked, or four others for the alternative
// effect), each by its CRC id, and a light half a unit above the explosion's
// position. The function, CParticleSystem::Create, CLight::Create and the
// virtuals are named by the symbols (the virtuals by their table slots); the
// meaning of the two flags and the class layouts are inferred (CParticleSystem's
// table pointer follows 28 bytes of members). The rest of the file is in other
// units.
class CVector3 {
public:
    union {
        double pair[2];
        float v[4];
    } d;
} __attribute__((aligned(8)));

class CMatrix {
public:
    CVector3 GetPos() const { return *(const CVector3*)m[3]; }

    float m[4][4];
} __attribute__((aligned(16)));

class CParticleSystem {
    unsigned char m_unknown00[28];

public:
    static CParticleSystem* Create(unsigned long);
    virtual void unknown008();
    virtual void unknown012();
    virtual void unknown016();
    virtual void unknown020();
    virtual void unknown024();
    virtual void unknown028();
    virtual void unknown032();
    virtual void unknown036();
    virtual void unknown040();
    virtual void unknown044();
    virtual void unknown048();
    virtual void unknown052();
    virtual void unknown056();
    virtual void unknown060();
    virtual void unknown064();
    virtual void unknown068();
    virtual void unknown072();
    virtual void unknown076();
    virtual void unknown080();
    virtual void unknown084();
    virtual void unknown088();
    virtual void unknown092();
    virtual void unknown096();
    virtual void unknown100();
    virtual void unknown104();
    virtual void unknown108();
    virtual void unknown112();
    virtual void unknown116();
    virtual void unknown120();
    virtual void unknown124();
    virtual void unknown128();
    virtual void unknown132();
    virtual void unknown136();
    virtual void SetLocalToWorld(const CMatrix&);
};

class CLight {
public:
    static CLight* Create(unsigned long);
    virtual void unknown008();
    virtual void unknown012();
    virtual void unknown016();
    virtual void unknown020();
    virtual void unknown024();
    virtual void unknown028();
    virtual void unknown032();
    virtual void unknown036();
    virtual void unknown040();
    virtual void unknown044();
    virtual void unknown048();
    virtual void unknown052();
    virtual void unknown056();
    virtual void unknown060();
    virtual void unknown064();
    virtual void unknown068();
    virtual void unknown072();
    virtual void unknown076();
    virtual void unknown080();
    virtual void unknown084();
    virtual void unknown088();
    virtual void unknown092();
    virtual void unknown096();
    virtual void unknown100();
    virtual void unknown104();
    virtual void unknown108();
    virtual void unknown112();
    virtual void unknown116();
    virtual void unknown120();
    virtual void unknown124();
    virtual void unknown128();
    virtual void unknown132();
    virtual void unknown136();
    virtual void unknown140();
    virtual void unknown144();
    virtual void unknown148();
    virtual void unknown152();
    virtual void unknown156();
    virtual void unknown160();
    virtual void unknown164();
    virtual void unknown168();
    virtual void unknown172();
    virtual void unknown176();
    virtual void unknown180();
    virtual void unknown184();
    virtual void unknown188();
    virtual void unknown192();
    virtual void unknown196();
    virtual void unknown200();
    virtual void unknown204();
    virtual void unknown208();
    virtual void unknown212();
    virtual void unknown216();
    virtual void unknown220();
    virtual void unknown224();
    virtual void unknown228();
    virtual void unknown232();
    virtual void unknown236();
    virtual void unknown240();
    virtual void unknown244();
    virtual void unknown248();
    virtual void unknown252();
    virtual void unknown256();
    virtual void unknown260();
    virtual void SetPosition(CVector3);
};

void CreateExplosionEffect(const CMatrix& matrix, bool extra, bool alternative) {
    if (!alternative) {
        CParticleSystem* system = CParticleSystem::Create(0xFB394D09);
        if (system)
            system->SetLocalToWorld(matrix);
        system = CParticleSystem::Create(0x4F2B8179);
        if (system)
            system->SetLocalToWorld(matrix);
        system = CParticleSystem::Create(0x1CD6E4B5);
        if (system)
            system->SetLocalToWorld(matrix);
        if (extra) {
            system = CParticleSystem::Create(0x4822CD8F);
            if (system)
                system->SetLocalToWorld(matrix);
        }
        CLight* light = CLight::Create(0x9BCE1511);
        if (light) {
            CVector3 position = matrix.GetPos();
            position.d.v[2] += 0.5f;
            light->SetPosition(position);
        }
    } else {
        CParticleSystem* system = CParticleSystem::Create(0xABD02935);
        if (system)
            system->SetLocalToWorld(matrix);
        system = CParticleSystem::Create(0x6BD7A8DB);
        if (system)
            system->SetLocalToWorld(matrix);
        system = CParticleSystem::Create(0x28E4D6EF);
        if (system)
            system->SetLocalToWorld(matrix);
        system = CParticleSystem::Create(0xFF8E1F13);
        if (system)
            system->SetLocalToWorld(matrix);
        CLight* light = CLight::Create(0x9BCE1511);
        if (light) {
            CVector3 position = matrix.GetPos();
            position.d.v[2] += 0.5f;
            light->SetPosition(position);
        }
    }
}
