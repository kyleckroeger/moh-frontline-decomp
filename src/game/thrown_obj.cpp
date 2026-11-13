// thrown_obj.cpp: CThrownObject, the whole file, with its vtable and the RTTI records of its
// chain (IDestructible .. CThrownObject): the weak copies of its inline object type, cast and
// script-transform defaults; the deleted flag, Destroy, MarkForDestruction, SetVelocity,
// HandleBulletCollision and the update hooks; Draw and ConstrainVelocity; OnCollision and
// CleanUp; Init; the destructor and the two constructors.
//
// The names come from the mangled symbols; the class chain and the vtable from
// __RTTI__13CThrownObject and __vt__13CThrownObject (ISceneNode's and IMovingSceneNode's slots
// from the shared headers, CStaticObject's from +0x124 as __vt__13CStaticObject, then
// CThrownObject's CleanUp at +0x164).
//
// Inferred, not in any mangled name: the data members (named by offset or role; CStaticObject
// is hierobject.cpp's declaration, whose names the fragments' views map onto: the bounds
// entries are ISceneNode's data words at +0x0c..+0x18, the sphere and the colliders the local
// volume at +0x10c and the volumes at +0x110/+0x114, the saved position +0x120, the owner the
// collision id at +0x1dc), the result types, the inline helpers (bounds, saved state, the
// velocity clamp, the impact sound) and the views of the definition, the mesh, the draw
// context, the collision, the bullet (AsProjectile and GetVelocity at +0x140/+0x144) and the
// sphere volume. The unit is compiled -inline auto,deferred, which emits the functions in
// reverse source order, so they are written last to first.
/* inferred: this file's CVector3 holds its three floats in an 8-aligned record and declares no
   constructors, so implicit copies move it as two doublewords and the returned rows keep their
   temporaries (the shared header's constructors change both). It is declared before the shared
   CMatrix header (its include guard predefined); ISceneNode and IMovingSceneNode below are
   game/ISceneNode.h and game/IMovingSceneNode.h with the two bodies that name a vector's members
   or construct one written for this view (their guards predefined too) */
#define GAME_CVECTOR3_H
#define GAME_ISCENENODE_H
#define GAME_IMOVINGSCENENODE_H
#include "game/msl_sqrtf.h"

struct VECTOR3VIEW {
    float x;
    float y;
    float z;
} __attribute__((aligned(8)));

class CVector3 {
public:
    void Set(float ax, float ay, float az) { v.x = ax; v.y = ay; v.z = az; }
    float Dot(const CVector3& o) const { return v.x * o.v.x + v.y * o.v.y + v.z * o.v.z; }
    float LengthSq() const { return v.x * v.x + v.y * v.y + v.z * v.z; }
    float Length() const { return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z); }
    void Normalize() {
        float length = sqrtf(LengthSq());
        if (length != 0.0f) {
            v.x *= 1.0f / length;
            v.y *= 1.0f / length;
            v.z *= 1.0f / length;
        }
    }
    CVector3& operator*=(float f) {
        v.x *= f;
        v.y *= f;
        v.z *= f;
        return *this;
    }
    CVector3& operator+=(const CVector3& o) {
        v.x += o.v.x;
        v.y += o.v.y;
        v.z += o.v.z;
        return *this;
    }

    VECTOR3VIEW v;
};

#include "game/CMatrix.h"

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
    virtual void GetPosition(CVector3& position) const { position.v.x = position.v.y = position.v.z = 0.0f; }
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

class CThrownObject;
class CWeapon;
class CTankObject;
enum EBSEventEnum {};

extern CScene g_scene;
void DebugMsg(const char*, ...);
extern "C" void MEM_free(void*);
void FreeBoxVolume(void*);
void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);

/* inferred views */
struct BSObject {
    unsigned char unknown00[16];
    int id;
};

class CQuaternion {
public:
    CQuaternion() {}
    CQuaternion(float ax, float ay, float az, float aw) : w(aw), x(ax), y(ay), z(az) {}
    void Slerp(CQuaternion, CQuaternion, float);
    void Normalize();
    void Set(float, float, float, float);
    void Rotate(CVector3&, CVector3);
    void SetFromMatrix(const CMatrix&);
    void Multiply(CQuaternion);
    void GetMatrix(CMatrix&) const;

