// anim_obj.cpp: CAnimObject, the animated scene object, the whole file in one unit (it defines
// __vt__11CAnimObject and the RTTI records of CAnimObject and its bases). The functions are
// written last to first: the file is compiled with deferred inlining (FinishAnimationState
// inlines the later SetAnimationState; the original numbers the RTTI records before every
// function's exception records), which emits the definitions in reverse order.
//
// CAnimObject is IMovingSceneNode (game/IMovingSceneNode.h) with its virtual functions in the
// order of __vt__11CAnimObject (as animplayer.cpp declares them); the empty overrides are
// inline in the original (weak symbols, emitted for the table) and are defined
// __declspec(weak) here, in the original's order. The data members come from the code; their
// names, types and the space between them are inferred (several functions name the same word
// differently, so a few are anonymous unions). CClsnInfo is a contact record (its normal and
// the other object's moving node), copied member by member. The CBullet functions emitted
// here are its weak defaults.
//
// Inferred, file-private views: the animation, light-volume, volume, collision, contact,
// draw-context, camera, shell and configuration classes declare only what these functions
// use (placeholders named by offset for the virtual slots in between). CLine3 has an inline
// destructor whose weak copy the linker took from another file. Two vector forms are file
// helpers: the constructor's three zero vectors store z first, and the collision normals are
// normalised with one divide per component.
extern "C" {
#include <math.h>
}

#include "game/CVector3.h"
#include "game/CMatrix.h"
#include "game/IMovingSceneNode.h"
#include "game/DwiContainers.h"
#include "game/CScene.h"

struct BSObject;
class CStaticObject;
class CThrownBullet {
public:
    void Explode();
};
class CProjectileBullet;
struct CharFrame_t;
class CObjLocationCollisionData;
class CObjLocationCollisionMap;
class CRenderList;
class CScreen;
class CVolBox;
class CVolSphere;
class CVolCapsule;
class CCDBObject;
class CVolFiber;

extern CScene g_scene;
extern const float CONTACT_EPSILON;

void* BSEventGetEventMemoryBlock(unsigned int);
void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);
void PlayImpactSound(unsigned short, unsigned short, CVector3, bool);

enum EShotLocations {};

class CShellMenu {
public:
    void Set_hits(unsigned int, bool);
    void Set_shotLocation(unsigned int, EShotLocations, bool);

    unsigned char unknown0000[6620];
};

extern CShellMenu g_Shell;

/* inferred: the configuration flag at +3 that stops animated objects drawing */
struct ConfigView {
    unsigned char unknown00[3];
    bool noAnimObjects;
    unsigned char unknown04[16];
};

extern ConfigView g_config;

class CCamera {
public:
    bool IsPointVisible(CVector3, CDrawContext&) const;
};

/* inferred: the draw context's transform pointer at +192 and camera at +212 */
class CDrawContext {
public:
    unsigned char unknown000[192];
    CMatrix* m_localToWorld;
    unsigned char unknown0c4[16];
    CCamera* m_camera;
};

// Inferred: the object drawing the animated mesh, called through the virtuals at +16 and +68
// of its table (the other slots are placeholders named by offset).
class AnimDrawerView {
public:
    virtual void unknown008();
    virtual void unknown00c();
    virtual void unknown010(void*, const CMatrix&);
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
    virtual bool unknown044(CDrawContext&);
};

class CQuaternion {
public:
    void SetFromMatrix(const CMatrix&);
    void Slerp(CQuaternion, CQuaternion, float);
    void GetMatrix(CMatrix&) const;

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
    virtual void vf2c();
    virtual void vf30();
    virtual void vf34();
    virtual void vf38();
    virtual void vf3c();
    virtual void vf40();
    virtual void vf44();
    virtual void GetExtents(CVector3&, CVector3&) const;
};

class CVolFiber : public IVolume {
public:
    CVolFiber(const CLine3&);
    virtual ~CVolFiber();

    unsigned char unknown04[12];
    CLine3 m_line;
};

class CVolSphere {
public:
    void SetCenter(CVector3);
};

class CSurfaceType {
public:
    static short GetSoundIndex(int);
};

struct SurfaceView {
    unsigned char unknown00[12];
    short m_type : 11;
};

/* inferred: a contact of a collision: its normal, position, body part or surface, and the
   next contact */
struct ContactView {
    CVector3 m_normal;
    CVector3 m_position;
    unsigned char unknown20[4];
    union {
        int m_part;
        SurfaceView* m_surface;
    };
    unsigned char unknown28[4];
    ContactView* m_next;
};

/* inferred: the collision's two objects, its time and its contact list */
class CCollision {
public:
    CCollision();
    ~CCollision();

    ISceneNode* m_object;
    ISceneNode* m_other;
    unsigned char unknown08[4];
    float m_t;
    unsigned char unknown10[8];
    ContactView* m_contacts;
    unsigned char unknown1c[4];
};

/* inferred: the bullet event block (24 bytes) passed with script event 43 */
struct BulletHitEvent {
    BSObject* m_shooter;
    int m_damage;
    int m_side;
    int m_type;
    int m_part;
};

