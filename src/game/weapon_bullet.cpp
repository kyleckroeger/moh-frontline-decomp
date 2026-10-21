// A fragment of weapon.cpp (0x800d4fac): CWeapon::SetInfiniteAmo (a flag
// bit), CanCook and GetBulletSpeed (asking the bullet factory about the
// weapon's bullet type; the speed is the weapon's speed scaled by the bullet
// weight), GetWeaponSoundType and FireBullet. FireBullet creates the shot's
// bullets: each starts at the weapon's position along its direction turned by
// a random spread (around an axis perpendicular to the direction, then a
// random roll); a player's grenade (bullet types 3 and 4) is pitched up 20
// degrees, another owner's grenade gets a fixed upward slope; a thrown bullet
// gets the cooking time as its detonation time, another bullet lights the
// muzzle flash. It then takes the ammunition (unless the cheat is on), plays
// the shot (a scripted sound for some weapon types fired from a tank, else
// the weapon's sound event with the azimuths and distances of the owner and
// the last bullet as heard by the player), makes noise for the AI and
// restarts the fire timer. The file name is this project's; the original
// record is weapon.cpp; GetMuzzleFlashStatus before these uses a pooled
// .sdata2 constant. The classes, functions and globals are named by the
// mangled symbols and the virtuals by the vtables; CWeapon (as in weapon.cpp),
// the weapon properties, the event and cheat views and the vector helpers are
// inferred views (members at their offsets, names not original), and the
// result types are inferred.
// CVector3 view: four floats, 8-byte aligned, overlaid with two doubles. The
// union is inferred from the code, not the original declaration: the position
// is copied as two lfd/stfd pairs, which an implicit copy through the double
// pair reproduces. sqrtf is the MSL inline.
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
    CVector3() {}
    CVector3(float x, float y, float z) { d.v[0] = x; d.v[1] = y; d.v[2] = z; }
    void Cross(const CVector3& a, const CVector3& b) {
        d.v[0] = a.d.v[1] * b.d.v[2] - a.d.v[2] * b.d.v[1];
        d.v[1] = a.d.v[2] * b.d.v[0] - a.d.v[0] * b.d.v[2];
        d.v[2] = a.d.v[0] * b.d.v[1] - a.d.v[1] * b.d.v[0];
    }
    float Length() const { return sqrtf(d.v[0] * d.v[0] + d.v[1] * d.v[1] + d.v[2] * d.v[2]); }
    union {
        double pair[2];
        float v[4];
    } d;
} __attribute__((aligned(8)));

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    void BuildRot(CVector3, float);
    void Rotate(CVector3, float);
    void TransformVector(CVector3&, CVector3) const;

    float m[4][4];
    static bool s_ClassInit;
} __attribute__((aligned(16)));

class CHierObject;
class CPlayerObject;
class CThrownBullet;

class ISceneNode {
public:
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
    virtual void GetPosition(CVector3&) const;
    virtual void unknown068();
    virtual void unknown072();
    virtual void unknown076();
    virtual void unknown080();
    virtual void unknown084();
    virtual void unknown088();
    virtual void unknown092();
    virtual void unknown096();
    virtual void TriggerScriptEvent(int, void*, bool);
    virtual void unknown104();
    virtual void unknown108();
    virtual void unknown112();
    virtual void unknown116();
    virtual void unknown120();
    virtual CHierObject* AsHierObject();
    virtual void unknown128();
    virtual void unknown132();
    virtual void unknown136();
    virtual void unknown140();
    virtual void unknown144();
    virtual void unknown148();
    virtual void unknown152();
    virtual CPlayerObject* AsPlayerObject();
};

class CHierObject : public ISceneNode {
public:
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
    virtual void unknown264();
    virtual void unknown268();
    virtual void unknown272();
    virtual void unknown276();
    virtual void unknown280();
    virtual void unknown284();
    virtual void unknown288();
    virtual void unknown292();
    virtual void unknown296();
    virtual void unknown300();
    virtual void unknown304();
    virtual void unknown308();
    virtual void unknown312();
    virtual void unknown316();
    virtual void unknown320();
    virtual void unknown324();
    virtual void* AsHierTankObject();
};

class CBullet : public ISceneNode {
public:
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
    virtual void unknown264();
    virtual void unknown268();
    virtual void unknown272();
    virtual void unknown276();
    virtual void unknown280();
    virtual void unknown284();
    virtual void unknown288();
    virtual void unknown292();
    virtual void unknown296();
    virtual void unknown300();
    virtual void unknown304();
    virtual void unknown308();
    virtual void unknown312();
    virtual CThrownBullet* AsThrown();
};

