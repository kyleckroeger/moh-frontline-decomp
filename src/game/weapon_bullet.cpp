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
// restarts the fire timer. Update runs the firing state (0: fire when the
// timer runs out, else start reloading; 1: count the timer down and start
// reloading when empty; 2: fire once and refill at once through AddAmo and
// DoneReloading), lets a dropped weapon fall until a line below it hits the
// world (then it is lined up with the ground's normal) and times the muzzle
// flash. The file name is this project's; the original
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
    void Normalize() {
        float length = Length();
        if (length != 0.0f) {
            d.v[0] *= 1.0f / length;
            d.v[1] *= 1.0f / length;
            d.v[2] *= 1.0f / length;
        }
    }
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
    void Translate(CVector3);
    void SetFront(CVector3);
    void SetRight(CVector3);
    void SetUp(CVector3);

    const CVector3& GetRow(int i) const { return *(const CVector3*)m[i]; }
    const CVector3& GetPos() const { return GetRow(3); }

    float m[4][4];
    static bool s_ClassInit;
} __attribute__((aligned(16)));

class CLine3 {
public:
    CLine3(CVector3 start, CVector3 end) { SetSE(start, end); }
    ~CLine3() {}
    void SetSE(CVector3 start, CVector3 end) {
        m_start = start;
        m_end = end;
        m_dir.d.v[0] = m_end.d.v[0] - m_start.d.v[0];
        m_dir.d.v[1] = m_end.d.v[1] - m_start.d.v[1];
        m_dir.d.v[2] = m_end.d.v[2] - m_start.d.v[2];
        m_flag38 = 0;
        m_flag39 = 0;
    }

    CVector3 m_start;
    CVector3 m_end;
    CVector3 m_dir;
    float m_value30;
    float m_value34;
    unsigned char m_flag38;
    unsigned char m_flag39;
} __attribute__((aligned(16)));

class CCollision;
class CVolFiber;
class CVolBox;
class CVolSphere;
class CVolCapsule;
class CCDBObject;

/* IVolume's virtuals in table order up to the fiber test. */
class IVolume {
public:
    virtual ~IVolume();
    virtual IVolume* Create() const;
    virtual void TransformedCopy(const IVolume&, const CMatrix&);
    virtual int TestCollision(const IVolume&, CCollision&, bool) const;
    virtual int TestCollision(const CVolBox&, CCollision&, bool) const;
    virtual int TestCollision(const CVolSphere&, CCollision&, bool) const;
    virtual int TestCollision(const CVolCapsule&, CCollision&, bool) const;
    virtual int TestCollision(const CCDBObject&, CCollision&, bool) const;
    virtual int TestCollision(const CVolFiber&, CCollision&, bool) const;
};

class CVolFiber : public IVolume {
public:
    CVolFiber(const CLine3&);
    CVolFiber(const CVolFiber&);
    virtual ~CVolFiber();

    unsigned char unknown04[12];
    CLine3 m_line;
};

class CCollision {
public:
    CCollision();
    ~CCollision();

