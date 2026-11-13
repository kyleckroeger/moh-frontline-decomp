// animplayer.cpp: CAnimatedPlayer, the whole file, with its vtable and the RTTI records of its
// chain (IDestructible .. CAnimatedPlayer): the weak copies of its empty UpdateAI and its
// AsAnimatedPlayerObject casts; CommitUpdate, AttemptUpdate, BeginUpdate, the bounding
// volumes, the flocking parameters and UpdateAnimation; Draw and HandleBulletCollision; InitAI
// and Init; the destructor and the constructor.
//
// The names come from the mangled symbols; the class chain and the vtable from
// __RTTI__15CAnimatedPlayer and __vt__15CAnimatedPlayer (ISceneNode's and IMovingSceneNode's
// slots from the shared headers, CAnimObject's from +0x124 and CSoldierObject's InitAI at
// +0x148, as __vt__11CAnimObject and __vt__14CSoldierObject).
//
// Inferred, not in any mangled name: the data members (named by offset or role; the split
// between CAnimObject and CSoldierObject at +0x3fc0 is a choice, CAnimatedPlayer's own members
// start at +0x44b0 since its constructor sets only that word), the result types, the reset
// helpers, and the views of the weapon, the volumes, the animation and morph classes, the jaw
// controller, the particle system, the bullet and the draw context; the extent nodes are
// ISceneNode's data words at +0x0c..+0x18. CVector3 is the shared header with the assignment
// through the double-pair union (both doublewords loaded before either is stored). The unit is
// compiled -inline auto,deferred, which emits the functions in reverse source order, so they
// are written last to first.
/* inferred: this file's CVector3 is the shared game/CVector3.h with the assignment through the
   double-pair union */
#define GAME_CVECTOR3_H
#include "game/msl_sqrtf.h"

struct CVector3XYZW {
    float x, y, z, w;
};

union CVector3Data {
    double pair[2];
    float v[4];
    CVector3XYZW c;
};

struct CVector3Pair {
    double a;
    double b;
};

struct CVector3Base {
    float x, y, z, w;
} __attribute__((aligned(8)));

class CVector3 : public CVector3Base {
public:
    CVector3() {}
    CVector3(float ax, float ay, float az) { x = ax; y = ay; z = az; }
    CVector3(const CVector3& other) : CVector3Base(other) {}
    CVector3& operator=(const CVector3& other) {
        *(CVector3Data*)this = *(const CVector3Data*)&other;
        return *this;
    }

    void Set(float ax, float ay, float az) { x = ax; y = ay; z = az; }
    float LengthSq() const { return x * x + y * y + z * z; }
    float LengthSquared() const { return x * x + y * y + z * z; }
    float Length() const { return sqrtf(x * x + y * y + z * z); }
    float Normalize() {
        float length = Length();
        if (length != 0.0f) {
            float scale = 1.0f / length;
            x *= scale;
            y *= scale;
            z *= scale;
        }
        return length;
    }
    float Dot(const CVector3& o) const { return x * o.x + y * o.y + z * o.z; }
    void Cross(const CVector3& a, const CVector3& b) {
        x = a.y * b.z - a.z * b.y;
        y = a.z * b.x - a.x * b.z;
        z = a.x * b.y - a.y * b.x;
    }
    void DotDotDot(const CVector3& a, const CVector3& b, const CVector3& c, const CVector3& v) {
        x = a.x * v.x + a.y * v.y + a.z * v.z;
        y = b.x * v.x + b.y * v.y + b.z * v.z;
        z = c.x * v.x + c.y * v.y + c.z * v.z;
    }
    void Abs(const CVector3& o) { x = __fabs(o.x); y = __fabs(o.y); z = __fabs(o.z); }
    CVector3& operator+=(const CVector3& o) { x += o.x; y += o.y; z += o.z; return *this; }
    CVector3& operator-=(const CVector3& o) { x -= o.x; y -= o.y; z -= o.z; return *this; }
    CVector3& operator*=(float f) { x *= f; y *= f; z *= f; return *this; }
    void Scale(const CVector3& o, float k) { x = k * o.x; y = k * o.y; z = k * o.z; }
    void Sub(const CVector3& a, const CVector3& b) { x = a.x - b.x; y = a.y - b.y; z = a.z - b.z; }
    void Add(const CVector3& o) { x += o.x; y += o.y; z += o.z; }
    void Unitize() {
        float length = sqrtf(LengthSq());
        if (length != 0.0f) {
            x *= 1.0f / length;
            y *= 1.0f / length;
            z *= 1.0f / length;
        }
    }
    void Constrain(const CVector3&, float);
} __attribute__((aligned(8)));