class CBullet {
public:
    virtual void vf008();
    virtual void vf00c();
    virtual void vf010();
    virtual void vf014();
    virtual void vf018();
    virtual void vf01c();
    virtual void vf020();
    virtual void vf024();
    virtual void vf028();
    virtual void vf02c();
    virtual void vf030();
    virtual void vf034();
    virtual void vf038();
    virtual void vf03c();
    virtual void vf040();
    virtual void vf044();
    virtual void GetForward(CVector3&) const;
    virtual void vf04c();
    virtual void vf050();
    virtual void vf054();
    virtual void vf058();
    virtual void vf05c();
    virtual void vf060();
    virtual void vf064();
    virtual void vf068();
    virtual void vf06c();
    virtual void vf070();
    virtual void vf074();
    virtual void vf078();
    virtual void vf07c();
    virtual void vf080();
    virtual void vf084();
    virtual void vf088();
    virtual void vf08c();
    virtual void vf090();
    virtual void vf094();
    virtual void vf098();
    virtual void vf09c();
    virtual void vf0a0();
    virtual void vf0a4();
    virtual void vf0a8();
    virtual void vf0ac();
    virtual void vf0b0();
    virtual void vf0b4();
    virtual void vf0b8();
    virtual void vf0bc();
    virtual void vf0c0();
    virtual void vf0c4();
    virtual void vf0c8();
    virtual void vf0cc();
    virtual void vf0d0();
    virtual void vf0d4();
    virtual void vf0d8();
    virtual void vf0dc();
    virtual void vf0e0();
    virtual void vf0e4();
    virtual void vf0e8();
    virtual void vf0ec();
    virtual void vf0f0();
    virtual void vf0f4();
    virtual void vf0f8();
    virtual void vf0fc();
    virtual void vf100();
    virtual void vf104();
    virtual void vf108();
    virtual void vf10c();
    virtual void vf110();
    virtual void vf114();
    virtual void vf118();
    virtual void vf11c();
    virtual void vf120();
    virtual void vf124();
    virtual void vf128();
    virtual void vf12c();
    virtual float GetDamage() const;
    virtual int GetScriptBulletType() const;
    virtual ISceneNode* GetFiredBy() const;
    virtual CThrownBullet* AsThrown();
    virtual CProjectileBullet* AsProjectile();

    CVector3 GetVelocity() const { return m_velocity; }

    unsigned char unknown04[92];
    CVector3 m_velocity;
};

class CLightVolumeManager {
public:
    CLightVolumeManager();
    ~CLightVolumeManager();
    BPDLightVolume* GetVolume();
    void RemoveVolume(BPDLightVolume*);
    void AddVolume(BPDLightVolume*);
    void Update(float);

    unsigned char unknown00[4];
};

class CAnimated {
public:
    virtual ~CAnimated();
    virtual void UpdateAnimation(float);
    virtual void Draw(CDrawContext&);
    virtual bool ExtractRotation(long*);
    virtual bool ExtractTranslation(CVector3*);
    virtual void SetRotation(long);

    unsigned long SetAnimationState(unsigned short, float, unsigned short&, long, void*);
    void SetGeomNodeState(long, long);
    static void InitClass();
};

class CAnimSkinned : public CAnimated {
public:
    CAnimSkinned();
    virtual ~CAnimSkinned();
    void Init(CAnimObject*, unsigned short, CharFrame_t*);
    void UpdateSkeletonMatrices(CMatrix*, CVector3*, CVector3*);

    unsigned char unknown04[44];
};

/* inferred: the animated volume's flag at +32 */
class CAnimatedVolume {
public:
    virtual ~CAnimatedVolume();
    void Set(CObjLocationCollisionData*, CObjLocationCollisionMap*);

    unsigned char unknown04[28];
    bool m_flag20;
};

class CCharacterShadow {
public:
    static void InitClass();
};

/* inferred: a node of the scene's extent lists, its value at +8 */
struct ExtentNodeView {
    unsigned char unknown00[8];
    float m_value;
};

// inferred: compound-copy multiply helper (keeps the product in its first operand's register)
inline float Mul(float a, float b) { float r = a; r *= b; return r; }

// inferred: a whole-vector copy as one 16-byte union (both doubles loaded, then stored), the
// form some assignments in this file have; the shared operator= stores each double after its load
union CVector3Words {
    double pair[2];
    float v[4];
};

inline void CopyWords(CVector3& to, const CVector3& from) {
    *(CVector3Words*)&to = *(const CVector3Words*)&from;
}

// inferred: the constructor's vector setter stores z first
inline void SetZYX(CVector3& v, float x, float y, float z) {
    v.z = z;
    v.y = y;
    v.x = x;
}

// inferred: the collision normal is normalised with one divide per component
inline void NormalizeEach(CVector3& v) {
    float length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    if (length != 0.0f) {
        v.x *= 1.0f / length;
        v.y *= 1.0f / length;
        v.z *= 1.0f / length;
    }
}

class CAnimObject : public IMovingSceneNode {
public:
    struct CClsnInfo {
        CClsnInfo() {}
        CClsnInfo(const CClsnInfo& o) {
            m_normal = o.m_normal;
            m_object = o.m_object;
        }

        CVector3 m_normal;
        IMovingSceneNode* m_object;
    };

