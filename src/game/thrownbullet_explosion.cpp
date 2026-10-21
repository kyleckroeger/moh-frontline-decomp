// A fragment of thrownbullet.cpp (0x800d301c): CreateExplosionEffect starts
// a grenade explosion's particle systems at the given matrix (three for a
// normal explosion, a fourth when asked, or four others for the alternative
// effect), each by its CRC id, and a light half a unit above the explosion's
// position. CThrownBullet::AttemptUpdate (unless the bullet is halted) resets
// the closest distance, restarts from the launch velocity (with gravity when
// enabled), advances the spin by the distance travelled, moves the matrix to
// the new position, sweeps the fiber volume from the old position to it and
// stores the fiber's extents (widened by CONTACT_EPSILON) in the scene's
// sweep entries. BeginUpdate updates the light volumes, checks for
// enemies, takes the position from the matrix, turns a rolling grenade's
// matrix along its velocity (front = velocity, up and right from the current
// rightward vector), warns about unknown types, keeps the velocity and spin
// for the next step, refreshes its proximity trigger, counts the detonation
// time down to zero and resets the closest distance. The functions, CParticleSystem::Create, CLight::Create and
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
    void Normalize() {
        float length = Length();
        if (length != 0.0f) {
            d.v[0] *= 1.0f / length;
            d.v[1] *= 1.0f / length;
            d.v[2] *= 1.0f / length;
        }
    }
    void Cross(const CVector3& a, const CVector3& b) {
        d.v[0] = a.d.v[1] * b.d.v[2] - a.d.v[2] * b.d.v[1];
        d.v[1] = a.d.v[2] * b.d.v[0] - a.d.v[0] * b.d.v[2];
        d.v[2] = a.d.v[0] * b.d.v[1] - a.d.v[1] * b.d.v[0];
    }

    union {
        double pair[2];
        float v[4];
    } d;
} __attribute__((aligned(8)));

class CMatrix {
public:
    CVector3 GetPos() const { return *(const CVector3*)m[3]; }
    void SetPos(CVector3);
    void SetFront(CVector3);
    void SetRight(CVector3);
    void SetUp(CVector3);

    float m[4][4];
} __attribute__((aligned(16)));

class CParticleSystem {
    unsigned char m_unknown00[28];

public:
    static CParticleSystem* Create(unsigned long);
    virtual void vf008();
    virtual void vf012();
    virtual void vf016();
    virtual void vf020();
    virtual void vf024();
    virtual void vf028();
    virtual void vf032();
    virtual void vf036();
    virtual void vf040();
    virtual void vf044();
    virtual void vf048();
    virtual void vf052();
    virtual void vf056();
    virtual void vf060();
    virtual void vf064();
    virtual void vf068();
    virtual void vf072();
    virtual void vf076();
    virtual void vf080();
    virtual void vf084();
    virtual void vf088();
    virtual void vf092();
    virtual void vf096();
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
    virtual void vf008();
    virtual void vf012();
    virtual void vf016();
    virtual void vf020();
    virtual void vf024();
    virtual void vf028();
    virtual void vf032();
    virtual void vf036();
    virtual void vf040();
    virtual void vf044();
    virtual void vf048();
    virtual void vf052();
    virtual void vf056();
    virtual void vf060();
    virtual void vf064();
    virtual void vf068();
    virtual void vf072();
    virtual void vf076();
    virtual void vf080();
    virtual void vf084();
    virtual void vf088();
    virtual void vf092();
    virtual void vf096();
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
    virtual void vf008();
    virtual void vf012();
    virtual void vf016();
    virtual void vf020();
    virtual void vf024();
    virtual void vf028();
    virtual void vf032();
    virtual void vf036();
    virtual void vf040();
    virtual void vf044();
    virtual void vf048();
    virtual void vf052();
    virtual void vf056();
    virtual void vf060();
    virtual void vf064();
    virtual void vf068();
    virtual void GetExtents(CVector3&, CVector3&) const;

    unsigned char unknown04[76];
};

/* inferred: a scene sweep extent (its coordinate at +8) */
struct SExtentView {
    unsigned char unknown00[8];
    float m_value;
};

extern const float CONTACT_EPSILON;

class CLightVolumeManager {
public:
    void Update(float);

    unsigned char unknown00[28];
};

class BSGO_Basic;
void ForceUpdateProximityTriggerStatus(BSGO_Basic*, bool);
void DebugMsg(const char*, ...);

class CThrownBullet {
public:
    virtual void vf008();
    virtual void vf012();
    virtual void vf016();
    virtual void vf020();
    virtual void vf024();
    virtual void vf028();
    virtual void vf032();
    virtual void vf036();
    virtual void vf040();
    virtual void vf044();
    virtual void vf048();
    virtual void vf052();
    virtual void vf056();
    virtual void vf060();
    virtual void GetPosition(CVector3&) const;
    virtual void GetRightward(CVector3&) const;
    void AttemptUpdate(float);
    void BeginUpdate(float);
    void CheckEnemies();

    unsigned char unknown004[8];
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
    unsigned char unknown098[12];
    int m_type;
    unsigned char unknown0a8[8];
    unsigned char m_trigger[36];
    CLightVolumeManager m_lights;
    CVector3 m_velocity;
    CVector3 m_launchVelocity;
    unsigned char unknown110[16];
    CVector3 m_rotation;
    CVector3 m_rotationBase;
    CVector3 m_rotationRate;
    unsigned char unknown150[80];
    CVolFiber m_sweep;
    unsigned char unknown1f0[8];
    unsigned char m_resting : 1;
    unsigned char m_flag40 : 1;
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

void CThrownBullet::BeginUpdate(float elapsed) {
    m_lights.Update(elapsed);
    CheckEnemies();
    m_position = m_tm.GetPos();
    switch (m_type) {
    case 0:
    case 1:
    case 2:
    case 8:
    case 10:
        break;
    case 3:
    case 4:
    case 5:
    case 9:
        if (!m_resting && m_velocity.Length() > CONTACT_EPSILON) {
            CVector3 front = m_velocity;
            front.Normalize();
            CVector3 right;
            GetRightward(right);
            CVector3 up;
            up.Cross(right, front);
            right.Cross(front, up);
            right.Normalize();
            up.Normalize();
            m_tm.SetFront(front);
            m_tm.SetRight(right);
            m_tm.SetUp(up);
        }
        break;
    default:
        DebugMsg("WARNING: Unknown thrown bullet type in BeginUpdate.  Type %d\n", m_type);
        break;
    }
    m_launchVelocity = m_velocity;
    m_rotationBase = m_rotation;
    ForceUpdateProximityTriggerStatus((BSGO_Basic*)m_trigger, true);
    if (m_detonationTime > 0.0f) {
        m_detonationTime -= elapsed;
        if (m_detonationTime < 0.0f)
            m_detonationTime = 0.0f;
    }
    m_closest = 3.4028235e+38f;
}