#include "game/CPlayerObject.h"
#include "game/CScene.h"

class CThrownBullet;
class CProjectileBullet;
class CStaticObject;
struct CharFrame_t;
enum EMoodType {};
enum EScriptBulletType {};

extern CScene g_scene;
extern bool g_bInMultiplayerMode;
extern const float CONTACT_EPSILON;
extern "C" void* memset(void*, int, unsigned long);
extern "C" int rand();
float MathFunRandomReal(float, float);
void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);
void* AllocateBoxVolume();
void* AllocateAnimatedVolume();

/* inferred views */
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

/* the volumes' virtual slots used here (TransformedCopy +0x10, GetExtents +0x48) */
class IVolume {
public:
    virtual ~IVolume();
    virtual IVolume* Create() const;
    virtual void TransformedCopy(const IVolume&, const CMatrix&);
    virtual void vf14();
    virtual void vf18();
    virtual void vf1c();
    virtual void vf20();
    virtual void vf24();
    virtual void vf28();
    virtual void vf2c();
    virtual void vf30();
    virtual void vf34();
    virtual void vf38();
    virtual void vf3c();
    virtual void vf40();
    virtual void vf44();
    virtual void GetExtents(CVector3&, CVector3&) const;
};

class CVolSphere {
public:
    void SetRadius(float);
    void SetCenter(CVector3);

    unsigned char unknown000[1968];
    void* m_owner;
};

class CVolBox {
public:
    void SetCorners(CVector3, CVector3);
    void SetBasis(CVector3, CVector3, CVector3);
    void SetWidth(float);
    void SetHeight(float);
    void SetDepth(float);
    void SetCenter(CVector3);
};

class CAnimated {
public:
    virtual ~CAnimated();
    virtual void UpdateAnimation(float);
    virtual void vf10();
    virtual void vf14();
    virtual void vf18();
    virtual void SetRotation(long);
    bool ValidState(unsigned short);
    void SetAnimationState(unsigned short, float, unsigned short&, long, void*);
};

class CAnimSkinned : public CAnimated {
public:
    unsigned char unknown04[44];
};

class CAnimMorph : public CAnimated {
public:
    void SetBlendTargetWeight(unsigned long, float);
    void Init(CAnimObject*, unsigned short, CharFrame_t*);
};

class CJawController {
public:
    void Update(int);
};

/* a node of the scene's extent lists, its value at +8 */
struct ExtentNodeView {
    unsigned char unknown00[8];
    float m_value;
};

class CWeapon {
public:
    unsigned char GetMuzzleFlashStatus();

    unsigned char unknown000[480];
    unsigned char m_flag1e0 : 1;
    unsigned char unknown1e0 : 7;
    unsigned char unknown1e1[167];
    float m_value288;
    unsigned char unknown28c[12];
    int m_type;
    unsigned char unknown29c[36];
    unsigned char unknown2c0a : 5;
    unsigned char muzzleFlash : 1;
    unsigned char unknown2c0b : 2;
};

class CLightVolumeManager {
public:
    void Update(float);

    unsigned char unknown00[4];
};

/* the draw context's value at +0x114 */
class CDrawContext {
public:
    unsigned char unknown000[0x114];
    int m_value114;
};