    CAnimObject();
    virtual void MarkForDestruction(int);
    virtual ~CAnimObject();
    virtual void Destroy();
    virtual void BeginUpdate(float);
    virtual void ConstrainVelocity();
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
    virtual bool IsDrawEnabled() const;
    virtual EClsnId GetCollisionId() const;
    virtual void SetCollisionId(EClsnId);
    virtual BSObject* GetScriptObject() const;
    virtual void TriggerScriptEvent(int, void*, bool);
    virtual void HandleBulletCollision(CBullet*, const CCollision&);
    virtual CAnimObject* AsAnimObject();
    virtual const CAnimObject* AsAnimObject() const;
    virtual void Reset();
    virtual void Halt();
    virtual void ApplyForceTo(CVector3, float);
    virtual void SetTMLocalToWorld(const CMatrix&);
    virtual void PreTransform(const CMatrix&);
    virtual void Transform(const CMatrix&);
    virtual void Move(CVector3);
    virtual void Rotate(CVector3, float);
    virtual void Pitch(float);
    virtual void Roll(float);
    virtual void Yaw(float);
    virtual void SetPosition(CVector3);
    virtual void SetBasis(CVector3, CVector3, CVector3);
    virtual void Orthonormalize();
    virtual CVector3 GetWorldLinearVelocity() const;
    virtual void EnterLightVolume(BPDLightVolume*);
    virtual void ExitLightVolume(BPDLightVolume*);
    virtual BPDLightVolume* GetLightVolume();
    virtual void Init();
    virtual void UpdateAnimation(float);
    virtual void SetScript(BSObject*);
    virtual void UpdateLocalBoundingVolume(EVolumeType, const CVector3&, const CVector3&);
    virtual int AttachObject(CStaticObject*, char*, int);
    virtual int DetachObject(char*);
    virtual int GetAttachPointMatrix(char*, CMatrix*);
    virtual void AttachFloatingBone();
    virtual void DetachFloatingBone();

    static void InitClass(CRenderList*, CScreen*);
    void SetAttached(bool);
    void AttachToLocation(const CMatrix&);
    void SetYaw(float);
    float GetYaw() const;
    float DetectGround();
    void SetWorldBoundingVolume(EVolumeType, IVolume*);
    void SetLocalBoundingVolume(EVolumeType, IVolume*);
    void SetNodeState(long, long);
    void SetMoveTarget(const CVector3&);
    bool FinishAnimationState(long, bool);
    void QueueAnimationState(unsigned short, float, long, void*);
    void SetAnimationState(unsigned short, float, long, void*);
    void BindLocationCollision(CObjLocationCollisionData*, CObjLocationCollisionMap*);

    CVector3 GetUp() const { return m_tm.up; }
    CVector3 GetFwd() const { return m_tm.front; }
    CVector3 GetRight() const { return m_tm.right; }
    CVector3 GetPos() const { return m_tm.pos; }
    CVector3 UpRow() const { return m_tm.up; }
    CVector3 ForwardRow() const { return m_tm.front; }
    CVector3 RightRow() const { return m_tm.right; }
    CVector3 PositionRow() const { return m_tm.pos; }

    int data24;
    int data28;
    int data2c;
    CAnimSkinned m_skinned;
    unsigned char unknown0060[3808];
    CMatrix m_matrices[80];
    CMatrix m_tm;
    union {
        IVolume* m_localVolume3;
        void* m_drawData;
    };
    IVolume* m_localVolume;
    union {
        IVolume* m_worldVolume3;
        AnimDrawerView* m_drawer;
    };
    IVolume* m_worldVolume;
    BSObject* m_script;
    int m_collisionId;
    union {
        CAnimatedVolume* m_volume;
        CVolSphere* m_sphere;
    };
    unsigned char unknown239c[4];
    CVector3 m_moveTarget;
    long m_yaw;
    unsigned char unknown23b4[4];
    CVector3 m_lastPosition;
    CVector3 m_velocity;
    CVector3 m_animDelta;
    float m_animScale;
    long m_lastYaw;
    float m_turn;
    unsigned char unknown23f4[4];
    CVector3 m_bonePosition;
    CQuaternion m_boneRotation;
    CVector3 m_nextBonePosition;
    CQuaternion m_nextBoneRotation;
    dwi::fast_vec<CClsnInfo> m_collisions;
    bool m_flag2450;
    bool m_flag2451;
    bool m_finished;
    bool m_finishPending;
    bool m_onGround;
    bool m_falling;
    union {
        bool m_forced;
        bool m_jumping;
    };
    bool m_noRotation;
    unsigned char unknown2458;
    bool m_useTranslation;
    union {
        bool m_attached;
        bool m_frozen;
    };
    union {
        bool m_attachedToLocation;
        bool m_useFixedTM;
    };
    unsigned char unknown245c[4];
    CMatrix m_fixedTM;
    bool m_queued;
    unsigned char unknown24a1;
    unsigned short m_queuedState;
    float m_queuedTime;
    long m_queuedValue;
    void* m_queuedData;
    bool m_finishing;
    bool m_hasMoveTarget;
    bool m_keepZ;
    bool m_detectGround;
    bool m_moved;
    bool m_stopped;
    unsigned char unknown24b6[2];
    CLightVolumeManager m_lightVolumes;
    unsigned char unknown24bc[20];
    short m_surfaceSound;
    unsigned char unknown24d2[6];
    CClsnInfo m_collisionStorage[128];
    float m_value30d8;
    unsigned char unknown30dc[3812];
};

