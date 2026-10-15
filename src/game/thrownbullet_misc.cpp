// A fragment of thrownbullet.cpp (0x800d2384): CThrownBullet's ApplyForceTo
// (the velocity at +240 set to the force's direction times the weapon
// record's first value and 1/512 of the scale, then copied to +256; the
// CVector3 view and MSL-style inline sqrtf are those of player_weapon.cpp),
// Halt (zeroes the velocity at +240 and the vector at +256, last component
// first),
// HandleBulletCollision (a projectile hit sets bit 0x10 of the flag byte at
// +504 to the inverse of bit 0x08), SetDetonationTime (stores the time at
// +144 when the weapon record's byte at +74 allows it), Explode (the same
// flag update), GetScriptBulletType (maps the thrown type at +164 to the
// script's bullet type, warning on an unknown type) and IsVisible (the world
// volume's visibility test, IVolume's slot +68). The classes and functions
// are named by the mangled symbols; the members, the weapon-record and
// volume views and the flag names are inferred, as in thrownbullet.cpp, and
// CBullet declares placeholder virtuals up to AsProjectile (+320, named by
// __vt__7CBullet). CThrownBullet declares MarkForDestruction (defined
// elsewhere) first so its table is not emitted here. The type numbers are
// not named. The rest of the file is not part of this unit.
class CCollision;
class CDrawContext;
class CProjectileBullet;

void DebugMsg(const char*, ...);

namespace std {
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
}

// Two doubles: CVector3's copies move it as a doubleword pair (the view of
// player_weapon.cpp).
union CVector3Pair {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3 operator=(const CVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
        return *this;
    }
    CVector3& operator*=(float f) {
        x *= f;
        y *= f;
        z *= f;
        return *this;
    }
    float LengthSq() const { return x * x + y * y + z * z; }
    void Normalize() {
        float length = std::sqrtf(LengthSq());
        if (length != 0.0f) {
            x *= 1.0f / length;
            y *= 1.0f / length;
            z *= 1.0f / length;
        }
    }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CBullet {
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
    virtual void unknown0d0();
    virtual void unknown0d4();
    virtual void unknown0d8();
    virtual void unknown0dc();
    virtual void unknown0e0();
    virtual void unknown0e4();
    virtual void unknown0e8();
    virtual void unknown0ec();
    virtual void unknown0f0();
    virtual void unknown0f4();
    virtual void unknown0f8();
    virtual void unknown0fc();
    virtual void unknown100();
    virtual void unknown104();
    virtual void unknown108();
    virtual void unknown10c();
    virtual void unknown110();
    virtual void unknown114();
    virtual void unknown118();
    virtual void unknown11c();
    virtual void unknown120();
    virtual void unknown124();
    virtual void unknown128();
    virtual void unknown12c();
    virtual void unknown130();
    virtual void unknown134();
    virtual void unknown138();
    virtual void unknown13c();
    virtual CProjectileBullet* AsProjectile();
};

class IVolume {
public:
    virtual ~IVolume();
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
    virtual bool TestVisibility(CDrawContext&) const;
};

/* inferred: an 80-byte bounding volume */
class ThrownVolumeView : public IVolume {
public:
    unsigned char unknown04[76];
};

/* inferred: the weapon record's detonation-time switch at +74 */
struct ThrownWeaponView {
    float forceScale;
    unsigned char unknown04[70];
    signed char timedDetonation;
};

class CThrownBullet {
public:
    virtual void MarkForDestruction(int); /* defined elsewhere; keeps the table out */
    void ApplyForceTo(CVector3, float);
    void Halt();
    void HandleBulletCollision(CBullet*, const CCollision&);
    void SetDetonationTime(float);
    void Explode();
    int GetScriptBulletType() const;
    bool IsVisible(CDrawContext&) const;

    unsigned char unknown004[140];
    float m_detonationTime;
    unsigned char unknown094[16];
    unsigned int m_type;
    ThrownWeaponView* m_weapon;
    unsigned char unknown0ac[68];
    CVector3 m_velocity;
    CVector3 m_vector100;
    unsigned char unknown110[64];
    ThrownVolumeView m_localVolume;
    ThrownVolumeView m_worldVolume;
    unsigned char unknown1f0[8];
    unsigned char m_flags1f8 : 3;
    unsigned char m_flag10 : 1;
    unsigned char m_flag08 : 1;
    unsigned char m_flags1f8b : 3;
};

void CThrownBullet::ApplyForceTo(CVector3 force, float scale) {
    float strength = scale / 512.0f;
    m_velocity = force;
    m_velocity.Normalize();
    m_velocity *= m_weapon->forceScale * strength;
    m_vector100 = m_velocity;
}

void CThrownBullet::Halt() {
    m_velocity.x = m_velocity.y = m_velocity.z = 0.0f;
    m_vector100.x = m_vector100.y = m_vector100.z = 0.0f;
}

void CThrownBullet::HandleBulletCollision(CBullet* bullet, const CCollision&) {
    if (bullet->AsProjectile())
        m_flag10 = !m_flag08;
}

void CThrownBullet::SetDetonationTime(float time) {
    if (m_weapon->timedDetonation)
        m_detonationTime = time;
}

void CThrownBullet::Explode() {
    m_flag10 = !m_flag08;
}

int CThrownBullet::GetScriptBulletType() const {
    switch (m_type) {
    case 3:
        return 3;
    case 0:
    case 8:
    case 10:
        return 21;
    case 1:
        return 23;
    case 2:
        return 22;
    case 4:
        return 24;
    case 5:
        return 24;
    case 9:
        return 24;
    case 6:
        return 26;
    default:
        DebugMsg("WARNING: Unknown thrown bullet type in GetScriptBulletType.  Type %d\n", m_type);
        return 3;
    }
}

bool CThrownBullet::IsVisible(CDrawContext& context) const {
    return m_worldVolume.TestVisibility(context);
}