/* a collision contact: normal and position */
struct ContactView {
    CVector3 m_normal;
    CVector3 m_position;
};

class CCollision {
public:
    unsigned char unknown00[24];
    ContactView* m_contacts;
};

/* the particle system: 28 bytes of members before its table pointer; SetLocalToWorld at +0x8c */
class CParticleSystem {
    unsigned char unknown00[28];

public:
    static CParticleSystem* Create(unsigned long);
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
    virtual void SetLocalToWorld(const CMatrix&);
};

/* the bullet's slots used here (from +0x124 as __vt__7CBullet) */
class CBullet : public IMovingSceneNode {
public:
    virtual void unknown124();
    virtual void unknown128();
    virtual void unknown12c();
    virtual float GetDamage() const;
    virtual EScriptBulletType GetScriptBulletType() const;
    virtual ISceneNode* GetFiredBy() const;
    virtual CThrownBullet* AsThrown();
    virtual CProjectileBullet* AsProjectile();
    virtual void GetVelocity(CVector3&);
    virtual void unknown148();
    virtual void Penetrate(const CCollision&);
};

class CAnimObject : public IMovingSceneNode {
public:
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
    virtual void UpdateLocalBoundingVolume(ISceneNode::EVolumeType, const CVector3&, const CVector3&);
    virtual void AttachObject(CStaticObject*, char*, int);
    virtual void DetachObject(char*);
    virtual void GetAttachPointMatrix(char*, CMatrix*);
    virtual void AttachFloatingBone();
    virtual void DetachFloatingBone();

    unsigned char unknown024[12];
    CAnimSkinned m_skinned;
    unsigned char unknown060[0xca0];
    unsigned char m_frameData[0x240];
    CMatrix m_matrices[80];
    CMatrix m_tm;
    IVolume* m_localVolume3;
    IVolume* m_localVolume;
    IVolume* m_worldVolume3;
    IVolume* m_worldVolume;
    BSObject* m_script;
    unsigned char unknown2394[4];
    CVolSphere* m_sphere;
    unsigned char unknown239c[20];
    long m_yaw;
    unsigned char unknown23b4[4];
    CVector3 m_lastPosition;
    CVector3 m_velocity;
    unsigned char unknown23d8[20];
    long m_lastYaw;
    float m_turn;
    unsigned char unknown23f4[4];
    CVector3 m_bonePosition;
    CQuaternion m_boneRotation;
    CVector3 m_nextBonePosition;
    CQuaternion m_nextBoneRotation;
    unsigned char unknown2438[8];
    int m_value2440;
    unsigned char unknown2444[12];
    bool m_flag2450;
    unsigned char unknown2451[3];
    bool m_onGround;
    bool m_falling;
    unsigned char unknown2456[4];
    bool m_frozen;
    bool m_useFixedTM;
    unsigned char unknown245c[4];
    CMatrix m_fixedTM;
    unsigned char unknown24a0[24];
    CLightVolumeManager m_lightVolumes;
    unsigned char unknown24bc[3100];
    float m_value30d8;
    unsigned char unknown30dc[100];
    float m_value3140;
    float m_value3144;
    float m_value3148;
    int m_value314c;
    float m_value3150;
    int m_value3154;
    unsigned char unknown3158[4];
    int m_value315c;
    bool m_flag3160;
    bool m_flag3161;
    unsigned char unknown3162[14];
    CAnimMorph m_morph;
    unsigned char unknown3174[3660];
};