void CAnimObject::InitClass(CRenderList*, CScreen*) {
    CAnimated::InitClass();
    CCharacterShadow::InitClass();
}

CAnimObject::CAnimObject() {
    m_volume = 0;
}

void CAnimObject::Init() {
    m_skinned.Init(this, 0, 0);
    data24 = 0;
    data28 = 0;
    data2c = 0;
    m_tm.Ident();
    SetZYX(m_velocity, 0.0f, 0.0f, 0.0f);
    SetZYX(m_lastPosition, 0.0f, 0.0f, 0.0f);
    SetZYX(m_animDelta, 0.0f, 0.0f, 0.0f);
    m_yaw = 0;
    for (int i = 0; i < 80; i++)
        m_matrices[i].Ident();
    m_localVolume3 = 0;
    m_localVolume = 0;
    m_worldVolume3 = 0;
    m_worldVolume = 0;
    m_collisionId = -1;
    m_collisions.reserve(m_collisionStorage, 128);
    m_script = 0;
    m_frozen = 0;
    m_useFixedTM = 0;
    m_useTranslation = 0;
    m_hasMoveTarget = 0;
    m_finished = 1;
    m_finishPending = 0;
    m_keepZ = 1;
    m_detectGround = 1;
    m_falling = 0;
    m_forced = 0;
    m_onGround = 1;
    m_queued = 0;
    m_finishing = 0;
    m_flag2451 = 0;
    m_surfaceSound = 1;
    m_stopped = 0;
    m_value30d8 = 0.0f;
}

void CAnimObject::BindLocationCollision(CObjLocationCollisionData* data, CObjLocationCollisionMap* map) {
    if (m_volume)
        m_volume->Set(data, map);
}

CAnimObject::~CAnimObject() {
    if (m_volume) {
        delete m_volume;
        m_volume = 0;
    }
}

void CAnimObject::SetAnimationState(unsigned short state, float time, long value, void* data) {
    unsigned short next;
    if (m_skinned.SetAnimationState(state, time, next, value, data) == 0xf0f0f0f0) {
        QueueAnimationState(state, time, value, data);
        m_skinned.SetAnimationState(next, time, next, value, data);
    } else {
        m_queued = false;
    }
    m_finishPending = false;
}

void CAnimObject::QueueAnimationState(unsigned short state, float time, long value, void* data) {
    m_queuedState = state;
    m_queuedTime = time;
    m_queuedValue = value;
    m_queuedData = data;
    m_queued = true;
}

bool CAnimObject::FinishAnimationState(long event, bool keep) {
    if (!m_finished || !m_finishPending) {
        if (m_queued && !keep) {
            m_queued = false;
            m_finishing = true;
            SetAnimationState(m_queuedState, m_queuedTime, m_queuedValue, m_queuedData);
            m_finished = true;
        } else {
            m_finishing = false;
            BSObjectTriggerEvent(m_script, event, 0, 0, false);
            if (!keep)
                m_finishPending = true;
        }
    }
    return false;
}

void CAnimObject::UpdateAnimation(float dt) {
    m_turn = 0.0f;
    m_noRotation = false;
    m_animDelta.x = m_animDelta.y = m_animDelta.z = 0.0f;
    m_animScale = 1.0f;
    m_skinned.UpdateAnimation(dt);
    long rotation = 0;
    if (!m_noRotation && m_skinned.ExtractRotation(&rotation)) {
        float angle = 6.2831855f * (float)rotation / 16777216.0f;
        if (angle > 3.1415927f)
            angle -= 6.2831855f;
        else if (angle < -3.1415927f)
            angle += 6.2831855f;
        m_turn += angle;
    }
    CVector3 translation;
    bool translated = false;
    if (m_skinned.ExtractTranslation(&translation) && m_useTranslation)
        translated = true;
    float inv = 1.0f / dt;
    m_moved = false;
    if (!m_forced && !m_stopped) {
        if (translated) {
            m_velocity.x = inv * (m_animScale * translation.x + m_animDelta.x);
            m_velocity.y = inv * (m_animScale * translation.y + m_animDelta.y);
            if (!m_keepZ)
                m_velocity.z = inv * translation.z;
            m_moved = true;
        } else {
            m_velocity.x = 0.0f;
            m_velocity.y = 0.0f;
            if (!m_keepZ)
                m_velocity.z = 0.0f;
        }
    } else if (m_stopped) {
        m_velocity.x = m_velocity.y = m_velocity.z = 0.0f;
    }
    m_turn *= inv;
    if (m_volume)
        m_volume->m_flag20 = false;
}

void CAnimObject::SetMoveTarget(const CVector3& target) {
    CopyWords(m_moveTarget, target);
    m_hasMoveTarget = true;
}

void CAnimObject::SetNodeState(long node, long state) {
    m_skinned.SetGeomNodeState(node, state);
}

