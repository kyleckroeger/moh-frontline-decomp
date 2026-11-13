// thrownbullet.cpp: CThrownBullet, the whole file, with its vtable and the RTTI records of its
// chain (IDestructible .. CThrownBullet): the weak copies of its inline Init, bounding volumes,
// collision switch, attached light, damage, cast and velocity; the light-volume forwards and
// SomebodyCaughtYou; CheckEnemies, ApplyForceTo, Halt, the bullet hit, the detonation time,
// Explode, the script bullet type, IsVisible and Draw; the collision id; CommitUpdate,
// CreateExplosionEffect, AttemptUpdate, BeginUpdate, ConstrainVelocity and Penetrate;
// OnCollision, Destroy, MarkForDestruction, Shutdown and Init (with its function-static power
// range); the subject callback SECB_NullifyPointer, the destructor, the constructor and the
// array operator new; and the multiplayer draw delay MP_THROWNBULLET_DRAW_DELAY.
//
// The names come from the mangled symbols; the class chain and the vtable from
// __RTTI__13CThrownBullet and __vt__13CThrownBullet (ISceneNode's and IMovingSceneNode's slots
// as the shared headers, CBullet's from +0x124 as __vt__7CBullet, IVolume's as __vt__9CVolFiber).
//
// Inferred, not in any mangled name: the data members (named by offset or role), the result
// types, the inline helpers (the subscription setter, the warning loop, the team test) and the
// views of the properties, the cheats, the AI doodad, the soldier's weapons, the draw context,
// the static objects, the particle system's slots, the contacts and the collision. The vector
// and matrix are file-private views (below): a vector of a double-pair/float union with
// implicit copies and no constructors, a matrix of four float rows (GetRow by reference,
// GetPos by value), declared before private copies of ISceneNode and IMovingSceneNode (the
// shared headers' text with the two bodies that name a vector's members or construct one
// written for this view; all four include guards predefined). The unit is compiled
// -inline auto,deferred, which emits the functions in reverse source order, so they are
// written last to first.
#define GAME_CVECTOR3_H
#define GAME_CMATRIX_H
#define GAME_ISCENENODE_H
#define GAME_IMOVINGSCENENODE_H
#include "game/msl_sqrtf.h"

/* inferred: file-private vector and matrix views */
class CVector3 {
public:
    void Set(float x, float y, float z) {
        d.v[0] = x;
        d.v[1] = y;
        d.v[2] = z;
    }
    float Dot(const CVector3& o) const { return d.v[0] * o.d.v[0] + d.v[1] * o.d.v[1] + d.v[2] * o.d.v[2]; }
    float Length() const { return sqrtf(d.v[0] * d.v[0] + d.v[1] * d.v[1] + d.v[2] * d.v[2]); }
    float LengthSquared() const { return d.v[0] * d.v[0] + d.v[1] * d.v[1] + d.v[2] * d.v[2]; }
    float LengthSq() const { return d.v[0] * d.v[0] + d.v[1] * d.v[1] + d.v[2] * d.v[2]; }
    void Normalize() {
        float length = Length();
        if (length != 0.0f) {
            d.v[0] *= 1.0f / length;
            d.v[1] *= 1.0f / length;
            d.v[2] *= 1.0f / length;
        }
    }
    void operator*=(float s) {
        d.v[0] *= s;
        d.v[1] *= s;
        d.v[2] *= s;
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
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    static bool s_ClassInit;
    void Ident();
    void BuildTrans(CVector3);
    void BuildRotX(float);
    void RotateY(float);
    void RotateZ(float);
    void Multiply(const CMatrix&, const CMatrix&);
    const CVector3& GetRow(int i) const { return *(const CVector3*)m[i]; }
    CVector3 GetPos() const { return *(const CVector3*)m[3]; }
    void SetPos(CVector3);
    void SetFront(CVector3);
    void SetRight(CVector3);
    void SetUp(CVector3);

    float m[4][4];
} __attribute__((aligned(16)));

class CCollision;
class CDrawContext;
class CBullet;
class CLight;
class CStaticObject;
class CHierObject;
class CWorldObject;
class CAnimObject;
class CSoldierObject;
class CPlayerObject;
class CPlayerWeaponObject;
class CAnimatedPlayerObject;
class CCollisionVolume;
class CAIDoodad;
class IMovingSceneNode;
class IVolume;
struct BSObject;

enum EClsnId {};

class IDestructible {
public:
    IDestructible() {}
    virtual void MarkForDestruction(int);
    virtual ~IDestructible() {}
    virtual void Destroy() {}
};

class ISubject : public IDestructible {
public:
    ISubject() {}
    virtual void MarkForDestruction(int);
    virtual ~ISubject();
};

class IObserver : public ISubject {
public:
    IObserver() {}
    virtual ~IObserver();
};

class ISceneNode : public IObserver {
public:
    enum EVolumeType {};
    struct SExtent {
        SExtent() {}

        ISceneNode* node;
        bool start;
        unsigned char unknown5[11];
    };