    float w;
    float x;
    float y;
    float z;
};


/* inferred: the volumes' virtual slots used here (TransformedCopy +0x10, GetExtents +0x48) */
class IVolume {
public:
    virtual ~IVolume();
    virtual IVolume* Create() const;
    virtual void TransformedCopy(const IVolume&, const CMatrix&);
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
    virtual void GetExtents(CVector3&, CVector3&) const;
};

class CVolBox {
public:
    void SetWidth(float);
    void SetHeight(float);
    void SetDepth(float);
    void SetBasis(CVector3, CVector3, CVector3);
    void SetCenter(CVector3);

    unsigned char unknown00[16];
    CVector3 m_axes[3];
    CVector3 m_center;
};

/* the mesh's attach points, 80 bytes each (a matrix, then the id at +64) */
struct AttachPoint {
    CMatrix matrix;
    int id;
    unsigned char unknown44[12];
};

/* the mesh data: its box (+32), attach points (+40) and their count */
struct MeshDataView {
    unsigned char unknown00[32];
    CVolBox* box;
    unsigned char unknown24[4];
    AttachPoint* points;
    int count;
};

class CStaticMesh {
public:
    virtual ~CStaticMesh();
    virtual void Draw(CDrawContext&, int);
    virtual void EnableLighting(bool);
    virtual bool IsLightingEnabled();
    static void SetLightVolume(BPDLightVolume*);

    MeshDataView* data;
};

/* a motion key (32 bytes): position and rotation */
struct MotionKey {
    CVector3 m_position;
    CQuaternion m_rotation;
};

class CLightVolumeManager {
public:
    void Update(float);
    BPDLightVolume* GetVolume();

    unsigned char unknown00[32];
};

/* a surface record, its impact sound index in the top 11 bits of the halfword at +12 */
struct SurfaceView {
    unsigned char unknown00[12];
    short m_sound : 11;
    short m_flags : 5;
};

/* a collision contact: normal, point, surface, next contact */
struct ContactView {
    CVector3 m_normal;
    CVector3 m_point;
    unsigned char unknown20[4];
    SurfaceView* m_surface;
    unsigned char unknown28[4];
    ContactView* m_next;
};

class CCollision {
public:
    ISceneNode* m_nodeA;
    ISceneNode* m_nodeB;
    unsigned char unknown08[16];
    ContactView* m_contacts;
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

    void SetLocalBoundingVolume(EVolumeType, IVolume*);
    void SetWorldBoundingVolume(EVolumeType, IVolume*);

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
    int m_drawMode;
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
};

class CVolSphere {
public:
    float GetRadius() const;
    void SetRadius(float);
    void SetCenter(CVector3);
    CVolSphere& operator=(const CVolSphere&);
};

void* AllocateSphereVolume();
void FreeSphereVolume(void*);

class CProjectileBullet;
class CThrownObject;

/* the bullet: AsProjectile and GetVelocity at +0x140/+0x144 */
class CBullet : public IMovingSceneNode {
public:
    virtual void unknown124();
    virtual void unknown128();
    virtual void unknown12c();
    virtual void unknown130();
    virtual void unknown134();
    virtual void unknown138();
    virtual void unknown13c();
    virtual CProjectileBullet* AsProjectile();
    virtual void GetVelocity(CVector3&);
};

/* the thrown-object definition's properties (friction, restitution, roll speed, the record's
   lifetime and flag, the impulse scale) */
struct ThrownPropertiesView {
    unsigned char unknown00[52];
    float friction;
    unsigned char unknown38[4];
    float restitution;
    unsigned char unknown40[4];
    float minRollSpeed;
    unsigned char unknown48[4];
    int m_value4c;
    unsigned char unknown50[12];
    int m_value5c;
    unsigned char unknown60[4];
    float impulseScale;
};

struct LevelFileContentsStruct_ {
    unsigned char unknown00[8];
    ThrownPropertiesView* properties;
};

/* the draw context's current-matrix pointer at +192 */
struct DRAWCONTEXTVIEW {
    unsigned char unknown000[192];
    CMatrix* matrix;
};

