// A fragment of particlesystem.cpp (0x8007eef4): CPropertyParticleSystem's
// SetSystemAcceleration (stores +224, then the same flag update as below), the
// weak CParticleSystem::ValidateBoundingBox (a header inline emitted after its
// first caller; the base class holds the vtable and the flag bits, the split
// at the bits is inferred), SetSystemInitialVelocity (stores the velocity at +208, invalidates the
// bounding box through the virtual ValidateBoundingBox, and keeps the moving
// flag unless both the velocity and the acceleration at +224 are below 1e-6
// in squared length; the constant is an entry of the file's .sdata2 pool),
// the empty SetLocalToWorld, SetSeed and the particle acceleration, velocity
// and position getters (each vector set from the particle-property record
// through an inline three-float setter, which explains the reversed loads),
// and GetParticleSize (two vectors with a zero z, 0.0f being an entry of the
// file's .sdata2 pool, and a fifth value). The file name is this project's;
// the original record is particlesystem.cpp. The class names come from the
// mangled symbols; the members, the property-record view and the setter are
// inferred (what the size values mean is unknown), and the class is declared
// with its virtual functions up to ValidateBoundingBox (as in
// __vt__23CPropertyParticleSystem, after 28 bytes of data as for CRenderBin;
// earlier slots are placeholders named by offset); the vector's
// doubleword-pair copy and its zero test are inferred inlines.
class CMatrix;

union CVector3Pair;

class CVector3 {
public:
    CVector3& operator=(const CVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
        return *this;
    }
    float LengthSq() const { return x * x + y * y + z * z; }
    bool IsZero() const {
        bool zero = (float)__fabs(LengthSq()) < 1e-6f;
        return zero;
    }
    void Set(float x_, float y_, float z_) {
        x = x_;
        y = y_;
        z = z_;
    }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CColor {
public:
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

struct ParticleVectorView {
    float x;
    float y;
    float z;
};

struct ParticlePropertiesView {
    unsigned char unknown00[112];
    unsigned char fogFlags;
    CColor color71;
    CColor color75;
    unsigned char unknown79;
    CColor color7a;
    unsigned char unknown7e[14];
    int seed;
    ParticleVectorView position;
    ParticleVectorView positionRange;
    ParticleVectorView velocity;
    ParticleVectorView velocityRange;
    ParticleVectorView acceleration;
    float size0cc;
    float size0d0;
    float size0d4;
    float size0d8;
    float size0dc;
};

// Inferred: two doubles, CVector3's copies move it as a doubleword pair.
union CVector3Pair {
    double pair[2];
    float v[4];
};

class CParticleSystem {
    unsigned char unknown000[28];

public:
    virtual void unknown008();
    virtual void unknown00c();
    virtual void unknown010();
    virtual void unknown014();
    virtual void unknown018();
    virtual void unknown01c();
    virtual void unknown020();
    virtual void unknown024();
    virtual void unknown028();
    virtual void unknown02c();
    virtual void unknown030();
    virtual void unknown034();
    virtual void unknown038();
    virtual void unknown03c();
    virtual void unknown040();
    virtual void unknown044();
    virtual void unknown048();
    virtual void unknown04c();
    virtual void unknown050();
    virtual void unknown054();
    virtual void unknown058();
    virtual void unknown05c();
    virtual void unknown060();
    virtual void unknown064();
    virtual void unknown068();
    virtual void unknown06c();
    virtual void unknown070();
    virtual void unknown074();
    virtual void unknown078();
    virtual void unknown07c();
    virtual void unknown080();
    virtual void unknown084();
    virtual void unknown088();
    virtual void unknown08c();
    virtual void unknown090();
    virtual void unknown094();
    virtual void unknown098();
    virtual void unknown09c();
    virtual void unknown0a0();
    virtual void unknown0a4();
    virtual void unknown0a8();
    virtual void unknown0ac();
    virtual void unknown0b0();
    virtual void unknown0b4();
    virtual void unknown0b8();
    virtual void unknown0bc();
    virtual void unknown0c0();
    virtual void unknown0c4();
    virtual void unknown0c8();
    virtual void unknown0cc();
    virtual void ValidateBoundingBox(bool);

    unsigned char data020[104];
    unsigned char m_active : 1;
    unsigned char m_emitting : 1;
    unsigned char m_boundingBoxValid : 1;
    unsigned char m_moving : 1;
    unsigned char m_flags : 4;
};

class CPropertyParticleSystem : public CParticleSystem {
public:
    void SetLocalToWorld(const CMatrix&);
    void SetSeed(int);
    void SetSystemAcceleration(CVector3);
    void SetSystemInitialVelocity(CVector3);
    void GetParticleAcceleration(CVector3&) const;
    void GetParticleVelocity(CVector3&, CVector3&) const;
    void GetParticlePosition(CVector3&, CVector3&) const;
    void GetParticleSize(CVector3&, CVector3&, float&) const;
    void GetParticleColor(CColor&, CColor&, CColor&, CColor&) const;
    bool GetFogEnable() const;

    unsigned char data08c[68];
    CVector3 m_velocity;
    CVector3 m_acceleration;
    ParticlePropertiesView* m_properties;
};

void CPropertyParticleSystem::SetSystemAcceleration(CVector3 acceleration) {
    m_acceleration = acceleration;
    ValidateBoundingBox(false);
    bool moving = true;
    if (m_velocity.IsZero() && m_acceleration.IsZero())
        moving = false;
    m_moving = moving;
}

__declspec(weak) void CParticleSystem::ValidateBoundingBox(bool valid) {
    m_boundingBoxValid = valid;
}

void CPropertyParticleSystem::SetSystemInitialVelocity(CVector3 velocity) {
    m_velocity = velocity;
    ValidateBoundingBox(false);
    bool moving = true;
    if (m_velocity.IsZero() && m_acceleration.IsZero())
        moving = false;
    m_moving = moving;
}

void CPropertyParticleSystem::SetLocalToWorld(const CMatrix&) {
}

void CPropertyParticleSystem::SetSeed(int seed) {
    m_properties->seed = seed;
}

void CPropertyParticleSystem::GetParticleAcceleration(CVector3& acceleration) const {
    acceleration.Set(m_properties->acceleration.x, m_properties->acceleration.y, m_properties->acceleration.z);
}

void CPropertyParticleSystem::GetParticleVelocity(CVector3& velocity, CVector3& range) const {
    velocity.Set(m_properties->velocity.x, m_properties->velocity.y, m_properties->velocity.z);
    range.Set(m_properties->velocityRange.x, m_properties->velocityRange.y, m_properties->velocityRange.z);
}

void CPropertyParticleSystem::GetParticlePosition(CVector3& position, CVector3& range) const {
    position.Set(m_properties->position.x, m_properties->position.y, m_properties->position.z);
    range.Set(m_properties->positionRange.x, m_properties->positionRange.y, m_properties->positionRange.z);
}

void CPropertyParticleSystem::GetParticleSize(CVector3& size, CVector3& range, float& value) const {
    size.Set(m_properties->size0cc, m_properties->size0d4, 0.0f);
    range.Set(m_properties->size0d0, m_properties->size0d8, 0.0f);
    value = m_properties->size0dc;
}