    ISceneNode() : data04(0), data08(0), data0c(0), data10(0), data14(0), data18(0), data1c(0), data20(0) {}
    virtual ~ISceneNode() {}
    virtual void BeginUpdate(float) {}
    virtual void UpdateAI(float) {}
    virtual void CommitAI() {}
    virtual void ConstrainVelocity() {}
    virtual void AttemptUpdate(float) {}
    virtual void OnCollision(const CCollision&) {}
    virtual void CommitUpdate() {}
    virtual void Draw(CDrawContext&) {}
    virtual IVolume* GetLocalBoundingVolume(EVolumeType) const { return 0; }
    virtual IVolume* GetWorldBoundingVolume(EVolumeType) const { return 0; }
    virtual void GetTMLocalToWorld(CMatrix& matrix) const { matrix.Ident(); }
    virtual void GetPosition(CVector3& position) const { position.d.v[0] = position.d.v[1] = position.d.v[2] = 0.0f; }
    virtual void GetRightward(CVector3& direction) const { direction.Set(1.0f, 0.0f, 0.0f); }
    virtual void GetForward(CVector3& direction) const { direction.Set(0.0f, 1.0f, 0.0f); }
    virtual void GetUpward(CVector3& direction) const { direction.Set(0.0f, 0.0f, 1.0f); }
    virtual bool IsVisible(CDrawContext&) const { return false; }
    virtual bool IsDrawEnabled() const { return false; }
    virtual EClsnId GetCollisionId() const { return (EClsnId)-1; }
    virtual void SetCollisionId(EClsnId) {}
    virtual BSObject* GetScriptObject() const { return 0; }
    virtual void TriggerScriptEvent(int, void*, bool) {}
    virtual void HandleBulletCollision(CBullet*, const CCollision&) {}
    virtual IMovingSceneNode* AsMovingNode() { return 0; }
    virtual const IMovingSceneNode* AsMovingNode() const { return 0; }
    virtual CStaticObject* AsStaticObject() { return 0; }
    virtual const CStaticObject* AsStaticObject() const { return 0; }
    virtual CHierObject* AsHierObject() { return 0; }
    virtual const CHierObject* AsHierObject() const { return 0; }
    virtual CWorldObject* AsWorldObject() { return 0; }
    virtual const CWorldObject* AsWorldObject() const { return 0; }
    virtual CAnimObject* AsAnimObject() { return 0; }
    virtual const CAnimObject* AsAnimObject() const { return 0; }
    virtual CSoldierObject* AsSoldierObject() { return 0; }
    virtual const CSoldierObject* AsSoldierObject() const { return 0; }
    virtual CPlayerObject* AsPlayerObject() { return 0; }
    virtual const CPlayerObject* AsPlayerObject() const { return 0; }
    virtual CPlayerWeaponObject* AsPlayerWeaponObject() { return 0; }
    virtual const CPlayerWeaponObject* AsPlayerWeaponObject() const { return 0; }
    virtual CAnimatedPlayerObject* AsAnimatedPlayerObject() { return 0; }
    virtual const CAnimatedPlayerObject* AsAnimatedPlayerObject() const { return 0; }
    virtual CLight* AsLight() { return 0; }
    virtual const CLight* AsLight() const { return 0; }
    virtual CBullet* AsBullet() { return 0; }
    virtual const CBullet* AsBullet() const { return 0; }
    virtual CCollisionVolume* AsCollisionVolume() { return 0; }
    virtual const CCollisionVolume* AsCollisionVolume() const { return 0; }
    virtual CAIDoodad* GetAIDoodad() { return 0; }
    virtual const CAIDoodad* GetAIDoodad() const { return 0; }
    virtual void SetAttachedLight(CLight*, CVector3) {}
    virtual CLight* GetAttachedLight() const { return 0; }

    int data04;
    int data08;
    int data0c;
    int data10;
    int data14;
    int data18;
    int data1c;
    int data20;
};

struct BPDLightVolume;

class IMovingSceneNode : public ISceneNode {
public:
    IMovingSceneNode() {}
    virtual ~IMovingSceneNode() {}
    virtual void Reset() {}
    virtual void Halt() {}
    virtual void ApplyForceTo(CVector3, float) {}
    virtual void SetTMLocalToWorld(const CMatrix&) {}
    virtual void PreTransform(const CMatrix&) {}
    virtual void Transform(const CMatrix&) {}
    virtual void Move(CVector3) {}
    virtual void Rotate(CVector3, float) {}
    virtual void Pitch(float) {}
    virtual void Roll(float) {}
    virtual void Yaw(float) {}
    virtual void SetPosition(CVector3) {}
    virtual void SetBasis(CVector3, CVector3, CVector3) {}
    virtual void Orthonormalize() {}
    virtual CVector3 GetWorldLinearVelocity() const {
        CVector3 velocity;
        velocity.Set(0.0f, 0.0f, 0.0f);
        return velocity;
    }
    virtual void EnterLightVolume(BPDLightVolume*) {}
    virtual void ExitLightVolume(BPDLightVolume*) {}
    virtual BPDLightVolume* GetLightVolume() { return 0; }
    virtual IMovingSceneNode* AsMovingNode() { return this; }
    virtual const IMovingSceneNode* AsMovingNode() const { return this; }
};

#include "game/CScene.h"

class CThrownBullet;
class CProjectileBullet;
class CAIDoodad;
enum EScriptBulletType {};
enum EThrownBulletTypes {};
enum ESubjectEvent {};
struct TriggerObject_struct;

extern CScene g_scene;
extern bool g_bInMultiplayerMode;
extern bool g_bIsRubberGrenadeCheatEnabled;
extern const float CONTACT_EPSILON;
extern "C" int rand();
void DebugMsg(const char*, ...);
bool BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);
void BSSendMessage(unsigned short, BSObject*, TriggerObject_struct*, BSObject*, void*);
unsigned short GetAzimuth(CVector3, const CPlayerObject*);
long long MathFunRandomI64(long long, long long);
void AEMS_SendEvent(void*);
void* DWI_alloc(const char*, int, int);

/* IVolume's virtuals in table order (__vt__9CVolFiber); its destructor and constructor are
   inline (weak in the original) */
class CVolBox;
class CVolSphere;
class CVolCapsule;
class CCDBObject;
class CVolFiber;
class CAnimatedVolume;
class CWorldVolume;
class CLine3;
class CPlane;
class CTriangle;
class CCSGVolume;

class IVolume {
public:
    IVolume() {}
    virtual ~IVolume() {}
    virtual IVolume* Create() const;
    virtual void TransformedCopy(const IVolume&, const CMatrix&);
    virtual int TestCollision(const IVolume&, CCollision&, bool) const;
    virtual int TestCollision(const CVolBox&, CCollision&, bool) const;
    virtual int TestCollision(const CVolSphere&, CCollision&, bool) const;
    virtual int TestCollision(const CVolCapsule&, CCollision&, bool) const;
    virtual int TestCollision(const CCDBObject&, CCollision&, bool) const;
    virtual int TestCollision(const CVolFiber&, CCollision&, bool) const;
    virtual int TestCollision(const CAnimatedVolume&, CCollision&, bool) const;
    virtual int TestCollision(const CWorldVolume&, CCollision&, bool) const;
    virtual int TestCollision(CVector3, CCollision&, bool) const;
    virtual int TestCollision(const CLine3&, CCollision&, bool) const;
    virtual int TestCollision(const CPlane&, CCollision&, bool) const;
    virtual int TestCollision(const CTriangle&, CCollision&, bool) const;
    virtual bool TestVisibility(CDrawContext&) const;
    virtual void GetExtents(CVector3&, CVector3&) const;
};

class CVolFiber : public IVolume {
public:
    CVolFiber() {}
    virtual IVolume* Create() const;
    virtual ~CVolFiber();
    virtual bool TestVisibility(CDrawContext&) const;
    virtual void GetExtents(CVector3&, CVector3&) const;
    void Set(CVector3, CVector3);

    unsigned char unknown04[76];
} __attribute__((aligned(8)));