/* an entry of a sorted bounds list, keyed by the float at +8 */
struct BOUNDVIEW {
    unsigned char unknown0[8];
    float key;
};

class CSurfaceType {
public:
    static int GetSoundIndex(int);
};

extern const float CONTACT_EPSILON;
void PlayImpactSound(unsigned short, unsigned short, CVector3, bool);
void DeleteSpecialThrownObject(CThrownObject*);

class BSGO_Basic {
public:
    int m_field0;

private:
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

void ForceUpdateProximityTriggerStatus(BSGO_Basic*, bool);

class CThrownObject : public CStaticObject {
public:
    CThrownObject(CStaticMesh*, int, int, int, int, int, CStaticObject::MotionFrame*, LevelFileContentsStruct_*);
    CThrownObject();
    virtual void MarkForDestruction(int);
    virtual ~CThrownObject();
    virtual void Destroy();
    virtual void BeginUpdate(float);
    virtual void ConstrainVelocity();
    virtual void AttemptUpdate(float);
    virtual void OnCollision(const CCollision&);
    virtual void CommitUpdate();
    virtual void Draw(CDrawContext&);
    virtual void HandleBulletCollision(CBullet*, const CCollision&);
    virtual void Init();
    virtual int GetObjectType() const { return 4; }
    virtual CThrownObject* AsThrownObject() { return this; }
    virtual void TranslateFromScript(CVector3&, CVector3&, EBSEventEnum) {}
    virtual void RotateFromScript(CVector3&, CVector3&, EBSEventEnum) {}
    virtual void ScaleFromScript(float, float, EBSEventEnum) {}
    virtual void CleanUp();

    void SetDeleted(bool);
    void SetVelocity(CVector3&);
    void SetBounds(const CVector3& low, const CVector3& high) {
        ((BOUNDVIEW*)data0c)->key = low.v.x;
        ((BOUNDVIEW*)data14)->key = low.v.y;
        ((BOUNDVIEW*)data10)->key = high.v.x;
        ((BOUNDVIEW*)data18)->key = high.v.y;
    }
    CVector3 PositionRow() const { return m_matrix.pos; }
    void SaveState() {
        m_position = PositionRow();
        m_lastVelocity = m_velocity;
    }
    void ClampVelocity() {
        CVolSphere* sphere = (CVolSphere*)m_localVolume;
        if (m_collisionId != -1) {
            float speed = sqrtf(m_velocity.v.x * m_velocity.v.x + m_velocity.v.y * m_velocity.v.y + m_velocity.v.z * m_velocity.v.z);
            float limit = 2.0f * sphere->GetRadius();
            if (limit < speed) {
                float scale = limit / speed;
                m_velocity.v.x *= scale;
                m_velocity.v.y *= scale;
                m_velocity.v.z *= scale;
            }
        }
    }
    void PickImpactSound(ISceneNode* other, const ContactView* contact) {
        if (m_pendingSound == 0 && other->AsWorldObject()) {
            if (contact->m_surface && contact->m_surface->m_sound >= 0)
                m_pendingSound = CSurfaceType::GetSoundIndex(contact->m_surface->m_sound);
            else
                m_pendingSound = 1;
        }
    }