    unsigned char unknown00[12];
    float m_t;
    unsigned char unknown10[8];
    CVector3* m_normal;
    unsigned char unknown1c[4];
};

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
    virtual IVolume* GetWorldBoundingVolume(int) const;
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

    unsigned char unknown000[24];
    ISceneNode* m_worldObject;
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
    unsigned char unknown00[4];
    short clipSize;
    short fireDelay;
    float speed;
    short bulletType;
    short bulletFlag;
    short bulletsPerShot;
    short maxReserve;
    unsigned char unknown14[48];
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
    void Update(float, bool);
    /* AddAmo and DoneReloading are defined after Update in weapon.cpp (see
       weapon.cpp); the original file is compiled with deferred inlining, so
       Update inlines them. This fragment is compiled in source order, so their
       bodies are repeated here as inline members. */
    void AddAmo(short count) {
        if (m_reserve + m_loaded == 0)
            reloading = 1;
        m_reserve += count;
        m_reserve = m_reserve > m_properties->maxReserve ? m_properties->maxReserve : m_reserve;
    }
    void DoneReloading() {
        reloading = 0;
        if (infiniteAmmo) {
            m_loaded = m_properties->clipSize;
        } else if (m_loaded + m_reserve < m_properties->clipSize) {
            m_loaded += m_reserve;
            m_reserve = 0;
        } else {
            m_reserve = m_reserve - (m_properties->clipSize - m_loaded);
            m_loaded = m_properties->clipSize;
        }
    }

    unsigned char unknown000[64];
    CMatrix m_tm;
    unsigned char unknown080[224];
    CVector3 m_velocity;
    unsigned char unknown170[272];
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
    unsigned char falling : 1;
    unsigned char infiniteAmmo : 1;
    unsigned char flashShown : 1;
    unsigned char muzzleFlash : 1;
    unsigned char unknown2c0d : 2;
    unsigned char unknown2c1[3];
    float m_flashTime;
    unsigned char unknown2c8[8];
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

void CWeapon::Update(float elapsed, bool paused) {
    fired = false;
    if (g_cheats.m_infiniteAmmo && m_reserve == 0)
        m_reserve = 1;
    switch (m_reloadState) {
    case 0:
        if (m_fireTimer <= 0.0f) {
            if (m_loaded > 0 && !reloading) {
                if (!paused)
                    FireBullet();
            } else if (!reloading && !paused) {
                if (m_owner)
                    m_owner->TriggerScriptEvent(25, (void*)m_reserve, false);
                reloading = true;
            }
        } else {
            m_fireTimer -= elapsed;
        }
        if (m_fireTimer <= 0.0f && m_owner && !paused)
            m_owner->TriggerScriptEvent(21, 0, true);
        break;
    case 1:
        if (m_fireTimer > 0.0f) {
            m_fireTimer -= elapsed;
            if (m_fireTimer <= 0.0f && m_owner && !paused)
                m_owner->TriggerScriptEvent(21, 0, true);
        }
        if (m_loaded <= 0 && !reloading && !paused) {
            if (m_owner)
                m_owner->TriggerScriptEvent(25, (void*)m_reserve, false);
            reloading = true;
        }
        break;
    case 2:
        m_reloadState = 1;
        if (!paused)
            FireBullet();
        if (m_loaded <= 0 && !paused) {
            if (m_owner)
                m_owner->TriggerScriptEvent(25, (void*)m_reserve, false);
            AddAmo(m_properties->clipSize);
            DoneReloading();
        }
        break;
    }
    if (falling) {
        CCollision collision;
        IVolume* world = g_scene.m_worldObject->GetWorldBoundingVolume(0);
        CVector3 bottom = m_tm.GetPos();
        bottom.d.v[2] -= 0.05f;
        CLine3 line(CVector3(bottom.d.v[0], bottom.d.v[1], 1.0f + bottom.d.v[2]), bottom);
        CVolFiber fiber(line);
        if (world->TestCollision(fiber, collision, true) == 1) {
            m_tm.Translate(m_velocity);
            m_velocity.d.v[2] += -0.005455351f * elapsed;
        } else {
            m_velocity.d.v[2] = 0.99f - collision.m_t;
            m_tm.Translate(m_velocity);
            m_velocity.d.v[2] = 0.0f;
            CVector3 right = *collision.m_normal;
            CVector3 front = m_tm.GetRow(1);
            CVector3 up;
            up.Cross(right, front);
            front.Cross(up, right);
            up.Normalize();
            front.Normalize();
            m_tm.SetFront(front);
            m_tm.SetRight(right);
            m_tm.SetUp(up);
            falling = false;
        }
    }
    if (muzzleFlash) {
        flashShown = true;
        if (!fired)
            m_flashTime += elapsed;
    }
}