class CLightVolumeManager {
public:
    CLightVolumeManager();
    ~CLightVolumeManager();
    void Update(float);
    void Reset();
    BPDLightVolume* GetVolume();
    void RemoveVolume(BPDLightVolume*);
    void AddVolume(BPDLightVolume*);

    unsigned char unknown00[28];
};

class CLight : public IMovingSceneNode {
public:
    static CLight* Create(unsigned long);
};

/* the particle system: 28 bytes of members before its table pointer; slots +0x1c (the
   release used here), +0x8c and +0xa0 */
class CParticleSystem {
    unsigned char m_unknown00[28];

public:
    static CParticleSystem* Create(unsigned long);
    virtual void unknown008();
    virtual void unknown00c();
    virtual void unknown010();
    virtual void unknown014();
    virtual void unknown018();
    virtual void Release();
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
    virtual void SetLocalToWorld(const CMatrix&);
    virtual void unknown090();
    virtual void unknown094();
    virtual void unknown098();
    virtual void unknown09c();
    virtual void SetVelocity(CVector3);
};

/* the draw context's matrix pointer at +192 */
class CDrawContext {
public:
    unsigned char unknown000[192];
    const CMatrix* m_matrix;
};

class CStaticMesh {
public:
    virtual void unknown008();
    virtual void Draw(CDrawContext&, int);
    virtual void unknown010();
    virtual bool UsesLightVolume();
    static void SetLightVolume(BPDLightVolume*);
};

/* a static object's mesh at +48 */
struct StaticObjectView {
    unsigned char unknown00[48];
    CStaticMesh* m_mesh;
};

extern StaticObjectView* g_pStaticObjectGrenade;
extern StaticObjectView* g_pStaticObjectStickGrenade;
extern StaticObjectView* g_pStaticObjectKnife;
extern StaticObjectView* g_pStaticObjectWrench;
extern StaticObjectView* g_pStaticObjectBazooka;

enum aistatus_var {};

class CAIStatus {
public:
    int GetVarValue(aistatus_var) const;
};

/* the AI object behind a doodad (team at +20, status at +436) */
struct AIObjectView {
    unsigned char unknown000[20];
    unsigned char m_team;
    unsigned char unknown015[415];
    CAIStatus m_status;
};

struct AIDoodadInfoView {
    unsigned char unknown00[8];
    AIObjectView* m_object;
};

class CAIDoodad {
public:
    unsigned char unknown00[4];
    AIDoodadInfoView* m_info;
};

/* the weapon's properties pointer (+640) and the flag at +74 */
struct WeaponPropertiesView {
    unsigned char unknown00[74];
    short m_flag4a;
};

struct WeaponView {
    unsigned char unknown000[640];
    WeaponPropertiesView* m_properties;
};

/* the soldier's weapons */
class CSoldierObject : public ISceneNode {
public:
    unsigned char unknown0024[16364];
    WeaponView* m_weapons[25];
    int m_currentWeapon;
};

/* a script game object: three words before its table pointer */
class BSGO_Basic {
public:
    BSObject* m_object;
    BSGO_Basic* m_next;
    int m_field8;

    BSGO_Basic() : m_object(0), m_next(0), m_field8(0) {}
    virtual void Destroy();
    virtual void* GetScriptData();
    virtual ISceneNode* GetSceneNode();
};

class BSGO_Dummy : public BSGO_Basic {
public:
    virtual void Destroy();
};

extern BSGO_Basic* g_pGermanList;
void ForceUpdateProximityTriggerStatus(BSGO_Basic*, bool);

/* inferred: a vector built from its components, for the one place that assigns a constructed
   temporary (the vector view itself declares no constructors) */
struct CVector3Value : public CVector3 {
    CVector3Value(float x, float y, float z) { Set(x, y, z); }
};

inline float DotXYZ(float x, float y, float z, const CVector3& v) { return x * v.d.v[0] + y * v.d.v[1] + z * v.d.v[2]; }

/* a scene sweep extent (its coordinate at +8) */
struct SExtentView {
    unsigned char unknown00[8];
    float m_value;
};

/* the grenade warning's event block */
struct GrenadeWarningInfo {
    BSObject* m_firedBy;
    CThrownBullet* m_bullet;
    int m_count;
};

/* whether two AI teams are enemies */
inline bool AreEnemies(unsigned char team, unsigned char other) {
    switch (team) {
    case 1:
        switch (other) {
        case 0:
        case 1:
            return false;
        case 2:
            return true;
        }
        break;
    case 2:
        switch (other) {
        case 0:
        case 2:
            return false;
        case 1:
            return true;
        }
        break;
    case 0:
        switch (other) {
        case 0:
            return false;
        case 1:
        case 2:
            return true;
        }
        break;
    }
    return false;
}

/* the cheat flags read here */
struct CheatsView {
    unsigned char unknown00[10];
    unsigned char m_keepGrenadeSpeed;
    bool m_bigExplosions;
    unsigned char unknown0c[8];
};

extern CheatsView g_cheats;

/* the thrown bullet's properties */
struct ThrownBulletProperties_struct {
    float m_speed;
    unsigned char unknown04[52];
    float m_friction;
    float m_bounce;
    float m_damage;
    float m_explosionRadius;
    short m_fuse;
    signed char m_bounces;
};

/* a contact of the collision: normal first, chained at +44 */
struct ContactView {
    float m_normalX;
    float m_normalY;
    float m_normalZ;
    unsigned char unknown0c[32];
    ContactView* m_next;
};

class CCollision {
public:
    ISceneNode* m_nodeA;
    ISceneNode* m_nodeB;
    unsigned char unknown08[16];
    ContactView* m_contacts;
};

/* the explosion's storage (its constructor and destructor are explosion.cpp's) */
class CExplosion {
public:
    CExplosion(CScene&, CVector3, float, float, float, EScriptBulletType, ISceneNode*, bool);
    ~CExplosion();

    unsigned char unknown00[68];
} __attribute__((aligned(16)));

class IObserver;
typedef void (*SubjectCallback)(IObserver&, ISubject**, ESubjectEvent);

class CPublisher {
public:
    static void CancelSubscription(IObserver&, ISubject**);
    static void AddSubscription(IObserver&, ISubject**, SubjectCallback);
};

static void SECB_NullifyPointer(IObserver&, ISubject**, ESubjectEvent);
void SECB_NullifyPointerOnMarked(IObserver&, ISubject**, ESubjectEvent);

class CBulletFactory {
public:
    void DestroyBullet(CBullet*);
};

extern CBulletFactory* g_pBulletFactory;