void CAnimObject::Draw(CDrawContext& context) {
    if (!g_config.noAnimObjects) {
        context.m_localToWorld = &m_tm;
        CVector3 min;
        CVector3 max;
        m_skinned.UpdateSkeletonMatrices(m_matrices, &min, &max);
        m_skinned.Draw(context);
        UpdateLocalBoundingVolume((EVolumeType)3, min, max);
    }
}

void CAnimObject::SetLocalBoundingVolume(EVolumeType type, IVolume* volume) {
    switch (type) {
    case 4:
        break;
    case 3:
        m_localVolume3 = volume;
        break;
    case 0:
        m_localVolume = volume;
        break;
    }
}

void CAnimObject::SetWorldBoundingVolume(EVolumeType type, IVolume* volume) {
    switch (type) {
    case 4:
        break;
    case 3:
        m_worldVolume3 = volume;
        break;
    case 0:
        m_worldVolume = volume;
        break;
    }
}

IVolume* CAnimObject::GetLocalBoundingVolume(EVolumeType type) const {
    switch (type) {
    case 3:
        return m_localVolume3;
    case 0:
    case 1:
    case 2:
        return m_localVolume;
    case 4:
        return 0;
    default:
        return 0;
    }
}

IVolume* CAnimObject::GetWorldBoundingVolume(EVolumeType type) const {
    switch (type) {
    case 3:
        return m_worldVolume3;
    case 0:
    case 1:
    case 2:
        return m_worldVolume;
    case 4:
        return 0;
    default:
        return 0;
    }
}

void CAnimObject::TriggerScriptEvent(int event, void* data, bool flag) {
    if (m_script)
        BSObjectTriggerEvent(m_script, event, data, 0, flag);
}

__declspec(weak) CProjectileBullet* CBullet::AsProjectile() {
    return 0;
}

__declspec(weak) CThrownBullet* CBullet::AsThrown() {
    return 0;
}

__declspec(weak) int CBullet::GetScriptBulletType() const {
    return -1;
}

__declspec(weak) float CBullet::GetDamage() const {
    return 0.0f;
}

__declspec(weak) ISceneNode* CBullet::GetFiredBy() const {
    return 0;
}

__declspec(weak) void CBullet::GetForward(CVector3& forward) const {
    forward = GetVelocity();
}

void CAnimObject::HandleBulletCollision(CBullet* bullet, const CCollision& collision) {
    ContactView* contact;
    CProjectileBullet* projectile = bullet->AsProjectile();
    CThrownBullet* thrown = bullet->AsThrown();
    if (projectile) {
        BulletHitEvent* event = (BulletHitEvent*)BSEventGetEventMemoryBlock(24);
        event->m_type = bullet->GetScriptBulletType();
        event->m_damage = (int)bullet->GetDamage();
        ISceneNode* shooter = bullet->GetFiredBy();
        if (shooter)
            event->m_shooter = shooter->GetScriptObject();
        else
            event->m_shooter = 0;
        CVector3 direction;
        bullet->GetForward(direction);
        CVector3 forward;
        GetForward(forward);
        CVector3 right;
        GetRightward(right);
        float front = forward.x * direction.x + forward.y * direction.y + forward.z * direction.z;
        float side = right.x * direction.x + right.y * direction.y + right.z * direction.z;
        if (side < -0.7071f)
            event->m_side = 3;
        else if (side > 0.7071f)
            event->m_side = 2;
        else if (front < 0.0f)
            event->m_side = 0;
        else
            event->m_side = 1;
        contact = collision.m_contacts;
        event->m_part = contact->m_part;
        switch (event->m_part) {
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
            event->m_damage = (int)(0.5f * event->m_damage);
            break;
        case 0:
        case 1:
            event->m_damage = (int)(3.0f * event->m_damage);
            break;
        case 2:
        case 9:
        case 10:
        case 11:
        case 12:
            event->m_damage = (int)(1.0f * event->m_damage);
            break;
        }
        BSObjectTriggerEvent(m_script, 43, event, 0, false);
        if (shooter->AsPlayerObject()) {
            g_Shell.Set_hits(1, false);
            switch (event->m_part) {
            case 3:
            case 4:
            case 5:
                g_Shell.Set_shotLocation(1, (EShotLocations)1, false);
                break;
            case 6:
            case 7:
            case 8:
                g_Shell.Set_shotLocation(1, (EShotLocations)2, false);
                break;
            case 13:
            case 14:
            case 15:
                g_Shell.Set_shotLocation(1, (EShotLocations)4, false);
                break;
            case 16:
            case 17:
            case 18:
                g_Shell.Set_shotLocation(1, (EShotLocations)5, false);
                break;
            case 19:
            case 20:
                g_Shell.Set_shotLocation(1, (EShotLocations)6, false);
                break;
            case 0:
            case 1:
                g_Shell.Set_shotLocation(1, (EShotLocations)0, false);
                break;
            case 2:
            case 9:
            case 10:
            case 11:
            case 12:
                g_Shell.Set_shotLocation(1, (EShotLocations)3, false);
                break;
            }
        }
        PlayImpactSound(14, 1, contact->m_position, false);
    } else if (thrown) {
        thrown->Explode();
    }
}