class CSoldierObject : public CAnimObject {
public:
    CSoldierObject();
    virtual ~CSoldierObject();
    virtual void Destroy();
    virtual void BeginUpdate(float);
    virtual void UpdateAI(float);
    virtual void AttemptUpdate(float);
    virtual void OnCollision(const CCollision&);
    virtual void CommitUpdate();
    virtual void Draw(CDrawContext&);
    virtual IVolume* GetLocalBoundingVolume(EVolumeType) const;
    virtual IVolume* GetWorldBoundingVolume(EVolumeType) const;
    virtual void HandleBulletCollision(CBullet*, const CCollision&);
    virtual CSoldierObject* AsSoldierObject();
    virtual const CSoldierObject* AsSoldierObject() const;
    virtual CAIDoodad* GetAIDoodad();
    virtual const CAIDoodad* GetAIDoodad() const;
    virtual void SetAttachedLight(CLight*, CVector3);
    virtual void Init();
    virtual void UpdateAnimation(float);
    virtual void UpdateLocalBoundingVolume(ISceneNode::EVolumeType, const CVector3&, const CVector3&);
    virtual void AttachObject(CStaticObject*, char*, int);
    virtual void DetachObject(char*);
    virtual void GetAttachPointMatrix(char*, CMatrix*);
    virtual void AttachFloatingBone();
    virtual void DetachFloatingBone();
    virtual void InitAI();

    void SetMood(EMoodType);

    bool m_flag3fc0;
    unsigned char unknown3fc1[3];
    int m_value3fc4;
    unsigned char unknown3fc8[72];
    CWeapon* m_weapons[25];
    int m_currentWeapon;
    unsigned char unknown4078[520];
    bool m_flag4280;
    unsigned char unknown4281[79];
    long m_morphTarget;
    long m_blendTargets[2];
    float m_blendWeights[2];
    float m_blendMax[2];
    float m_blendSpeeds[2];
    float m_cycleTimer;
    float m_cycleMin;
    float m_cycleMax;
    float m_holdLength;
    float m_fadeLength;
    float m_rampLength;
    unsigned char unknown430c[4];
    float m_rampEnd;
    float m_holdEnd;
    float m_fadeEnd;
    bool m_statePending;
    unsigned char unknown431d;
    unsigned short m_state;
    unsigned char unknown4320[128];
    int m_value43a0;
    int m_value43a4;
    int m_value43a8;
    float m_value43ac;
    bool m_flag43b0;
    unsigned char unknown43b1[11];
    float m_value43bc;
    unsigned char unknown43c0;
    bool m_flag43c1;
    unsigned char unknown43c2[2];
    int m_value43c4;
    int m_value43c8;
    unsigned char unknown43cc[104];
    CSoldierObject* m_self;
    float m_value4438;
    unsigned char unknown443c[4];
    CJawController m_jaw;
    unsigned char unknown4441[35];
    long m_extraTarget;
    long m_mouthTarget;
    unsigned char unknown446c[4];
    float m_mouthWeight;
    unsigned char unknown4474[12];
    int m_value4480;
    bool m_flag4484;
    unsigned char unknown4485[3];
    int m_value4488;
    int m_value448c;
    float m_value4490;
    int m_value4494;
    bool m_flag4498;
    unsigned char unknown4499[7];
    float m_vectorX;
    float m_vectorY;
    float m_vectorZ;
    unsigned char unknown44ac[4];
};

class CAnimatedPlayer : public CSoldierObject {
public:
    CAnimatedPlayer();
    virtual ~CAnimatedPlayer();
    virtual void BeginUpdate(float);
    virtual void UpdateAI(float) {}
    virtual void AttemptUpdate(float);
    virtual void CommitUpdate();
    virtual void Draw(CDrawContext&);
    virtual IVolume* GetLocalBoundingVolume(EVolumeType) const;
    virtual IVolume* GetWorldBoundingVolume(EVolumeType) const;
    virtual void HandleBulletCollision(CBullet*, const CCollision&);
    virtual CAnimatedPlayerObject* AsAnimatedPlayerObject() { return (CAnimatedPlayerObject*)this; }
    virtual const CAnimatedPlayerObject* AsAnimatedPlayerObject() const { return (const CAnimatedPlayerObject*)this; }
    virtual void Init();
    virtual void UpdateAnimation(float);
    virtual void UpdateLocalBoundingVolume(ISceneNode::EVolumeType, const CVector3&, const CVector3&);
    virtual void InitAI();