class CBullet : public IMovingSceneNode {
public:
    CBullet();
    virtual ~CBullet();
    virtual void Draw(CDrawContext&);
    virtual void GetTMLocalToWorld(CMatrix&) const;
    virtual void GetPosition(CVector3&) const;
    virtual void GetRightward(CVector3&) const;
    virtual void GetForward(CVector3&) const;
    virtual bool IsDrawEnabled() const;
    virtual CBullet* AsBullet();
    virtual const CBullet* AsBullet() const;
    virtual void Init();
    virtual void Shutdown();
    virtual void GetUp(CVector3&) const;
    virtual float GetDamage() const;
    virtual EScriptBulletType GetScriptBulletType() const;
    virtual ISceneNode* GetFiredBy() const;
    virtual CThrownBullet* AsThrown();
    virtual CProjectileBullet* AsProjectile();
    virtual void GetVelocity(CVector3&);
    virtual void SetExclusionPair(ISceneNode*);
    virtual void Penetrate(const CCollision&);

    int m_collisionId;
    int data28;
    unsigned char unknown2c[4];
    CVector3 m_position;
    unsigned char unknown040[16];
    CMatrix m_tm;
};

class CThrownBullet : public CBullet {
public:
    CThrownBullet();
    virtual void MarkForDestruction(int);
    virtual ~CThrownBullet();
    virtual void Destroy();
    virtual void BeginUpdate(float);
    virtual void ConstrainVelocity();
    virtual void AttemptUpdate(float);
    virtual void OnCollision(const CCollision&);
    virtual void CommitUpdate();
    virtual void Draw(CDrawContext&);
    virtual bool IsVisible(CDrawContext&) const;
    virtual EClsnId GetCollisionId() const;
    virtual void HandleBulletCollision(CBullet*, const CCollision&);
    virtual void Halt();
    virtual void ApplyForceTo(CVector3, float);
    virtual void EnterLightVolume(BPDLightVolume*);
    virtual void ExitLightVolume(BPDLightVolume*);
    virtual BPDLightVolume* GetLightVolume();
    virtual void Shutdown();
    virtual EScriptBulletType GetScriptBulletType() const;
    virtual void Penetrate(const CCollision&);
    virtual void Init() { CBullet::Init(); }
    virtual IVolume* GetLocalBoundingVolume(EVolumeType) const { return (IVolume*)&m_localVolume; }
    virtual IVolume* GetWorldBoundingVolume(EVolumeType) const { return (IVolume*)&m_worldVolume; }
    virtual bool IsCollisionEnabled() const { return true; }
    virtual CLight* GetAttachedLight() const { return (CLight*)m_light; }
    virtual float GetDamage() const { return m_properties->m_damage; }
    virtual CThrownBullet* AsThrown() { return this; }
    virtual void GetVelocity(CVector3& v) { v = m_velocity; }
    virtual ISceneNode* GetFiredBy() const { return m_firedBy; }

    static void* operator new[](unsigned long);
    void SomebodyCaughtYou();
    void CheckEnemies();
    void SetDetonationTime(float);
    void Explode();
    void Init(EThrownBulletTypes, ThrownBulletProperties_struct*, const CVector3&, const CVector3&, float, EClsnId, float, ISceneNode*);
    void SetSubject(ISubject** slot, ISubject* subject, SubjectCallback callback) {
        if (subject != *slot) {
            if (slot)
                CPublisher::CancelSubscription(*(IObserver*)this, slot);
            *slot = subject;
            if (subject)
                CPublisher::AddSubscription(*(IObserver*)this, slot, callback);
        }
    }

    float m_detonationTime;
    float m_closest;
    unsigned char unknown098[12];
    unsigned int m_type;
    ThrownBulletProperties_struct* m_properties;
    ISceneNode* m_firedBy;
    BSGO_Dummy m_scriptObject;
    CThrownBullet* m_self;
    int m_value0c4;
    int m_value0c8;
    int m_value0cc;
    int m_value0d0;
    CLightVolumeManager m_lightVolumes;
    CVector3 m_velocity;
    CVector3 m_launchVelocity;
    CVector3 m_normal;
    CVector3 m_rotation;
    CVector3 m_rotationBase;
    CVector3 m_rotationRate;
    CVolFiber m_localVolume;
    CVolFiber m_worldVolume;
    ISubject* m_light;
    CParticleSystem* m_effect;
    bool m_resting : 1;
    bool m_touched : 1;
    bool m_gravity : 1;
    bool m_flag10 : 1;
    bool m_flag08 : 1;
    bool m_flag04 : 1;
    bool m_warned : 1;
    bool m_halted : 1;
    int m_value1fc;
};

float MP_THROWNBULLET_DRAW_DELAY = 2.0f;
void* CThrownBullet::operator new[](unsigned long size) {
    return DWI_alloc(0, size, 1024);
}

CThrownBullet::CThrownBullet() {
    data28 = 2;
}

CThrownBullet::~CThrownBullet() {
}

static void SECB_NullifyPointer(IObserver& observer, ISubject** subject, ESubjectEvent) {
    if (*subject) {
        if (subject)
            CPublisher::CancelSubscription(observer, subject);
        *subject = 0;
    }
}

inline double fabs(double x) { return __fabs(x); }
#define RANDF ((float)rand() / 32767.0f)