    float m_lifetime;
    unsigned char unknown284[4];
    CVector3 m_lastVelocity;
    CVector3 m_push;
    CVector3 m_step;
    CQuaternion m_orientation;
    LevelFileContentsStruct_* m_definition;
    int m_pendingSound;
    BSGO_Dummy m_scriptObject;
    CThrownObject* m_self;
    int m_value2e4;
    int m_value2e8;
    int m_value2ec;
    int m_value2f0;
    unsigned char flag0 : 1;
    unsigned char flag1 : 1;
    unsigned char flag2 : 1;
    unsigned char flag3 : 1;
    unsigned char deleted : 1;
    unsigned char markedForDestruction : 1;
    unsigned char flag6 : 1;
    unsigned char flag7 : 1;
    unsigned char unknown2f5[3];
    int m_state;
    int m_impactSound;
};
CThrownObject::CThrownObject() {
    deleted = 0;
    markedForDestruction = 0;
}

CThrownObject::CThrownObject(CStaticMesh* mesh, int a, int b, int c, int d, int e, CStaticObject::MotionFrame* frames,
                             LevelFileContentsStruct_* definition)
    : CStaticObject(mesh, a, b, c, d, e, frames) {
    m_definition = definition;
    deleted = 0;
    markedForDestruction = 0;
    m_orientation.x = 0.0f;
    m_orientation.y = 0.0f;
    m_orientation.z = 0.0f;
    m_orientation.w = 1.0f;
}

CThrownObject::~CThrownObject() {
}

void CThrownObject::Init() {
    CStaticObject::Init();
    m_lifetime = m_definition->properties->m_value5c;
    flag3 = m_definition->properties->m_value4c != 0;
    m_lastVelocity.v.z = 0.0f;
    m_lastVelocity.v.y = 0.0f;
    m_lastVelocity.v.x = 0.0f;
    m_step.v.z = 0.0f;
    m_step.v.y = 0.0f;
    m_step.v.x = 0.0f;
    flag0 = 1;
    flag1 = 0;
    flag2 = 0;
    deleted = 0;
    m_impactSound = 0;
    markedForDestruction = 0;
    m_state = -1;
    CVolBox* frame = m_mesh->data->box;
    CVector3 right = frame->m_axes[0];
    CVector3 front = frame->m_axes[1];
    CVector3 up = frame->m_axes[2];
    float a = right.Length();
    float b = front.Length();
    float c = up.Length();
    float radius = a > b ? a : b;
    radius = radius > c ? radius : c;
    CVolSphere* local = (CVolSphere*)AllocateSphereVolume();
    local->SetRadius(radius);
    local->SetCenter(m_mesh->data->box->m_center);
    SetLocalBoundingVolume((EVolumeType)3, (IVolume*)local);
    SetLocalBoundingVolume((EVolumeType)0, (IVolume*)local);
    CVolSphere* world = (CVolSphere*)AllocateSphereVolume();
    *world = *local;
    SetWorldBoundingVolume((EVolumeType)3, (IVolume*)world);
    SetWorldBoundingVolume((EVolumeType)0, (IVolume*)world);
    m_value2e8 = 0;
    m_value2e4 = 0;
    m_value2f0 = 0;
    m_value2ec = 0;
    m_scriptObject.m_field0 = 0;
    m_self = this;
    m_pendingSound = 0;
}

void CThrownObject::CleanUp() {
    void* local = GetLocalBoundingVolume((EVolumeType)3);
    void* world;
    if (local)
        FreeSphereVolume(local);
    world = GetWorldBoundingVolume((EVolumeType)3);
    if (world && world != local)
        FreeSphereVolume(world);
    m_localVolume = 0;
    m_worldVolume = 0;
}

void CThrownObject::OnCollision(const CCollision& collision) {
    bool first = collision.m_nodeA == this;
    ISceneNode* other;
    if (collision.m_nodeA == this)
        other = collision.m_nodeB;
    else
        other = collision.m_nodeA;
    if (m_state == -1) {
        if (!other->AsBullet()) {
            for (ContactView* contact = collision.m_contacts; contact; contact = contact->m_next) {
                CVector3 normal;
                if (first) {
                    normal.v.x = -contact->m_normal.v.x;
                    normal.v.y = -contact->m_normal.v.y;
                    normal.v.z = -contact->m_normal.v.z;
                } else
                    normal = contact->m_normal;
                if (!(m_velocity.v.x * normal.v.x + m_velocity.v.y * normal.v.y + m_velocity.v.z * normal.v.z >= 0.0f)) {
                    m_push.v.x += normal.v.x;
                    m_push.v.y += normal.v.y;
                    m_push.v.z += normal.v.z;
                    flag2 = 1;
                }
                if (contact->m_normal.v.z > 0.7f) {
                    flag1 = 1;
                    PickImpactSound(other, contact);
                }
            }
        }
    } else if (m_state != 0) {
        if (m_state == 1 && (other->AsAnimObject() || other->AsPlayerObject())) {
            BSObjectTriggerEvent(GetScriptObject(), 231, other->GetScriptObject(), 0, false);
            g_scene.ExcludeCollision(*this, *other, -1);
        } else {
            for (ContactView* contact = collision.m_contacts; contact; contact = contact->m_next) {
                if (contact->m_normal.v.z > 0.7f) {
                    flag1 = 1;
                    if (m_state != 2) {
                        BSObjectTriggerEvent(GetScriptObject(), 232, 0, 0, false);
                        m_state = 2;
                    }
                    {
                        CVector3 zero;
                        zero.Set(0.0f, 0.0f, 0.0f);
                        m_velocity = zero;
                    }
                    PickImpactSound(other, contact);
                }
            }
        }
    }
}

void CThrownObject::ConstrainVelocity() {
    if (flag2) {
        CVector3 normal;
        CVector3 normalPart;
        CVector3 pos;
        float dot;
        float d;
        float tx, ty, tz;
        m_push.Normalize();
        normal = m_push;
        dot = m_velocity.Dot(normal);
        if (dot < 0.0f) {
            normal *= 2.0f * dot;
            m_velocity.v.x -= normal.v.x;
            m_velocity.v.y -= normal.v.y;
            m_velocity.v.z -= normal.v.z;
        } else {
            flag2 = 0;
            return;
        }
        d = m_push.Dot(m_velocity);
        normalPart.v.x = d * m_push.v.x;
        normalPart.v.y = d * m_push.v.y;
        normalPart.v.z = d * m_push.v.z;
        tx = m_velocity.v.x - normalPart.v.x;
        ty = m_velocity.v.y - normalPart.v.y;
        tz = m_velocity.v.z - normalPart.v.z;
        if (!m_impactSound || flag1)
            normalPart *= m_definition->properties->restitution;
        if (normalPart.LengthSq() < 0.0001f) {
            normalPart.v.z = 0.0f;
            normalPart.v.y = 0.0f;
            normalPart.v.x = 0.0f;
        }
        if (flag1) {
            float f = 1.0f - m_definition->properties->friction;
            tx *= f;
            ty *= f;
            tz *= f;
        } else if (!m_impactSound) {
            float f = 1.0f - m_definition->properties->friction;
            f *= f;
            f *= f;
            f *= f;
            tx *= f;
            ty *= f;
            tz *= f;
        }
        normalPart.v.x += tx;
        normalPart.v.y += ty;
        normalPart.v.z += tz;
        m_velocity = normalPart;
        if (!flag1) {
            CVector3 row = PositionRow();
            CVector3 offset;
            float e = 2.0f * CONTACT_EPSILON;
            offset.v.x = e * m_push.v.x;
            offset.v.y = e * m_push.v.y;
            offset.v.z = e * m_push.v.z;
            pos.v.x = row.v.x + offset.v.x;
            pos.v.y = row.v.y + offset.v.y;
            pos.v.z = row.v.z + offset.v.z;
            m_matrix.SetPos(pos);
        }
    }
}

void CThrownObject::Draw(CDrawContext& context) {
    CMatrix rotation;
    float minSpeed = m_definition->properties->minRollSpeed;
    if (sqrtf(m_velocity.LengthSq()) > minSpeed) {
        CVector3 up;
        CVector3 axis;
        CVolSphere* sphere = (CVolSphere*)m_localVolume;
        float radius;
        CQuaternion turn;
        up.v.x = 0.0f;
        up.v.y = 0.0f;
        up.v.z = 1.0f;
        axis.v.x = up.v.y * m_velocity.v.z - up.v.z * m_velocity.v.y;
        axis.v.y = up.v.z * m_velocity.v.x - up.v.x * m_velocity.v.z;
        axis.v.z = up.v.x * m_velocity.v.y - up.v.y * m_velocity.v.x;
        axis.Normalize();
        if (sphere)
            radius = sphere->GetRadius();
        else
            radius = 1.0f;
        rotation.BuildRot(axis, sqrtf(m_step.LengthSq()) / radius);
        turn.SetFromMatrix(rotation);
        m_orientation.Multiply(turn);
    }
    m_orientation.GetMatrix(rotation);
    rotation.Multiply(rotation, m_matrix);
    ((DRAWCONTEXTVIEW&)context).matrix = &rotation;
    if (m_mesh->IsLightingEnabled())
        CStaticMesh::SetLightVolume(m_lightVolumes.GetVolume());
    m_mesh->Draw(context, m_drawMode);
}

void CThrownObject::BeginUpdate(float dt) {
    m_lightVolumes.Update(dt);
    m_position = PositionRow();
    m_lastVelocity = m_velocity;
    ForceUpdateProximityTriggerStatus(&m_scriptObject, true);
    if (m_pendingSound > 0) {
        if (m_impactSound == 1)
            PlayImpactSound(m_pendingSound, 4, m_position, false);
        m_pendingSound = -1;
    }
    if (m_lifetime > 0.0f && m_state) {
        m_lifetime -= (int)dt;
        if (m_lifetime < 0.0f)
            m_lifetime = 0.0f;
    }
}

void CThrownObject::AttemptUpdate(float dt) {
    if (!deleted) {
        CVector3 pos;
        CVector3 low;
        CVector3 high;
        CVector3 position;
        m_push.v.z = 0.0f;
        m_push.v.y = 0.0f;
        m_push.v.x = 0.0f;
        if (flag0 && m_state)
            m_velocity.v.z = -0.00545535097f * dt + m_lastVelocity.v.z;
        ClampVelocity();
        m_step.v.x = dt * m_velocity.v.x;
        m_step.v.y = dt * m_velocity.v.y;
        m_step.v.z = dt * m_velocity.v.z;
        pos.v.x = m_position.v.x + m_step.v.x;
        pos.v.y = m_position.v.y + m_step.v.y;
        pos.v.z = m_position.v.z + m_step.v.z;
        m_matrix.SetPos(pos);
        if (m_worldVolume) {
            m_worldVolume->TransformedCopy(*m_localVolume, m_matrix);
            m_worldVolume->GetExtents(low, high);
            SetBounds(low, high);
        } else {
            GetPosition(position);
            SetBounds(position, position);
        }
    }
}

void CThrownObject::CommitUpdate() {
    if (!deleted) {
        if (m_lifetime > 0.0f) {
            if (m_volume110 != m_worldVolume && m_volume110 && m_localVolume)
                m_volume110->TransformedCopy(*m_localVolume, m_matrix);
            ConstrainVelocity();
            SaveState();
            if (flag1)
                flag0 = 0;
            else
                flag0 = 1;
            flag1 = 0;
            flag2 = 0;
        } else {
            MarkForDestruction(0);
        }
    }
}

void CThrownObject::HandleBulletCollision(CBullet* bullet, const CCollision& collision) {
    if (bullet->AsProjectile() && m_state == -1) {
        CVector3 impulse;
        ContactView* hit;
        bullet->GetVelocity(impulse);
        impulse *= m_definition->properties->impulseScale;
        m_velocity += impulse;
        SetVelocity(m_velocity);
        hit = collision.m_contacts;
        if (m_impactSound == 1)
            PlayImpactSound(15, 1, hit->m_point, false);
    }
}

void CThrownObject::SetVelocity(CVector3& velocity) {
    m_velocity = velocity;
    if (m_collisionId != -1) {
        CVolSphere* sphere = (CVolSphere*)m_localVolume;
        float speed = sqrtf(m_velocity.v.x * m_velocity.v.x + m_velocity.v.y * m_velocity.v.y + m_velocity.v.z * m_velocity.v.z);
        float limit = 2.0f * sphere->GetRadius();
        if (limit < speed) {
            float scale = limit / speed;
            m_velocity.v.x *= scale;
            m_velocity.v.y *= scale;
            m_velocity.v.z *= scale;
        }
    }
    m_lastVelocity = m_velocity;
}

void CThrownObject::MarkForDestruction(int when) {
    if (!markedForDestruction) {
        CStaticObject::MarkForDestruction(when);
        markedForDestruction = 1;
    }
}

void CThrownObject::Destroy() {
    markedForDestruction = 0;
    DeleteSpecialThrownObject(this);
}

/* ---- definitions ---- */
void CThrownObject::SetDeleted(bool deletedFlag) {
    deleted = deletedFlag;
}