    void GetFlocoParams(float&, long&);
    void SetFlocoParams(float, long);
    void ResetState4498() {
        m_flag4498 = false;
        m_vectorX = m_vectorY = m_vectorZ = 0.0f;
    }
    void ResetState() {
        m_flag3fc0 = false;
        m_flag4280 = false;
        m_value43a0 = -1;
        m_value43a4 = 1;
        m_value43a8 = 40;
        m_value43ac = 0.75f;
        m_flag43b0 = false;
        m_value43bc = 0.0f;
        m_self = this;
        m_value4438 = 1.0f;
        m_flag43c1 = false;
        m_value43c4 = 0;
        m_value43c8 = 0;
        m_value315c = 1;
        m_value3150 = 0.0f;
        m_value3140 = 0.0f;
        m_value3144 = 0.0f;
        m_value3148 = 0.0f;
        m_value314c = 0;
        m_value3154 = 0;
        m_flag3160 = false;
        m_flag3161 = false;
        m_flag4484 = false;
        m_value4488 = 0;
        m_value448c = 0;
        m_value4490 = 0.0f;
        m_value4494 = 0;
        ResetState4498();
    }

    CPlayerObject* m_player;
    float m_flocoParam;
    long m_flocoCount;
};
CAnimatedPlayer::CAnimatedPlayer() {
    m_player = 0;
}

CAnimatedPlayer::~CAnimatedPlayer() {
}

void CAnimatedPlayer::Init() {
    CAnimObject::Init();
    memset(&m_weapons[9], 0, 32);
    memset(&m_weapons[17], 0, 32);
    memset(m_weapons, 0, 36);
    m_value3fc4 = 0;
    m_morph.Init((CAnimObject*)this, 0, (CharFrame_t*)m_frameData);
    m_morphTarget = -1;
    m_blendTargets[1] = -1;
    m_blendTargets[0] = -1;
    m_blendWeights[1] = 0.0f;
    m_blendWeights[0] = 0.0f;
    m_blendMax[1] = 0.0f;
    m_blendMax[0] = 0.0f;
    m_blendSpeeds[1] = 0.0f;
    m_blendSpeeds[0] = 0.0f;
    int roll = rand() % 100;
    if (roll < 25)
        m_value4480 = 13;
    else if (roll < 50)
        m_value4480 = 6;
    else if (roll < 75)
        m_value4480 = 7;
    else
        m_value4480 = 5;
    if (g_bInMultiplayerMode)
        ((CSoldierObject*)this)->SetMood((EMoodType)2);
    else
        ((CSoldierObject*)this)->SetMood((EMoodType)0);
    m_statePending = false;
    CVolSphere* sphere = (CVolSphere*)AllocateAnimatedVolume();
    sphere->SetRadius(1.2f);
    sphere->SetCenter(CVector3(0.0f, 0.0f, 0.0f));
    sphere->m_owner = this;
    m_sphere = sphere;
    m_localVolume = (IVolume*)sphere;
    m_worldVolume = (IVolume*)AllocateAnimatedVolume();
    CVolBox* box = (CVolBox*)AllocateBoxVolume();
    box->SetBasis(CVector3(1.0f, 0.0f, 0.0f), CVector3(0.0f, 1.0f, 0.0f), CVector3(0.0f, 0.0f, 1.0f));
    box->SetWidth(0.75f);
    box->SetHeight(1.6f);
    box->SetDepth(0.6f);
    box->SetCenter(CVector3(0.0f, 0.0f, 0.8f));
    m_localVolume3 = (IVolume*)box;
    m_worldVolume3 = (IVolume*)AllocateBoxVolume();
    SetCollisionId((EClsnId)28);
    m_currentWeapon = 0;
    ResetState4498();
    ResetState();
    ResetState();
    m_flocoParam = 0.0f;
    m_flocoCount = 0;
}