void CThrownBullet::Init(EThrownBulletTypes type, ThrownBulletProperties_struct* properties, const CVector3& position,
                         const CVector3& direction, float speed, EClsnId clsnId, float power, ISceneNode* firedBy) {
    CBullet::Init();
    m_type = type;
    m_properties = properties;
    m_detonationTime = properties->m_fuse;
    m_position = position;
    m_collisionId = clsnId;
    m_resting = false;
    m_touched = false;
    m_flag10 = false;
    m_flag04 = false;
    m_warned = false;
    m_halted = false;
    m_flag08 = !properties->m_bounces;
    SetSubject(&m_light, 0, SECB_NullifyPointerOnMarked);
    m_effect = 0;
    m_value1fc = 0;
    SetSubject((ISubject**)&m_firedBy, (ISubject*)firedBy, SECB_NullifyPointer);
    power *= 0.001953125f;
    static float maxPower = 1.0f;
    static float minPower = 0.1f;
    if (m_firedBy && m_firedBy->AsSoldierObject()) {
        switch (type) {
        case 0:
        case 1:
        case 2:
            power = 0.25f * maxPower;
            break;
        case 10:
            power = 0.5f * maxPower;
            break;
        default:
            power = maxPower * power;
            break;
        }
    } else {
        power = minPower + power * (maxPower - minPower);
    }
    m_velocity = direction;
    m_velocity.Normalize();
    m_velocity *= power * (speed * m_properties->m_speed);
    m_launchVelocity = m_velocity;
    m_tm.BuildTrans(position);
    CVector3 front = direction;
    CVector3 right;
    CVector3 up;
    front.Normalize();
    if (fabs(front.d.v[2]) < fabs(front.d.v[0])) {
        up.Set(0.0f, 0.0f, 1.0f);
        right.Cross(front, up);
        up.Cross(right, front);
    } else {
        if (front.d.v[1] < 0.0f)
            right.Set(-1.0f, 0.0f, 0.0f);
        else
            right.Set(1.0f, 0.0f, 0.0f);
        up.Cross(right, front);
        right.Cross(front, up);
    }
    m_tm.SetFront(front);
    m_tm.SetRight(right);
    m_tm.SetUp(up);
    g_scene.Add(*(ISceneNode*)this);
    CVector3 top;
    top.Set(0.0f, -m_velocity.Length(), 0.0f);
    CVector3 bottom;
    bottom.Set(0.0f, 0.0f, 0.0f);
    m_localVolume.Set(top, bottom);
    switch (m_type) {
    case 5:
        m_gravity = false;
        m_rotation.Set(0.0f, 0.0f, 0.0f);
        {
            float x, y, z;
            z = RANDF;
            y = RANDF;
            x = RANDF;
            m_rotationRate.Set(-1.3962634f * x, -1.3962634f * y, -1.3962634f * z);
        }
        break;
    case 4:
        m_rotation.Set(0.0f, 0.0f, 0.0f);
        m_rotationRate = CVector3Value(0.0f, 1.3962634f, 1.3962634f);
        break;
    case 3:
    case 6:
    case 9:
        m_rotation.Set(0.0f, 0.0f, 0.0f);
        {
            float x, y, z;
            z = RANDF;
            y = RANDF;
            x = RANDF;
            m_rotationRate.Set(-0.41887903f * x, -0.41887903f * y, -0.41887903f * z);
        }
        break;
    case 0:
    case 1:
    case 2:
    case 10: {
        m_gravity = false;
        m_rotation.Set(0.0f, 0.0f, 0.0f);
        m_rotationRate.Set(0.0f, 0.0f, 0.0f);
        CLight* light = CLight::Create(0xA2401755);
        if (light) {
            light->SetTMLocalToWorld(m_tm);
            SetSubject(&m_light, light, SECB_NullifyPointerOnMarked);
        }
        m_effect = CParticleSystem::Create(0xA23FD6BD);
        if (m_effect) {
            CVector3 velocity;
            velocity.Set(60.0f * m_velocity.d.v[0], 60.0f * m_velocity.d.v[1], 60.0f * m_velocity.d.v[2]);
            m_effect->SetVelocity(velocity);
            m_effect->SetLocalToWorld(m_tm);
        }
        break;
    }
    case 8: {
        m_gravity = false;
        m_rotation.Set(0.0f, 0.0f, 0.0f);
        m_rotationRate.Set(0.0f, 0.0f, 0.0f);
        CLight* light = CLight::Create(0xA2401755);
        if (light) {
            light->SetTMLocalToWorld(m_tm);
            SetSubject(&m_light, light, SECB_NullifyPointerOnMarked);
        }
        m_effect = CParticleSystem::Create(0xA23FD6BD);
        if (m_effect) {
            CVector3 velocity;
            velocity.Set(60.0f * m_velocity.d.v[0], 60.0f * m_velocity.d.v[1], 60.0f * m_velocity.d.v[2]);
            m_effect->SetVelocity(velocity);
            m_effect->SetLocalToWorld(m_tm);
        }
        break;
    }
    default:
        DebugMsg("WARNING: Unknown thrown bullet type in Init.  Type %d\n", m_type);
        break;
    }
    m_rotationBase = m_rotation;
    m_value0c8 = 0;
    m_value0c4 = 0;
    m_value0d0 = 0;
    m_value0cc = 0;
    m_scriptObject.m_object = 0;
    m_self = this;
    CSoldierObject* owner = m_firedBy ? m_firedBy->AsSoldierObject() : 0;
    if (owner) {
        for (BSGO_Basic* german = g_pGermanList; german; german = german->m_next) {
            if (german->GetSceneNode()->AsSoldierObject() == owner) {
                BSObjectTriggerEvent(german->m_object, 264, this, 0, true);
                break;
            }
        }
    }
    if (g_bIsRubberGrenadeCheatEnabled || g_cheats.m_keepGrenadeSpeed) {
        m_properties->m_friction = 0.0f;
        m_properties->m_bounce = 1.0f;
    }
}

void CThrownBullet::Shutdown() {
    CBullet::Shutdown();
    m_lightVolumes.Reset();
}

void CThrownBullet::MarkForDestruction(int flag) {
    ISubject::MarkForDestruction(flag);
    if (m_light) {
        m_light->MarkForDestruction(flag);
        if (m_light) {
            if (&m_light)
                CPublisher::CancelSubscription(*(IObserver*)this, &m_light);
            m_light = 0;
        }
    }
    if (m_effect) {
        m_effect->Release();
        m_effect = 0;
    }
    m_halted = true;
    g_scene.Remove(*(ISceneNode*)this);
    m_collisionId = -1;
}

void CThrownBullet::Destroy() {
    g_pBulletFactory->DestroyBullet(this);
}

void CThrownBullet::OnCollision(const CCollision& collision) {
    if (m_halted || m_flag10)
        return;
    switch (m_type) {
    case 5:
        m_gravity = true;
    case 3:
    case 4:
    case 9:
        m_normal.d.v[0] = m_normal.d.v[1] = m_normal.d.v[2] = 0.0f;
        for (ContactView* contact = collision.m_contacts; contact; contact = contact->m_next) {
            if (contact->m_normalZ > 0.7f && contact->m_normalX * m_velocity.d.v[0] + contact->m_normalY * m_velocity.d.v[1] + contact->m_normalZ * m_velocity.d.v[2] <= 0.0f)
                m_resting = true;
            m_normal.d.v[0] += contact->m_normalX;
            m_normal.d.v[1] += contact->m_normalY;
            m_normal.d.v[2] += contact->m_normalZ;
            m_touched = true;
        }
        break;
    case 0:
    case 1:
    case 2:
    case 8:
    case 10:
        for (ContactView* contact = collision.m_contacts; contact; contact = contact->m_next) {
            if (contact->m_normalZ > 0.7f)
                m_resting = true;
            m_touched = true;
        }
        if (m_touched)
            m_flag10 = !m_flag08;
        break;
    default:
        DebugMsg("WARNING: Unknown thrown bullet type in OnCollision.  Type %d\n", m_type);
        break;
    }
    if (m_touched) {
        ISceneNode* other = collision.m_nodeA;
        if (other == (ISceneNode*)this)
            other = collision.m_nodeB;
        other->HandleBulletCollision(this, collision);
    }
}

