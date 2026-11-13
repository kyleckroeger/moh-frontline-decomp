// weapon.cpp: CWeapon, the whole file, with its vtable and the RTTI records of its chain
// (IDestructible .. CWeapon): the weak copies of its inline AsWeaponObject, GetPosition and
// Init; the muzzle-flash status; the infinite-ammunition flag, cooking, the sound type and
// the bullet speed; FireBullet and Update; Draw, BeginUpdate and the ammunition helpers
// (reloading, AddAmo, Shoot), the bullet sprite, Shutdown and Init; the destructor, the
// constructor and the array operator new.
//
// The names come from the mangled symbols; the class chain and the vtable from
// __RTTI__7CWeapon and __vt__7CWeapon (ISceneNode's and IMovingSceneNode's slots from the
// shared headers, CStaticObject's from +0x124 as __vt__13CStaticObject).
//
// Inferred, not in any mangled name: the data members (named by offset or role; CStaticObject
// is hierobject.cpp's declaration, the matrix at +0x40 and the velocity at +0x160 being the
// ones used here), the result types, the views of the weapon properties, the sound event,
// the cheats, the collision, the bullet (AsThrown at +0x13c) and the hierarchy object (only
// its AsHierTankObject slot), and the file-private CVector3 and CMatrix (below), declared
// before the shared scene-node headers so those use them: a vector of four floats copied
// through a double-pair union (both doublewords loaded, then stored) and a matrix of four rows
// returned by reference. The weapon's holder is its owner scene node (its AsPlayerObject and
// GetCollisionId slots). The unit is compiled -inline auto,deferred, which emits the
// functions in reverse source order (and lets Update inline the later AddAmo and
// DoneReloading), so they are written last to first.
#define GAME_CVECTOR3_H
#define GAME_CMATRIX_H
#include "game/msl_sqrtf.h"

/* inferred: file-private vector and matrix views */
union CVector3Pair {
    double pair[2];
    float v[4];
};

struct CVector3Base {
    float x, y, z, w;
} __attribute__((aligned(8)));

class CVector3 : public CVector3Base {
public:
    CVector3() {}
    CVector3(const CVector3& other) : CVector3Base(other) {}
    CVector3& operator=(const CVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
        return *this;
    }
    CVector3(float ax, float ay, float az) { x = ax; y = ay; z = az; }
    void Set(float ax, float ay, float az) { x = ax; y = ay; z = az; }
    void Cross(const CVector3& a, const CVector3& b) {
        x = a.y * b.z - a.z * b.y;
        y = a.z * b.x - a.x * b.z;
        z = a.x * b.y - a.y * b.x;
    }
    float Length() const { return sqrtf(x * x + y * y + z * z); }
    void Normalize() {
        float length = Length();
        if (length != 0.0f) {
            x *= 1.0f / length;
            y *= 1.0f / length;
            z *= 1.0f / length;
        }
    }

} __attribute__((aligned(8)));

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    void Ident();
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

#include "game/ISceneNode.h"
#include "game/IMovingSceneNode.h"
#include "game/CScene.h"

class CThrownObject;
class CWeapon;
class CTankObject;
class CThrownBullet;
enum EBSEventEnum {};

extern CScene g_scene;
void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);
float MathFunRandomReal(float, float);
unsigned short GetAzimuth(CVector3, const CPlayerObject*);
void MakeNoise(ISceneNode*, int, float, int);
void AEMS_SendEvent(void*);
void* DWI_alloc(const char*, int, int);
struct BSGO_Basic;
void ForceUpdateProximityTriggerStatus(BSGO_Basic*, bool);
extern void* g_pStaticObjectMuzzleFlashNPC;

/* inferred views */
class CQuaternion {
public:
    float w;
    float x;
    float y;
    float z;
};