void CAnimatedPlayer::InitAI() {
}

void CAnimatedPlayer::HandleBulletCollision(CBullet* bullet, const CCollision& collision) {
    if (m_player == (CPlayerObject*)bullet->GetFiredBy()) {
        g_scene.ExcludeCollision(*(ISceneNode*)this, *(ISceneNode*)bullet, 5);
        bullet->Penetrate(collision);
        return;
    }
    if (bullet->AsProjectile()) {
        ContactView* contact = collision.m_contacts;
        CParticleSystem* blood = CParticleSystem::Create(0x6da45fe8);
        if (blood) {
            CMatrix matrix;
            matrix.Ident();
            CVector3 right;
            CVector3 front;
            if ((float)__fabs(contact->m_normal.Dot(CVector3(1.0f, 0.0f, 0.0f))) <= 0.9999f) {
                front.Cross(contact->m_normal, CVector3(1.0f, 0.0f, 0.0f));
                front.Normalize();
                right.Cross(front, contact->m_normal);
                right.Normalize();
            } else {
                right.Cross(contact->m_normal, CVector3(0.0f, 0.0f, 1.0f));
                right.Normalize();
                front.Cross(contact->m_normal, right);
                front.Normalize();
            }
            matrix.SetRight(right);
            matrix.SetFront(front);
            matrix.SetUp(contact->m_normal);
            matrix.SetPos(contact->m_position);
            blood->SetLocalToWorld(matrix);
        }
    }
    m_player->HandleBulletCollision(bullet, collision);
}

void CAnimatedPlayer::Draw(CDrawContext& context) {
    if (context.m_value114 != m_player->m_viewport)
        CSoldierObject::Draw(context);
}

void CAnimatedPlayer::UpdateAnimation(float dt) {
    CMatrix unused;
    CAnimObject::UpdateAnimation(dt);
    m_value30d8 = m_player->m_pitch;
    if (m_statePending) {
        m_statePending = false;
        if (m_morph.ValidState(m_state)) {
            unsigned short result;
            m_morph.SetAnimationState(m_state, 1.0f, result, -1, 0);
        }
    }
    m_morph.UpdateAnimation(dt);
    if (m_morphTarget != -1)
        m_morph.SetBlendTargetWeight(m_morphTarget + 28, 1.0f);
    if (m_blendTargets[0] >= 0) {
        int i;
        m_morph.SetBlendTargetWeight(m_blendTargets[0] + 11, m_blendWeights[0]);
        if (m_blendTargets[1] >= 0)
            m_morph.SetBlendTargetWeight(m_blendTargets[1] + 11, m_blendWeights[1]);
        if (0.0f == m_blendWeights[0] && m_blendTargets[1] >= 0) {
            m_blendTargets[0] = m_blendTargets[1];
            m_blendWeights[0] = m_blendWeights[1];
            m_blendSpeeds[0] = 0.0f;
            m_blendMax[0] = m_blendMax[1];
            m_blendTargets[1] = -1;
        }
        for (i = 0; i < 2; i++) {
            m_blendWeights[i] += m_blendSpeeds[i];
            if (m_blendWeights[i] < 0.0f) {
                m_blendWeights[i] = 0.0f;
                m_blendSpeeds[i] = 0.0f;
            } else if (m_blendWeights[i] > m_blendMax[i]) {
                m_blendWeights[i] = m_blendMax[i];
                m_blendSpeeds[i] = 0.0f;
            }
        }
    }
    m_cycleTimer -= dt / 59.94f;
    if (m_cycleTimer <= 0.0f) {
        m_rampEnd = m_rampLength;
        m_holdEnd = m_rampEnd + m_holdLength;
        m_fadeEnd = m_holdEnd + m_fadeLength;
        m_cycleTimer = m_fadeEnd + MathFunRandomReal(m_cycleMin, m_cycleMax);
    }
    float weight = 0.0f;
    if (m_cycleTimer >= m_rampEnd && m_cycleTimer <= m_holdEnd)
        weight = 1.0f;
    else if (m_cycleTimer > m_holdEnd && m_cycleTimer < m_fadeEnd)
        weight = 1.0f - (m_cycleTimer - m_holdEnd) / (m_fadeEnd - m_holdEnd);
    else if (m_cycleTimer < m_rampEnd)
        weight = m_cycleTimer / m_rampEnd;
    m_morph.SetBlendTargetWeight(13, weight);
    if (m_extraTarget >= 0)
        m_morph.SetBlendTargetWeight(m_extraTarget, 0.0f);
    m_jaw.Update((int)dt);
    m_morph.SetBlendTargetWeight(m_mouthTarget, m_mouthWeight);
}