float CAnimObject::DetectGround() {
    float ground = 0.0f;
    CCollision collision;
    int hit = 1;
    m_stopped = false;
    if (m_detectGround && (!m_jumping || m_velocity.z < 0.0f)) {
        IVolume* world = g_scene.m_world->GetWorldBoundingVolume((ISceneNode::EVolumeType)0);
        CVector3 bottom = m_tm.GetPos();
        bottom.z -= 0.05f;
        CLine3 line(CVector3(bottom.x, bottom.y, 1.0f + bottom.z), bottom);
        CVolFiber fiber(line);
        hit = world->TestCollision(fiber, collision, true);
        if (hit == 1) {
            CLine3 up(CVector3(bottom.x, bottom.y, 100.0f + bottom.z), CVector3(bottom.x, bottom.y, bottom.z - 100.0f));
            CVolFiber fiber2(up);
            CCollision collision2;
            if (world->TestCollision(fiber2, collision2, true) == 1)
                m_stopped = true;
        }
    }
    if (m_stopped) {
        m_velocity.x = 0.0f;
        m_velocity.y = 0.0f;
        m_velocity.z = 0.0f;
    } else if (hit != 1) {
        m_onGround = true;
        m_jumping = false;
        ground = 0.99f - collision.m_t;
        if (m_falling) {
            TriggerScriptEvent(133, 0, false);
            m_falling = false;
        }
        m_surfaceSound = 1;
        if (collision.m_contacts && collision.m_contacts->m_surface && collision.m_contacts->m_surface->m_type >= 0)
            m_surfaceSound = CSurfaceType::GetSoundIndex(collision.m_contacts->m_surface->m_type);
    }
    return ground;
}

void CAnimObject::BeginUpdate(float dt) {
    m_lightVolumes.Update(dt);
    m_lastPosition = m_tm.GetPos();
    m_lastYaw = m_yaw;
    CMatrix bone(m_matrices[1]);
    m_boneRotation.SetFromMatrix(bone);
    m_bonePosition = bone.GetPos();
    UpdateAnimation(dt);
    if (m_frozen || m_stopped) {
        m_velocity.x = m_velocity.y = m_velocity.z = 0.0f;
    } else if (m_onGround && m_keepZ) {
        m_velocity.z = 0.0f;
    } else if (m_keepZ) {
        m_velocity.z += -0.005455351f * dt;
        float vz = m_velocity.z;
        if ((float)fabs(vz) > 1.5f) {
            if (vz > 0.0f)
                m_velocity.z = 1.5f;
            else
                m_velocity.z = -1.5f;
        }
        m_falling = true;
    }
    m_flag2450 = false;
}

void CAnimObject::AttemptUpdate(float dt) {
    CVector3 move;
    move.x = dt * m_velocity.x;
    move.y = dt * m_velocity.y;
    move.z = dt * m_velocity.z;
    float turn = m_turn * dt;
    CMatrix bone(m_matrices[1]);
    CQuaternion rotation;
    rotation.SetFromMatrix(bone);
    CVector3 position = bone.pos;
    float pz = m_bonePosition.z;
    float py = m_bonePosition.y;
    float px = m_bonePosition.x;
    m_nextBonePosition.x = px + dt * (position.x - px);
    m_nextBonePosition.y = py + dt * (position.y - py);
    m_nextBonePosition.z = pz + dt * (position.z - pz);
    m_nextBoneRotation.Slerp(rotation, m_boneRotation, dt);
    m_nextBoneRotation.GetMatrix(bone);
    bone.SetPos(m_nextBonePosition);
    if (m_tm.pos.z < -250.0f)
        BSObjectTriggerEvent(m_script, 45, 0, 0, false);
    if (!m_frozen) {
        m_yaw = (m_lastYaw + ((long)(2670176.8f * turn) & 0xffffff)) & 0xffffff;
        m_tm.BuildRotZ(6.2831855f * (float)m_yaw / 16777216.0f);
        m_tm.SetPos(m_lastPosition);
        m_tm.Translate(move);
        m_skinned.SetRotation(m_yaw);
    } else if (m_useFixedTM) {
        m_tm = m_fixedTM;
    }
    if (m_localVolume) {
        bone.Multiply(bone, m_tm);
        m_sphere->SetCenter(CVector3(0.0f, 0.0f, 0.0f));
        m_worldVolume->TransformedCopy(*m_localVolume, bone);
        CVector3 min;
        CVector3 max;
        m_worldVolume->GetExtents(min, max);
        ((ExtentNodeView*)data0c)->m_value = min.x - CONTACT_EPSILON;
        ((ExtentNodeView*)data14)->m_value = min.y - CONTACT_EPSILON;
        ((ExtentNodeView*)data10)->m_value = CONTACT_EPSILON + max.x;
        ((ExtentNodeView*)data18)->m_value = CONTACT_EPSILON + max.y;
    } else {
        CVector3 p;
        GetPosition(p);
        ((ExtentNodeView*)data0c)->m_value = p.x - CONTACT_EPSILON;
        ((ExtentNodeView*)data14)->m_value = p.y - CONTACT_EPSILON;
        ((ExtentNodeView*)data10)->m_value = CONTACT_EPSILON + p.x;
        ((ExtentNodeView*)data18)->m_value = CONTACT_EPSILON + p.y;
    }
    m_onGround = false;
    m_collisions.m_size = 0;
    m_flag2450 = false;
}