class CLine3 {
public:
    CLine3(CVector3 start, CVector3 end) { SetSE(start, end); }
    ~CLine3() {}
    void SetSE(CVector3 start, CVector3 end) {
        m_start = start;
        m_end = end;
        m_dir.x = m_end.x - m_start.x;
        m_dir.y = m_end.y - m_start.y;
        m_dir.z = m_end.z - m_start.z;
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

class CVolFiber;
class CVolBox;
class CVolSphere;
class CVolCapsule;
class CCDBObject;

/* IVolume's virtuals in table order up to the fiber test */
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

class CStaticMesh;
struct MotionKey;

class CLightVolumeManager {
public:
    unsigned char unknown00[4];
};

class CStaticObject : public IMovingSceneNode {
public:
    struct MotionFrame;

    CStaticObject();
    CStaticObject(CStaticMesh*, int, int, int, int, int, MotionFrame*);
    virtual void MarkForDestruction(int);
    virtual ~CStaticObject();
    virtual void Destroy();
    virtual void BeginUpdate(float);
    virtual void AttemptUpdate(float);
    virtual void OnCollision(const CCollision&);
    virtual void CommitUpdate();
    virtual void Draw(CDrawContext&);
    virtual IVolume* GetLocalBoundingVolume(EVolumeType) const;
    virtual IVolume* GetWorldBoundingVolume(EVolumeType) const;
    virtual void GetTMLocalToWorld(CMatrix&) const;
    virtual void GetPosition(CVector3&) const;
    virtual void GetRightward(CVector3&) const;
    virtual void GetForward(CVector3&) const;
    virtual void GetUpward(CVector3&) const;
    virtual bool IsVisible(CDrawContext&) const;
    virtual EClsnId GetCollisionId() const;
    virtual void SetCollisionId(EClsnId id) { m_collisionId = id; }
    virtual BSObject* GetScriptObject() const { return m_script; }
    virtual void TriggerScriptEvent(int event, void* data, bool flag) {
        BSObjectTriggerEvent(m_script, event, data, 0, flag);
    }
    virtual void HandleBulletCollision(CBullet*, const CCollision&);
    virtual CStaticObject* AsStaticObject() { return this; }
    virtual const CStaticObject* AsStaticObject() const { return this; }
    virtual bool IsDrawEnabled() const { return m_drawEnabled; }
    virtual void SetAttachedLight(CLight*, CVector3);
    virtual CLight* GetAttachedLight() const { return m_attachedLight; }
    virtual void SetTMLocalToWorld(const CMatrix&);
    virtual void SetPosition(CVector3);
    virtual void SetBasis(CVector3, CVector3, CVector3);
    virtual void EnterLightVolume(BPDLightVolume*);
    virtual void ExitLightVolume(BPDLightVolume*);
    virtual BPDLightVolume* GetLightVolume();
    virtual void Init();
    virtual void SetScript(BSObject* script) { m_script = script; }
    virtual int GetObjectType() const;
    virtual void TranslateFromScript(CVector3&, CVector3&, EBSEventEnum);
    virtual void RotateFromScript(CVector3&, CVector3&, EBSEventEnum);
    virtual void ScaleFromScript(float, float, EBSEventEnum);
    virtual void StartMotionPlayback(int, int, EBSEventEnum);
    virtual CThrownObject* AsThrownObject() { return 0; }
    virtual CWeapon* AsWeaponObject() { return 0; }
    virtual CTankObject* AsHierTankObject() { return 0; }
    virtual void AttemptScriptMotionUpdate(float);
    virtual void AttemptAnimMotionUpdate(float);
    virtual void AttemptPathMotionUpdate(float);
    virtual void CommitScriptMotionUpdate();
    virtual void CommitAnimMotionUpdate();
    virtual void CommitPathMotionUpdate();

    unsigned char unknown024[12];
    CStaticMesh* m_mesh;
    unsigned char unknown034[12];
    CMatrix m_matrix;
    unsigned char unknown080[88];
    float m_speed;
    unsigned char unknown0dc[40];
    int m_pathState;
    unsigned char unknown108[4];
    IVolume* m_localVolume;
    IVolume* m_volume110;
    IVolume* m_worldVolume;
    BSObject* m_script;
    unsigned char unknown11c[4];
    CVector3 m_position;
    CVector3 m_keyOffset;
    CQuaternion m_keyRotation;
    CQuaternion m_rotation;
    CVector3 m_velocity;
    CVector3 m_pathPosition;
    CQuaternion m_animRotation;
    CQuaternion m_scriptRotation;
    CQuaternion m_nextRotation;
    unsigned char unknown1b0[8];
    float m_moveTime;
    float m_turnTime;
    unsigned char unknown1c0[4];
    int m_moveEvent;
    int m_turnEvent;
    unsigned char unknown1cc[4];
    float m_moveStep;
    float m_turnStep;
    unsigned char unknown1d8[4];
    EClsnId m_collisionId;
    bool m_drawEnabled : 1;
    bool m_scriptMove : 1;
    bool m_scriptTurn : 1;
    bool m_flag10 : 1;
    bool m_transformDelta : 1;
    bool m_flag04 : 1;
    unsigned char m_flags1e0b : 2;
    unsigned char unknown1e1[23];
    float m_pathSpeed;
    unsigned char unknown1fc[12];
    int m_frameCount;
    unsigned char unknown20c[4];
    int m_ticksPerFrame;
    unsigned char unknown214[4];
    int m_keyEvent;
    int m_startKey;
    int m_lastKey;
    int m_keyCount;
    float m_keyTime;
    int m_keyFrames;
    int m_key;
    float m_rotationRate;
    CQuaternion m_targetRotation;
    MotionKey* m_keys;
    CLight* m_attachedLight;
    unsigned char unknown250[16];
    CLightVolumeManager m_lightVolumes;
    unsigned char unknown264[28];
};


/* the hierarchy object: only its AsHierTankObject slot is used here */
class CHierObject : public CStaticObject {
};

/* the bullet: AsThrown at +0x13c */
class CBullet : public IMovingSceneNode {
public:
    virtual void unknown124();
    virtual void unknown128();
    virtual void unknown12c();
    virtual void unknown130();
    virtual void unknown134();
    virtual void unknown138();
    virtual CThrownBullet* AsThrown();
};

class CThrownBullet {
public:
    void SetDetonationTime(float);
};

class CSoundTable {
public:
    struct SSoundParms {
        unsigned char unknown00[16];
    };
    bool GetSoundEvent(int, int, int, SSoundParms*) const;

    unsigned char unknown00[4];
};

extern CSoundTable g_soundMapSingleton;
void PlayScriptedSound(ISceneNode*, CVector3, const CSoundTable::SSoundParms&, int, BSObject*);

struct CheatsView {
    unsigned char unknown00;
    unsigned char m_infiniteAmmo;
    unsigned char unknown02[18];
};

extern CheatsView g_cheats;

/* the sound event */
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

/* the weapon properties record */
struct WeaponPropertiesView {
    unsigned char unknown00[4];
    short clipSize;
    short fireDelay;
    float speed;
    short bulletType;
    short bulletFlag;
    short bulletsPerShot;
    short maxReserve;
    unsigned char unknown14[20];
    void* mesh;
    unsigned char unknown2c[24];
    float spread;
    short soundType;
    unsigned char unknown4a[2];
    float noise;
};

class CBulletFactory {
public:
    bool CanBulletTypeCook(bool, int);
    float GetBulletWeight(bool, int);
    CBullet* CreateBullet(int, bool, CVector3, CVector3, float, EClsnId, float, ISceneNode*);
    void* GetBulletSprite(bool, int);
};

extern CBulletFactory* g_pBulletFactory;

class BSGO_Basic {
    int m_field0;
    int m_field4;
    int m_field8;

public:
    BSGO_Basic() : m_field0(0), m_field4(0), m_field8(0) {}
    virtual void unknown08();
};

class BSGO_Dummy : public BSGO_Basic {
public:
    virtual void unknown08();
};

enum EWeaponTypes {};
enum EWeaponShootType {};
struct WeaponProperties_struct;

class CWeapon : public CStaticObject {
public:
    CWeapon();
    virtual ~CWeapon();
    virtual void BeginUpdate(float);
    virtual void Draw(CDrawContext&);
    virtual CWeapon* AsWeaponObject() { return this; }
    virtual void GetPosition(CVector3& v) const { v = m_position; }
    virtual void Init() { CStaticObject::Init(); }

    static void* operator new[](unsigned long, char*);
    unsigned char GetMuzzleFlashStatus();
    void SetInfiniteAmo(bool);
    bool CanCook() const;
    int GetWeaponSoundType() const;
    float GetBulletSpeed() const;
    void FireBullet();
    void Update(float, bool);
    void DoneReloading();
    void ReloadSingle();
    void StartReloading();
    void Empty();
    void AddAmo(short);
    void Shoot(EWeaponShootType, float, float);
    void* GetBulletSprite() const;
    void Shutdown();
    void Init(EWeaponTypes, WeaponProperties_struct*, ISceneNode*, short);

    WeaponPropertiesView* m_properties;
    short m_reserve;
    short m_loaded;
    float m_fireTimer;
    int m_reloadState;
    float m_shootValue;
    int m_clsnId;
    EWeaponTypes m_type;
    float m_cookTime;
    CVector3 m_direction;
    CVector3 m_position;
    unsigned char reloading : 1;
    unsigned char fired : 1;
    unsigned char falling : 1;
    unsigned char infiniteAmmo : 1;
    unsigned char flashShown : 1;
    unsigned char muzzleFlash : 1;
    unsigned char unknown2c0 : 2;
    unsigned char unknown2c1[3];
    float m_flashTime;
    float m_flashDuration;
    void* m_muzzleFlash;
    float m_speedScale;
    ISceneNode* m_owner;
    BSGO_Dummy m_scriptObject;
    CWeapon* m_unknown2e8;
    int m_unknown2ec;
    int m_unknown2f0;
    int m_unknown2f4;
    int m_unknown2f8;
};
void* CWeapon::operator new[](unsigned long size, char* name) {
    return DWI_alloc(name, size, 1024);
}

CWeapon::CWeapon() {
    m_reserve = 0;
    m_loaded = 0;
    m_fireTimer = 0.0f;
    m_reloadState = 1;
    m_properties = 0;
}

CWeapon::~CWeapon() {
}

void CWeapon::Init(EWeaponTypes type, WeaponProperties_struct* properties, ISceneNode* holder, short reserve) {
    CStaticObject::Init();
    m_fireTimer = 0.0f;
    m_reloadState = 1;
    fired = 0;
    m_flashDuration = 4.0f;
    m_owner = holder;
    infiniteAmmo = 0;
    m_properties = (WeaponPropertiesView*)properties;
    m_type = type;
    m_mesh = (CStaticMesh*)((WeaponPropertiesView*)properties)->mesh;
    flashShown = 0;
    muzzleFlash = 0;
    m_flashTime = 0.0f;
    falling = 0;
    m_speedScale = 1.0f;
    m_reserve = reserve > m_properties->maxReserve ? m_properties->maxReserve : reserve;
    DoneReloading();
    m_position.z = 0.0f;
    m_position.y = 0.0f;
    m_position.x = 0.0f;
    if (m_owner) {
        switch (m_owner->GetCollisionId()) {
        case 2:
        case 13:
        case 23:
            if (m_properties->bulletFlag)
                m_clsnId = 4;
            else
                m_clsnId = 10;
            break;
        case 1:
            if (m_properties->bulletFlag)
                m_clsnId = 3;
            else
                m_clsnId = 9;
            break;
        case 5:
            if (m_properties->bulletFlag)
                m_clsnId = 6;
            else
                m_clsnId = 11;
            break;
        default:
            m_clsnId = -1;
            break;
        }
    }
    switch (m_type) {
    case 35:
    case 36:
    case 37:
    case 38:
    case 40:
        m_muzzleFlash = 0;
        break;
    default:
        m_muzzleFlash = g_pStaticObjectMuzzleFlashNPC;
        break;
    }
    m_unknown2f0 = 0;
    m_unknown2ec = 0;
    m_unknown2f8 = 0;
    m_unknown2f4 = 0;
    *(int*)&m_scriptObject = 0;
    m_unknown2e8 = this;
}

void CWeapon::Shutdown() {
}

void* CWeapon::GetBulletSprite() const {
    return g_pBulletFactory->GetBulletSprite(m_properties->bulletFlag != 0, m_properties->bulletType);
}

void CWeapon::Shoot(EWeaponShootType type, float value, float value2) {
    m_reloadState = type;
    m_shootValue = value;
    m_cookTime = value2;
}

void CWeapon::AddAmo(short count) {
    if (m_reserve + m_loaded == 0)
        reloading = 1;
    m_reserve += count;
    m_reserve = m_reserve > m_properties->maxReserve ? m_properties->maxReserve : m_reserve;
}

void CWeapon::Empty() {
    m_reserve = 0;
    m_loaded = 0;
}

void CWeapon::StartReloading() {
    m_reloadState = 1;
    reloading = 1;
}

void CWeapon::ReloadSingle() {
    if (m_loaded < m_properties->clipSize && m_reserve > 0) {
        m_loaded++;
        m_reserve--;
    }
}

void CWeapon::DoneReloading() {
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

void CWeapon::BeginUpdate(float elapsed) {
    if (falling)
        ForceUpdateProximityTriggerStatus(&m_scriptObject, true);
    CStaticObject::BeginUpdate(elapsed);
}

void CWeapon::Draw(CDrawContext& context) {
    if (m_mesh && !m_owner->AsPlayerObject())
        CStaticObject::Draw(context);
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
        IVolume* world = g_scene.m_world->GetWorldBoundingVolume((ISceneNode::EVolumeType)0);
        CVector3 bottom = m_matrix.GetPos();
        bottom.z -= 0.05f;
        CLine3 line(CVector3(bottom.x, bottom.y, 1.0f + bottom.z), bottom);
        CVolFiber fiber(line);
        if (world->TestCollision(fiber, collision, true) == 1) {
            m_matrix.Translate(m_velocity);
            m_velocity.z += -0.005455351f * elapsed;
        } else {
            m_velocity.z = 0.99f - collision.m_t;
            m_matrix.Translate(m_velocity);
            m_velocity.z = 0.0f;
            CVector3 right = *collision.m_normal;
            CVector3 front = m_matrix.GetRow(1);
            CVector3 up;
            up.Cross(right, front);
            front.Cross(up, right);
            up.Normalize();
            front.Normalize();
            m_matrix.SetFront(front);
            m_matrix.SetRight(right);
            m_matrix.SetUp(up);
            falling = false;
        }
    }
    if (muzzleFlash) {
        flashShown = true;
        if (!fired)
            m_flashTime += elapsed;
    }
}

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
        double ax = __fabs(direction.x);
        double az = __fabs(direction.z);
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
            direction.z = 0.0f;
            direction.z = 0.577f * direction.Length();
        }
        bullet = g_pBulletFactory->CreateBullet(m_properties->bulletType, m_properties->bulletFlag != 0, start,
                                                         direction, m_properties->speed * m_speedScale, (EClsnId)m_clsnId,
                                                         m_shootValue, m_owner);
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
        position.x = position.x - listenerPosition.x;
        position.y = position.y - listenerPosition.y;
        position.z = position.z - listenerPosition.z;
        distance = (int)position.Length() << 8;
        if (distance > 32765)
            distance = 32765;
        event.distance = distance;
        bullet->GetPosition(position);
        event.azimuth2 = GetAzimuth(position, listener);
        position.x = position.x - listenerPosition.x;
        position.y = position.y - listenerPosition.y;
        position.z = position.z - listenerPosition.z;
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

float CWeapon::GetBulletSpeed() const {
    float weight = g_pBulletFactory->GetBulletWeight(m_properties->bulletFlag != 0, m_properties->bulletType);
    return m_properties->speed * weight;
}

int CWeapon::GetWeaponSoundType() const {
    return m_properties->soundType;
}

bool CWeapon::CanCook() const {
    return g_pBulletFactory->CanBulletTypeCook(m_properties->bulletFlag != 0, m_properties->bulletType);
}

void CWeapon::SetInfiniteAmo(bool infinite) {
    infiniteAmmo = infinite;
}

/* ---- definitions ---- */
unsigned char CWeapon::GetMuzzleFlashStatus() {
    if (m_flashTime > m_flashDuration) {
        flashShown = 0;
        muzzleFlash = 0;
        muzzleFlash = 0;
        m_flashTime = 0.0f;
    }
    return muzzleFlash;
}
