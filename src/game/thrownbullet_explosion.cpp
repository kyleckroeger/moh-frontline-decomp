// A fragment of thrownbullet.cpp (0x800d301c): CreateExplosionEffect starts
// a grenade explosion's particle systems at the given matrix (three for a
// normal explosion, a fourth when asked, or four others for the alternative
// effect), each by its CRC id, and a light half a unit above the explosion's
// position. CThrownBullet::AttemptUpdate (unless the bullet is halted) resets
// the closest distance, restarts from the launch velocity (with gravity when
// enabled), advances the spin by the distance travelled, moves the matrix to
// the new position, sweeps the fiber volume from the old position to it and
// stores the fiber's extents (widened by CONTACT_EPSILON) in the scene's
// sweep entries. The functions, CParticleSystem::Create, CLight::Create and
// the virtuals are named by the symbols (the virtuals by their table slots);
// the meaning of the two flags, the members and the class layouts are
// inferred (CParticleSystem's table pointer follows 28 bytes of members).
// The rest of the file is in other units.
extern inline float sqrtf(float x)
{
    const double _half = .5;
    const double _three = 3.0;
    volatile float y;
    if (x > 0.0f)
    {
        double guess = __frsqrte((double)x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        y = (float)(x*guess);
        return y;
    }
    return x;
}

class CVector3 {
public:
    float Length() const { return sqrtf(d.v[0] * d.v[0] + d.v[1] * d.v[1] + d.v[2] * d.v[2]); }

    union {
        double pair[2];
        float v[4];
    } d;
} __attribute__((aligned(8)));

class CMatrix {
public:
    CVector3 GetPos() const { return *(const CVector3*)m[3]; }
    void SetPos(CVector3);

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

class CVolFiber {
public:
    void Set(CVector3, CVector3);
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
    virtual void GetExtents(CVector3&, CVector3&) const;

    unsigned char unknown04[76];
};

/* inferred: a scene sweep extent (its coordinate at +8) */
struct SExtentView {
    unsigned char unknown00[8];
    float m_value;
};

extern const float CONTACT_EPSILON;

class CThrownBullet {
public:
    void AttemptUpdate(float);

    unsigned char unknown000[12];
    SExtentView* m_minX;
    SExtentView* m_maxX;
    SExtentView* m_minY;
    SExtentView* m_maxY;
    unsigned char unknown01c[20];
    CVector3 m_position;
    unsigned char unknown040[16];
    CMatrix m_tm;
    float m_detonationTime;
    float m_closest;
    unsigned char unknown098[88];
    CVector3 m_velocity;
    CVector3 m_launchVelocity;
    unsigned char unknown110[16];
    CVector3 m_rotation;
    CVector3 m_rotationBase;
    CVector3 m_rotationRate;
    unsigned char unknown150[80];
    CVolFiber m_sweep;
    unsigned char unknown1f0[8];
    unsigned char m_flags1f8 : 2;
    unsigned char m_gravity : 1;
    unsigned char m_flags1f8b : 4;
    unsigned char m_halted : 1;
};

void CThrownBullet::AttemptUpdate(float elapsed) {
    if (!m_halted) {
        m_closest = 3.4028235e+38f;
        m_velocity = m_launchVelocity;
        if (m_gravity)
            m_velocity.d.v[2] += -0.005455351f * elapsed;
        CVector3 step;
        step.d.v[0] = elapsed * m_velocity.d.v[0];
        step.d.v[1] = elapsed * m_velocity.d.v[1];
        step.d.v[2] = elapsed * m_velocity.d.v[2];
        float distance = step.Length();
        m_rotation.d.v[0] = distance * m_rotationRate.d.v[0];
        m_rotation.d.v[1] = distance * m_rotationRate.d.v[1];
        m_rotation.d.v[2] = distance * m_rotationRate.d.v[2];
        m_rotation.d.v[0] = m_rotation.d.v[0] + m_rotationBase.d.v[0];
        m_rotation.d.v[1] = m_rotation.d.v[1] + m_rotationBase.d.v[1];
        m_rotation.d.v[2] = m_rotation.d.v[2] + m_rotationBase.d.v[2];
        step.d.v[0] = step.d.v[0] + m_position.d.v[0];
        step.d.v[1] = step.d.v[1] + m_position.d.v[1];
        step.d.v[2] = step.d.v[2] + m_position.d.v[2];
        m_tm.SetPos(step);
        m_sweep.Set(m_position, step);
        CVector3 min;
        CVector3 max;
        m_sweep.GetExtents(min, max);
        m_minX->m_value = min.d.v[0] - CONTACT_EPSILON;
        m_minY->m_value = min.d.v[1] - CONTACT_EPSILON;
        m_maxX->m_value = CONTACT_EPSILON + max.d.v[0];
        m_maxY->m_value = CONTACT_EPSILON + max.d.v[1];
    }
}