void CAnimObject::CommitUpdate() {
    ConstrainVelocity();
    m_lastPosition = m_tm.GetPos();
    m_lastYaw = m_yaw;
    CopyWords(m_bonePosition, m_nextBonePosition);
    m_boneRotation = m_nextBoneRotation;
    float ground = DetectGround();
    if (ground > 0.0f) {
        m_tm.Translate(CVector3(0.0f, 0.0f, ground));
        m_lastPosition = m_tm.GetPos();
    }
}

void CAnimObject::ConstrainVelocity() {
    if (m_collisions.m_size == 0 || m_flag2450)
        return;
    {
        CClsnInfo* it;
        CClsnInfo* begin;
        CClsnInfo* end;
        CClsnInfo* first;
        CClsnInfo* second;
        CVector3 bv;
        float best;
        float best2;
        float b;
        float a;
        float zero;
        m_flag2450 = true;
        best = 1.0f;
        best2 = best;
        zero = 0.0f;
        begin = m_collisions.m_data;
        end = m_collisions.m_data + m_collisions.m_size;
        first = end;
        second = end;
        CVector3 velocity = m_velocity;
        for (it = begin; it != end; ++it) {
            CVector3 normal = it->m_normal;
            CVector3 v = m_velocity;
            if (it->m_object)
                v -= it->m_object->GetWorldLinearVelocity();
            float d = v.x * normal.x + v.y * normal.y + v.z * normal.z;
            if (d < zero && d < best) {
                best = d;
                first = it;
            }
        }
        bool found = first != end;
        if (found) {
            bv.Scale(first->m_normal, best);
            m_velocity.Sub(m_velocity, bv);
            for (it = begin; it != end; ++it) {
                if (it == first)
                    continue;
                CVector3 normal = it->m_normal;
                CVector3 v = m_velocity;
                if (it->m_object)
                    v -= it->m_object->GetWorldLinearVelocity();
                float d = v.x * normal.x + v.y * normal.y + v.z * normal.z;
                if (d < -1e-6f && d < best2) {
                    best2 = d;
                    second = it;
                }
            }
            found = second != end;
        }
        if (found) {
            float det;
            float e;
            float bd;
            CVector3& n2 = second->m_normal;
            float py;
            float px;
            CVector3& n1 = first->m_normal;
            float dc;
            float d2;
            float inv;
            dc = best * (n1.x * n2.x + n1.y * n2.y + n1.z * n2.z);
            d2 = best2 + dc;
            px = d2 * n2.x;
            float pz;
            py = d2 * n2.y;
            pz = d2 * n2.z;
            e = px * n1.x + py * n1.y + pz * n1.z;
            bd = best * d2;
            det = bd - e * dc;
            bool solved;
            if ((float)fabs(det) < 1e-5f) {
                solved = false;
            } else {
                inv = 1.0f / det;
                b = inv * (d2 * best - e * d2);
                a = inv * (bd - dc * best);
                solved = true;
            }
            if (solved) {
                float ty;
                float sx;
                float sy;
                sx = Mul(b, bv.x);
                float dx;
                float dz;
                sy = Mul(b, bv.y);
                float ux;
                float tz;
                float tx;
                tx = Mul(px, a);
                ux = sx + tx;
                dx = Mul(ux, 1.05f);
                float uz;
                ty = Mul(py, a);
                float sz;
                float uy;
                m_velocity.x = velocity.x - dx;
                uy = sy + ty;
                float dy;
                dy = Mul(uy, 1.05f);
                sz = Mul(b, bv.z);
                tz = Mul(pz, a);
                uz = sz + tz;
                dz = Mul(uz, 1.05f);
                m_velocity.y = velocity.y - dy;
                m_velocity.z = velocity.z - dz;
            }
            for (it = begin; found && it != end; ++it) {
                if (it == first || it == second)
                    continue;
                CVector3 normal = it->m_normal;
                if (m_velocity.x * normal.x + m_velocity.y * normal.y + m_velocity.z * normal.z < -1e-6f) {
                    m_velocity.x = m_velocity.y = m_velocity.z = 0.0f;
                    found = false;
                }
            }
        }
        if (sqrtf(m_velocity.x * m_velocity.x + m_velocity.y * m_velocity.y + m_velocity.z * m_velocity.z) > 0.5f)
            m_velocity.x = m_velocity.y = m_velocity.z = 0.0f;
    }
}

void CAnimObject::OnCollision(const CCollision& collision) {
    m_flag2450 = false;
    bool first = collision.m_object == this;
    ISceneNode* other = collision.m_object == this ? collision.m_other : collision.m_object;
    for (ContactView* contact = collision.m_contacts; contact; contact = contact->m_next) {
        CClsnInfo info;
        info.m_object = 0;
        if (m_flag2451) {
            int id = GetCollisionId();
            if ((id == 5 || id == 7) && (other->AsAnimObject() || other->AsPlayerObject()))
                info.m_object = other->AsMovingNode();
            else if (other->AsAnimObject())
                info.m_object = other->AsMovingNode();
        }
        if (first) {
            info.m_normal.x = -contact->m_normal.x;
            info.m_normal.y = -contact->m_normal.y;
            info.m_normal.z = -contact->m_normal.z;
        } else {
            info.m_normal = contact->m_normal;
        }
        info.m_normal.z = 0.0f;
        if (info.m_normal.x * info.m_normal.x + info.m_normal.y * info.m_normal.y + info.m_normal.z * info.m_normal.z > 0.001f) {
            NormalizeEach(info.m_normal);
            m_collisions.push_back(info);
        }
    }
}