void CThrownBullet::Penetrate(const CCollision& collision) {
    const CVector3* normal = (const CVector3*)collision.m_contacts;
    CVector3 offset;
    GetForward(offset);
    float along = offset.Dot(*normal);
    if ((float)__fabs(along) > 1e-05f) {
        offset *= CONTACT_EPSILON / (float)__fabs(along);
    } else if (along > 0.0f) {
        offset.d.v[0] = CONTACT_EPSILON * normal->d.v[0];
        offset.d.v[1] = CONTACT_EPSILON * normal->d.v[1];
        offset.d.v[2] = CONTACT_EPSILON * normal->d.v[2];
    } else {
        offset.d.v[0] = -CONTACT_EPSILON * normal->d.v[0];
        offset.d.v[1] = -CONTACT_EPSILON * normal->d.v[1];
        offset.d.v[2] = -CONTACT_EPSILON * normal->d.v[2];
    }
    const CVector3& current = m_tm.GetPos();
    CVector3 position;
    position.d.v[0] = current.d.v[0] + offset.d.v[0];
    position.d.v[1] = current.d.v[1] + offset.d.v[1];
    position.d.v[2] = current.d.v[2] + offset.d.v[2];
    m_tm.SetPos(position);
    m_touched = 0;
    m_flag10 = 0;
}

void CThrownBullet::ConstrainVelocity() {
    if (m_touched) {
        float speed;
        float along;
        float into;
        CVector3 normal;
        CVector3 normalPart;
        float tangentX;
        float tangentY;
        float tangentZ;
        float up;
        float steep;
        float keep;
        float newSpeed;
        float ratio;
        float push;
        CVector3 position;
        speed = 1.0f;
        if (g_cheats.m_keepGrenadeSpeed)
            speed = m_velocity.Length();
        m_normal.Normalize();
        normal = m_normal;
        along = m_velocity.Dot(normal);
        if (along > 0.0f) {
            m_normal.d.v[0] = -m_normal.d.v[0];
            m_normal.d.v[1] = -m_normal.d.v[1];
            m_normal.d.v[2] = -m_normal.d.v[2];
            normal = m_normal;
            along = m_velocity.Dot(normal);
        }
        normal *= 2.0f * along;
        m_velocity.d.v[0] -= normal.d.v[0];
        m_velocity.d.v[1] -= normal.d.v[1];
        m_velocity.d.v[2] -= normal.d.v[2];
        into = m_normal.d.v[0] * m_velocity.d.v[0] + m_normal.d.v[1] * m_velocity.d.v[1] + m_normal.d.v[2] * m_velocity.d.v[2];
        normalPart.d.v[0] = into * m_normal.d.v[0];
        normalPart.d.v[1] = into * m_normal.d.v[1];
        normalPart.d.v[2] = into * m_normal.d.v[2];
        tangentX = m_velocity.d.v[0] - normalPart.d.v[0];
        tangentY = m_velocity.d.v[1] - normalPart.d.v[1];
        tangentZ = m_velocity.d.v[2] - normalPart.d.v[2];
        normalPart.d.v[0] *= m_properties->m_bounce;
        normalPart.d.v[1] *= m_properties->m_bounce;
        normalPart.d.v[2] *= m_properties->m_bounce;
        if (normalPart.LengthSquared() < 0.0001f && m_resting) {
            normalPart.d.v[2] = 0.0f;
            normalPart.d.v[1] = 0.0f;
            normalPart.d.v[0] = 0.0f;
        }
        up = DotXYZ(0.0f, 0.0f, 1.0f, m_normal);
        if (up <= 0.0f)
            steep = 0.0f;
        else
            steep = up * (up * up);
        keep = 1.0f - steep * m_properties->m_friction;
        tangentX *= keep;
        tangentY *= keep;
        tangentZ *= keep;
        normalPart.d.v[0] += tangentX;
        normalPart.d.v[1] += tangentY;
        normalPart.d.v[2] += tangentZ;
        m_velocity = normalPart;
        if (g_cheats.m_keepGrenadeSpeed) {
            newSpeed = m_velocity.Length();
            if (newSpeed > 0.01 && speed > 0.01) {
                ratio = speed / newSpeed;
                m_velocity.d.v[0] *= ratio;
                m_velocity.d.v[1] *= ratio;
                m_velocity.d.v[2] *= ratio;
            }
        }
        CVector3 offset;
        if (m_resting) {
            offset.d.v[0] = CONTACT_EPSILON * m_normal.d.v[0];
            offset.d.v[1] = CONTACT_EPSILON * m_normal.d.v[1];
            offset.d.v[2] = CONTACT_EPSILON * m_normal.d.v[2];
        } else {
            push = 2.0f * CONTACT_EPSILON;
            offset.d.v[0] = push * m_normal.d.v[0];
            offset.d.v[1] = push * m_normal.d.v[1];
            offset.d.v[2] = push * m_normal.d.v[2];
        }
        const CVector3& current = m_tm.GetPos();
        position.d.v[0] = current.d.v[0] + offset.d.v[0];
        position.d.v[1] = current.d.v[1] + offset.d.v[1];
        position.d.v[2] = current.d.v[2] + offset.d.v[2];
        m_tm.SetPos(position);
    }
}

void CThrownBullet::BeginUpdate(float elapsed) {
    m_lightVolumes.Update(elapsed);
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
    ForceUpdateProximityTriggerStatus(&m_scriptObject, true);
    if (m_detonationTime > 0.0f) {
        m_detonationTime -= elapsed;
        if (m_detonationTime < 0.0f)
            m_detonationTime = 0.0f;
    }
    m_closest = 3.4028235e+38f;
}

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
        m_worldVolume.Set(m_position, step);
        CVector3 min;
        CVector3 max;
        m_worldVolume.GetExtents(min, max);
        ((SExtentView*)data0c)->m_value = min.d.v[0] - CONTACT_EPSILON;
        ((SExtentView*)data14)->m_value = min.d.v[1] - CONTACT_EPSILON;
        ((SExtentView*)data10)->m_value = CONTACT_EPSILON + max.d.v[0];
        ((SExtentView*)data18)->m_value = CONTACT_EPSILON + max.d.v[1];
    }
}

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