void CAnimatedPlayer::SetFlocoParams(float param, long count) {
    m_flocoParam = param;
    m_flocoCount = count;
}

void CAnimatedPlayer::GetFlocoParams(float& param, long& count) {
    param = m_flocoParam;
    count = m_flocoCount;
}

IVolume* CAnimatedPlayer::GetLocalBoundingVolume(ISceneNode::EVolumeType type) const {
    switch (type) {
    case 3:
        return m_localVolume3;
    case 0:
    case 1:
    case 2:
        return m_localVolume;
    case 4:
        return 0;
    }
    return 0;
}

IVolume* CAnimatedPlayer::GetWorldBoundingVolume(ISceneNode::EVolumeType type) const {
    switch (type) {
    case 3:
        return m_worldVolume3;
    case 0:
    case 1:
    case 2:
        return m_worldVolume;
    case 4:
        return 0;
    }
    return 0;
}

void CAnimatedPlayer::UpdateLocalBoundingVolume(ISceneNode::EVolumeType type, const CVector3& min, const CVector3& max) {
    switch (type) {
    case 3:
        {
            CVolBox* box = (CVolBox*)m_localVolume3;
            box->SetCorners(min, max);
        }
        break;
    case 0:
        break;
    case 1:
        break;
    case 4:
        break;
    }
}

void CAnimatedPlayer::BeginUpdate(float dt) {
    m_lightVolumes.Update(dt);
    m_lastPosition = m_tm.GetPos();
    m_lastYaw = m_yaw;
    CMatrix bone(m_matrices[1]);
    m_boneRotation.SetFromMatrix(bone);
    m_bonePosition = bone.GetPos();
    UpdateAnimation(dt);
    m_velocity.x = m_velocity.y = m_velocity.z = 0.0f;
    m_flag2450 = false;
}

void CAnimatedPlayer::AttemptUpdate(float dt) {
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
    m_value2440 = 0;
    m_flag2450 = false;
    CWeapon* weapon = m_player->GetCurrentWeapon();
    if (m_player->m_dead) {
        m_weapons[m_currentWeapon]->muzzleFlash = 0;
        m_weapons[m_currentWeapon]->m_flag1e0 = 0;
    } else {
        m_weapons[m_currentWeapon]->muzzleFlash = weapon->GetMuzzleFlashStatus();
        if (weapon->m_value288 > 60.0f &&
            (m_weapons[m_currentWeapon]->m_type == 33 || m_weapons[m_currentWeapon]->m_type == 34))
            m_weapons[m_currentWeapon]->m_flag1e0 = 0;
        else
            m_weapons[m_currentWeapon]->m_flag1e0 = 1;
    }
}

/* ---- definitions ---- */
void CAnimatedPlayer::CommitUpdate() {
    ConstrainVelocity();
    CVector3 pos = m_tm.pos;
    m_lastPosition = pos;
    m_lastYaw = m_yaw;
    m_bonePosition = m_nextBonePosition;
    m_boneRotation = m_nextBoneRotation;
}