void CAnimObject::Halt() {
    m_velocity.x = m_velocity.y = m_velocity.z = 0.0f;
    m_turn = 0.0f;
}

void CAnimObject::ApplyForceTo(CVector3 force, float strength) {
    strength *= 0.001953125f;
    CopyWords(m_velocity, force);
    NormalizeEach(m_velocity);
    m_velocity.x *= strength;
    m_velocity.y *= strength;
    m_velocity.z *= strength;
    m_forced = true;
    m_onGround = false;
}

void CAnimObject::GetTMLocalToWorld(CMatrix& matrix) const {
    matrix = m_tm;
}

bool CAnimObject::IsVisible(CDrawContext& context) const {
    if (m_drawData)
        m_drawer->unknown010(m_drawData, m_tm);
    if (m_drawer)
        return m_drawer->unknown044(context);
    CVector3 position;
    GetPosition(position);
    return context.m_camera->IsPointVisible(position, context);
}

bool CAnimObject::IsDrawEnabled() const {
    return true;
}

EClsnId CAnimObject::GetCollisionId() const {
    return (EClsnId)m_collisionId;
}

void CAnimObject::Reset() {
    m_tm.Ident();
    m_yaw = 0;
    m_onGround = false;
}

void CAnimObject::SetPosition(CVector3 position) {
    m_tm.SetPos(position);
}

void CAnimObject::Move(CVector3 delta) {
    m_tm.Translate(delta);
}

void CAnimObject::Yaw(float angle) {
    m_tm.RotateZ(angle);
    m_yaw = (m_yaw + (int)(2670176.75f * angle)) & 0xFFFFFF;
}

float CAnimObject::GetYaw() const {
    return 6.2831855f * m_yaw / 16777216.0f;
}

void CAnimObject::SetYaw(float yaw) {
    m_yaw = (int)(2670176.75f * yaw) & 0xFFFFFF;
    m_lastYaw = m_yaw;
}

void CAnimObject::Orthonormalize() {
    m_tm.Orthonormalize();
}

void CAnimObject::GetPosition(CVector3& v) const {
    v = GetPos();
}

void CAnimObject::GetRightward(CVector3& v) const {
    v = GetRight();
}

void CAnimObject::GetForward(CVector3& v) const {
    v = GetFwd();
}

void CAnimObject::GetUpward(CVector3& v) const {
    v = GetUp();
}

void CAnimObject::EnterLightVolume(BPDLightVolume* volume) {
    m_lightVolumes.AddVolume(volume);
}

void CAnimObject::ExitLightVolume(BPDLightVolume* volume) {
    m_lightVolumes.RemoveVolume(volume);
}

BPDLightVolume* CAnimObject::GetLightVolume() {
    return m_lightVolumes.GetVolume();
}

void CAnimObject::SetTMLocalToWorld(const CMatrix& matrix) {
    m_tm = matrix;
}

void CAnimObject::AttachToLocation(const CMatrix& location) {
    m_attached = true;
    m_attachedToLocation = true;
    m_fixedTM = location;
}

void CAnimObject::SetAttached(bool attached) {
    m_attached = attached;
    m_attachedToLocation = false;
}

void CAnimObject::MarkForDestruction(int flag) {
    // The ISubject and ISceneNode bases are at the start of the object.
    ISubject::MarkForDestruction(flag);
    g_scene.Remove(*this);
}

void CAnimObject::Destroy() {
}

__declspec(weak) void CAnimObject::SetBasis(CVector3, CVector3, CVector3) {
}

__declspec(weak) void CAnimObject::Roll(float) {
}

__declspec(weak) void CAnimObject::Pitch(float) {
}

__declspec(weak) void CAnimObject::Rotate(CVector3, float) {
}

__declspec(weak) void CAnimObject::Transform(const CMatrix&) {
}

__declspec(weak) void CAnimObject::PreTransform(const CMatrix&) {
}

__declspec(weak) void CAnimObject::DetachFloatingBone() {
}

__declspec(weak) void CAnimObject::AttachFloatingBone() {
}

__declspec(weak) int CAnimObject::GetAttachPointMatrix(char*, CMatrix*) {
    return 0;
}

__declspec(weak) int CAnimObject::DetachObject(char*) {
    return 0;
}

__declspec(weak) int CAnimObject::AttachObject(CStaticObject*, char*, int) {
    return 0;
}

__declspec(weak) CAnimObject* CAnimObject::AsAnimObject() {
    return this;
}

__declspec(weak) const CAnimObject* CAnimObject::AsAnimObject() const {
    return this;
}

__declspec(weak) void CAnimObject::UpdateLocalBoundingVolume(EVolumeType, const CVector3&, const CVector3&) {
}