void CThrownBullet::CommitUpdate() {
    if (m_halted)
        return;
    switch ((int)m_type) {
    case 3:
    case 4:
    case 5:
    case 9:
        if (m_detonationTime > 0.0f && (!m_flag10 || m_flag08)) {
            ConstrainVelocity();
            m_position = m_tm.GetPos();
            m_launchVelocity = m_velocity;
            if ((int)m_type != 5)
                m_gravity = !m_resting;
            m_resting = 0;
            m_touched = 0;
        } else {
            if (!m_flag08) {
                CVector3 position = m_tm.GetRow(3);
                CExplosion explosion(g_scene, position, m_properties->m_explosionRadius, 1.0f, GetDamage(),
                                     GetScriptBulletType(), GetFiredBy(), false);
                CMatrix tm;
                tm.BuildTrans(position);
                CreateExplosionEffect(tm, m_resting, g_cheats.m_bigExplosions);
                CPlayerObject* player;
                if (g_bInMultiplayerMode)
                    player = g_scene.FindClosestPlayer(position);
                else
                    player = g_scene.GetPlayer(0);
                CVector3 delta;
                ((ISceneNode*)player)->GetPosition(delta);
                delta.d.v[0] = delta.d.v[0] - position.d.v[0];
                delta.d.v[1] = delta.d.v[1] - position.d.v[1];
                delta.d.v[2] = delta.d.v[2] - position.d.v[2];
                int event[10];
                event[0] = 7;
                event[1] = 3;
                event[2] = MathFunRandomI64(16, 17);
                event[3] = MathFunRandomI64(3600, 4600);
                event[4] = GetAzimuth(position, player);
                event[5] = 256.0f * delta.Length();
                event[6] = 0;
                event[7] = 24999;
                event[8] = 0;
                event[9] = 32000;
                AEMS_SendEvent(event);
                BSSendMessage(180, GetScriptObject(), 0, 0, this);
            }
            MarkForDestruction(0);
        }
        break;
    case 0:
    case 1:
    case 2:
    case 8:
    case 10:
        if (m_detonationTime > 0.0f && (!m_flag10 || m_flag08)) {
            ConstrainVelocity();
            m_position = m_tm.GetPos();
            m_launchVelocity = m_velocity;
        } else {
            CVector3 position = m_tm.GetRow(3);
            float speed = m_velocity.Length();
            if (speed > CONTACT_EPSILON) {
                float back = -0.05f / speed;
                float backX = back * m_velocity.d.v[0];
                float backY = back * m_velocity.d.v[1];
                float backZ = back * m_velocity.d.v[2];
                position.d.v[0] = position.d.v[0] + backX;
                position.d.v[1] = position.d.v[1] + backY;
                position.d.v[2] = position.d.v[2] + backZ;
            }
            CExplosion explosion(g_scene, position, m_properties->m_explosionRadius, 1.0f, GetDamage(),
                                 GetScriptBulletType(), GetFiredBy(), false);
            CMatrix tm;
            tm.BuildTrans(position);
            CreateExplosionEffect(tm, m_detonationTime > 0.0f, g_cheats.m_bigExplosions);
            CPlayerObject* player = g_scene.GetPlayer(0);
            CVector3 delta;
            ((ISceneNode*)player)->GetPosition(delta);
            delta.d.v[0] = delta.d.v[0] - position.d.v[0];
            delta.d.v[1] = delta.d.v[1] - position.d.v[1];
            delta.d.v[2] = delta.d.v[2] - position.d.v[2];
            int event[10];
            event[0] = 7;
            event[1] = 3;
            event[2] = MathFunRandomI64(34, 35);
            event[3] = MathFunRandomI64(3600, 4600);
            event[4] = GetAzimuth(position, player);
            event[5] = 256.0f * delta.Length();
            event[6] = 0;
            event[7] = 24999;
            event[8] = 0;
            event[9] = 32000;
            AEMS_SendEvent(event);
            BSSendMessage(180, GetScriptObject(), 0, 0, this);
            MarkForDestruction(0);
        }
        break;
    default:
        DebugMsg("WARNING: Unknown thrown bullet type in CommitUpdate.  Type %d\n", m_type);
        break;
    }
    if (m_light) {
        CMatrix tm;
        GetTMLocalToWorld(tm);
        ((CLight*)m_light)->SetTMLocalToWorld(tm);
    }
}

EClsnId CThrownBullet::GetCollisionId() const {
    return (EClsnId)m_collisionId;
}

void CThrownBullet::Draw(CDrawContext& context) {
    if (g_bInMultiplayerMode && m_firedBy && m_firedBy->AsPlayerObject()) {
        if (m_detonationTime > (float)m_properties->m_fuse - MP_THROWNBULLET_DRAW_DELAY)
            return;
    }
    StaticObjectView* object = 0;
    switch (m_type) {
    case 3:
        object = g_pStaticObjectGrenade;
        break;
    case 4:
        object = g_pStaticObjectStickGrenade;
        break;
    case 5:
        object = g_pStaticObjectKnife;
        break;
    case 9:
        object = g_pStaticObjectWrench;
        break;
    case 0:
    case 1:
    case 2:
    case 8:
    case 10:
        object = g_pStaticObjectBazooka;
        break;
    default:
        DebugMsg("WARNING: Unknown thrown bullet type in Draw.  Type %d\n", m_type);
        break;
    }
    if (object) {
        CMatrix world;
        CMatrix rotation;
        rotation.BuildRotX(m_rotation.d.v[0]);
        rotation.RotateY(m_rotation.d.v[1]);
        rotation.RotateZ(m_rotation.d.v[2]);
        world.Multiply(rotation, m_tm);
        CStaticMesh* mesh = object->m_mesh;
        if (mesh) {
            context.m_matrix = &world;
            if (mesh->UsesLightVolume())
                CStaticMesh::SetLightVolume(GetLightVolume());
            mesh->Draw(context, 0);
        }
    }
}

bool CThrownBullet::IsVisible(CDrawContext& context) const {
    return m_worldVolume.TestVisibility(context);
}