class CThrownBullet {
public:
    void SetDetonationTime(float);
};

class CScene {
public:
    CPlayerObject* GetPlayer(int) const;

    unsigned char unknown00[16];
};

extern CScene g_scene;

struct BSObject;

class CSoundTable {
public:
    struct SSoundParms {
        unsigned char unknown00[16];
    };

    bool GetSoundEvent(int, int, int, SSoundParms*) const;

    unsigned char unknown00[4];
};

extern CSoundTable g_soundMapSingleton;

struct CheatsView {
    unsigned char unknown00;
    unsigned char m_infiniteAmmo;
    unsigned char unknown02[18];
};

extern CheatsView g_cheats;

enum EClsnId {};

float MathFunRandomReal(float, float);
unsigned short GetAzimuth(CVector3, const CPlayerObject*);
void PlayScriptedSound(ISceneNode*, CVector3, const CSoundTable::SSoundParms&, int, BSObject*);
void MakeNoise(ISceneNode*, int, float, int);
void AEMS_SendEvent(void*);

struct WeaponPropertiesView {
    unsigned char unknown00[6];
    short fireDelay;
    float speed;
    short bulletType;
    short bulletFlag;
    short bulletsPerShot;
    unsigned char unknown12[50];
    float spread;
    short soundType;
    unsigned char unknown4a[2];
    float noise;
};

class CStaticObject {
public:
    void Init();
};

class CBulletFactory {
public:
    bool CanBulletTypeCook(bool, int);
    float GetBulletWeight(bool, int);
    CBullet* CreateBullet(int, bool, CVector3, CVector3, float, EClsnId, float, ISceneNode*);
};

extern CBulletFactory* g_pBulletFactory;

class CWeapon {
public:
    CWeapon* AsWeaponObject();
    void GetPosition(CVector3&) const;
    void Init();
    void SetInfiniteAmo(bool);
    bool CanCook() const;
    int GetWeaponSoundType() const;
    float GetBulletSpeed() const;
    void FireBullet();

    unsigned char unknown000[640];
    WeaponPropertiesView* m_properties;
    short m_reserve;
    short m_loaded;
    float m_fireTimer;
    int m_reloadState;
    float m_f290;
    EClsnId m_clsnId;
    int m_type;
    float m_cookTime;
    CVector3 m_direction;
    CVector3 m_position;
    unsigned char reloading : 1;
    unsigned char fired : 1;
    unsigned char unknown2c0b : 1;
    unsigned char infiniteAmmo : 1;
    unsigned char unknown2c0c : 1;
    unsigned char muzzleFlash : 1;
    unsigned char unknown2c0d : 2;
    unsigned char unknown2c1[15];
    float m_speedScale;
    ISceneNode* m_owner;
};

void CWeapon::SetInfiniteAmo(bool infinite) {
    infiniteAmmo = infinite;
}

bool CWeapon::CanCook() const {
    return g_pBulletFactory->CanBulletTypeCook(m_properties->bulletFlag != 0, m_properties->bulletType);
}

int CWeapon::GetWeaponSoundType() const {
    return m_properties->soundType;
}

float CWeapon::GetBulletSpeed() const {
    float weight = g_pBulletFactory->GetBulletWeight(m_properties->bulletFlag != 0, m_properties->bulletType);
    return m_properties->speed * weight;
}

/* inferred: the sound event views */
struct WeaponSoundEvent {
    int type;
    int sound;
    int unknown08;
    int azimuth;
    int azimuth2;
    int distance;
    int distance2;
    int count;
};

void CWeapon::FireBullet() {
    CBullet* bullet = 0;
    CMatrix rotation;
    WeaponSoundEvent event;
    CVector3 start;
    CVector3 direction;
    CVector3 axis;
    int i;

    for (i = 0; i < m_properties->bulletsPerShot; i++) {
        start = m_position;
        direction = m_direction;
        double ax = __fabs(direction.d.v[0]);
        double az = __fabs(direction.d.v[2]);
        if (az < ax)
            axis.Cross(direction, CVector3(0.0f, 0.0f, 1.0f));
        else
            axis.Cross(CVector3(1.0f, 0.0f, 0.0f), direction);
        float angle = MathFunRandomReal(m_properties->spread, -m_properties->spread);
        rotation.BuildRot(axis, angle);
        float roll = MathFunRandomReal(0.0f, 3.1415927f);
        rotation.Rotate(m_direction, roll);
        CPlayerObject* player = m_owner->AsPlayerObject();
        if (player) {
            m_owner->TriggerScriptEvent(24, 0, false);
            if (m_properties->bulletFlag == 0 && (m_properties->bulletType == 4 || m_properties->bulletType == 3)) {
                CVector3 right;
                right.Cross(direction, CVector3(0.0f, 0.0f, 1.0f));
                CVector3 up;
                up.Cross(right, direction);
                CVector3 pitch;
                pitch.Cross(direction, up);
                rotation.BuildRot(pitch, 0.3490656f);
            }
        }
        rotation.TransformVector(direction, m_direction);
        if (m_owner != (ISceneNode*)player && m_properties->bulletFlag == 0 &&
            (m_properties->bulletType == 4 || m_properties->bulletType == 3)) {
            direction.d.v[2] = 0.0f;
            direction.d.v[2] = 0.577f * direction.Length();
        }
        bullet = g_pBulletFactory->CreateBullet(m_properties->bulletType, m_properties->bulletFlag != 0, start,
                                                         direction, m_properties->speed * m_speedScale, m_clsnId,
                                                         m_f290, m_owner);
        if (!bullet) {
            m_fireTimer += m_properties->fireDelay;
            return;
        }
        if (m_properties->bulletFlag == 0) {
            CThrownBullet* thrown = bullet->AsThrown();
            if (m_cookTime >= 0.0)
                thrown->SetDetonationTime(m_cookTime);
        } else {
            muzzleFlash = true;
        }
    }
    if (!g_cheats.m_infiniteAmmo)
        m_loaded--;
    fired = true;
    event.azimuth = 0;
    event.azimuth2 = 0;
    event.distance = 0;
    event.distance2 = 0;
    int sound = m_properties->soundType;
    if (m_owner && m_owner->AsHierObject() && m_owner->AsHierObject()->AsHierTankObject()) {
        if (m_type == 44 || m_type == 31 || m_type == 32 || m_type == 30) {
            CSoundTable::SSoundParms parms;
            if (g_soundMapSingleton.GetSoundEvent(519, -1, 59, &parms)) {
                CVector3 position;
                m_owner->GetPosition(position);
                PlayScriptedSound(m_owner, position, parms, -1, 0);
            }
            goto done;
        }
    }
    if (m_owner->AsPlayerObject()) {
        event.azimuth = 0;
        event.azimuth2 = 0;
        event.distance = 0;
        event.distance2 = 0;
    } else if (m_properties->bulletFlag != 0 || m_type == 44 || m_type == 31 || m_type == 32 || m_type == 30) {
        CPlayerObject* listener = g_scene.GetPlayer(0);
        CVector3 position;
        CVector3 listenerPosition;
        int distance;
        m_owner->GetPosition(position);
        event.azimuth = GetAzimuth(position, listener);
        ((ISceneNode*)listener)->GetPosition(listenerPosition);
        position.d.v[0] = position.d.v[0] - listenerPosition.d.v[0];
        position.d.v[1] = position.d.v[1] - listenerPosition.d.v[1];
        position.d.v[2] = position.d.v[2] - listenerPosition.d.v[2];
        distance = (int)position.Length() << 8;
        if (distance > 32765)
            distance = 32765;
        event.distance = distance;
        bullet->GetPosition(position);
        event.azimuth2 = GetAzimuth(position, listener);
        position.d.v[0] = position.d.v[0] - listenerPosition.d.v[0];
        position.d.v[1] = position.d.v[1] - listenerPosition.d.v[1];
        position.d.v[2] = position.d.v[2] - listenerPosition.d.v[2];
        distance = (int)position.Length() << 8;
        if (distance > 32765)
            distance = 32765;
        event.distance2 = distance;
    }
    event.type = 1;
    event.sound = sound;
    event.unknown08 = 1;
    event.count = m_loaded;
    {
        int stop = 9;
        AEMS_SendEvent(&stop);
    }
    AEMS_SendEvent(&event);
done:
    if (m_owner->AsPlayerObject() && m_properties->bulletType == 3 && m_properties->bulletFlag == 0 && m_cookTime < 0.0f) {
        event.type = 1;
        event.sound = 24;
        event.unknown08 = 1;
        event.count = m_loaded;
        event.distance = 0;
        event.distance2 = 0;
        event.azimuth = 0;
        event.azimuth2 = 0;
        AEMS_SendEvent(&event);
    }
    MakeNoise(m_owner, 52, m_properties->noise, 10);
    m_fireTimer += m_properties->fireDelay;
}