EScriptBulletType CThrownBullet::GetScriptBulletType() const {
    switch (m_type) {
    case 3:
        return (EScriptBulletType)3;
    case 0:
    case 8:
    case 10:
        return (EScriptBulletType)21;
    case 1:
        return (EScriptBulletType)23;
    case 2:
        return (EScriptBulletType)22;
    case 4:
        return (EScriptBulletType)24;
    case 5:
        return (EScriptBulletType)24;
    case 9:
        return (EScriptBulletType)24;
    case 6:
        return (EScriptBulletType)26;
    default:
        DebugMsg("WARNING: Unknown thrown bullet type in GetScriptBulletType.  Type %d\n", m_type);
        return (EScriptBulletType)3;
    }
}

void CThrownBullet::Explode() {
    m_flag10 = !m_flag08;
}

void CThrownBullet::SetDetonationTime(float time) {
    if (m_properties->m_bounces)
        m_detonationTime = time;
}

void CThrownBullet::HandleBulletCollision(CBullet* bullet, const CCollision&) {
    if (bullet->AsProjectile())
        m_flag10 = !m_flag08;
}

void CThrownBullet::Halt() {
    m_velocity.d.v[0] = m_velocity.d.v[1] = m_velocity.d.v[2] = 0.0f;
    m_launchVelocity.d.v[0] = m_launchVelocity.d.v[1] = m_launchVelocity.d.v[2] = 0.0f;
}

void CThrownBullet::ApplyForceTo(CVector3 force, float scale) {
    float strength = scale / 512.0f;
    m_velocity = force;
    m_velocity.Normalize();
    m_velocity *= m_properties->m_speed * strength;
    m_launchVelocity = m_velocity;
}

inline void WarnOthers(CThrownBullet* self, BSGO_Basic* best, GrenadeWarningInfo& info, CVector3& delta, CSoldierObject*& soldier, AIObjectView*& target) {
    BSGO_Basic* other;
    AIDoodadInfoView* doodad;
    target = best->GetSceneNode()->GetAIDoodad()->m_info->m_object;
    for (other = g_pGermanList; other; other = other->m_next) {
        soldier = other->GetSceneNode()->AsSoldierObject();
        if (self->m_firedBy && soldier == self->m_firedBy->AsSoldierObject())
            continue;
        if (other == best)
            continue;
        doodad = soldier->GetAIDoodad()->m_info;
        if (doodad->m_object->m_status.GetVarValue((aistatus_var)0) <= 0)
            continue;
        soldier->GetPosition(delta);
        delta.d.v[0] = self->m_position.d.v[0] - delta.d.v[0];
        delta.d.v[1] = self->m_position.d.v[1] - delta.d.v[1];
        delta.d.v[2] = self->m_position.d.v[2] - delta.d.v[2];
        if (delta.d.v[0] * delta.d.v[0] + delta.d.v[1] * delta.d.v[1] < 36.0f) {
            BSObjectTriggerEvent(other->m_object, 174, &info, 0, true);
            if (!AreEnemies(target->m_team, doodad->m_object->m_team))
                info.m_count++;
        }
    }
}

void CThrownBullet::CheckEnemies() {
    BSGO_Basic* node;
    CSoldierObject* soldier;
    float bestDistance;
    BSGO_Basic* best;
    int event;
    bestDistance = 0.0f;
    best = 0;
    event = -1;
    if (m_warned || (int)m_type == 0 || m_type - 1 <= 1 || m_type - 5 <= 5 || (int)m_type == 8)
        return;
    CVector3 delta;
    CVector3 direction;
    for (node = g_pGermanList; node; node = node->m_next) {
        soldier = node->GetSceneNode()->AsSoldierObject();
        if (m_firedBy && soldier == m_firedBy->AsSoldierObject())
            continue;
        if (soldier->GetAIDoodad()->m_info->m_object->m_status.GetVarValue((aistatus_var)0) <= 0)
            continue;
        soldier->GetPosition(delta);
        delta.d.v[0] = m_position.d.v[0] - delta.d.v[0];
        delta.d.v[1] = m_position.d.v[1] - delta.d.v[1];
        delta.d.v[2] = m_position.d.v[2] - delta.d.v[2];
        float speed = sqrtf(m_velocity.LengthSq());
        if (delta.d.v[2] > 1.3f && delta.d.v[2] < 1.9f) {
            soldier->GetForward(direction);
            float facing = direction.d.v[0] * delta.d.v[0] + direction.d.v[1] * delta.d.v[1] + direction.d.v[2] * delta.d.v[2];
            if (facing < 1.0f && facing > 0.0f) {
                float low = -0.1f;
                float high = 0.5f;
                WeaponView* weapon = soldier->m_weapons[soldier->m_currentWeapon];
                if (weapon && weapon->m_properties->m_flag4a) {
                    low = -0.3f;
                    high = 0.2f;
                }
                soldier->GetRightward(direction);
                float side = direction.d.v[0] * delta.d.v[0] + direction.d.v[1] * delta.d.v[1] + direction.d.v[2] * delta.d.v[2];
                if (side < high && side > low) {
                    m_warned = 1;
                    best = node;
                    event = 163;
                    break;
                }
            }
        } else if (delta.d.v[2] < 0.05f && speed < 0.05f) {
            float distance = delta.d.v[0] * delta.d.v[0] + delta.d.v[1] * delta.d.v[1];
            if (distance < 9.0f) {
                if (!best || distance < bestDistance) {
                    m_warned = 1;
                    bestDistance = distance;
                    best = node;
                    event = 172;
                }
            } else if (distance < 36.0f) {
                if (!best || distance < bestDistance) {
                    m_warned = 1;
                    bestDistance = distance;
                    best = node;
                    event = 173;
                }
            }
        }
    }
    if (best) {
        GrenadeWarningInfo info;
        if (m_firedBy)
            info.m_firedBy = m_firedBy->GetScriptObject();
        else
            info.m_firedBy = 0;
        info.m_bullet = this;
        info.m_count = 0;
        AIObjectView* target;
        WarnOthers(this, best, info, delta, soldier, target);
        BSObjectTriggerEvent(best->m_object, (unsigned short)event, &info, 0, true);
    }
}

void CThrownBullet::SomebodyCaughtYou() {
    if (!m_flag10 && !m_halted) {
        m_flag10 = 0;
        m_flag08 = 1;
        MarkForDestruction(0);
    }
}

void CThrownBullet::EnterLightVolume(BPDLightVolume* volume) {
    m_lightVolumes.AddVolume(volume);
}

void CThrownBullet::ExitLightVolume(BPDLightVolume* volume) {
    m_lightVolumes.RemoveVolume(volume);
}

/* ---- definitions ---- */
BPDLightVolume* CThrownBullet::GetLightVolume() {
    return m_lightVolumes.GetVolume();
}
