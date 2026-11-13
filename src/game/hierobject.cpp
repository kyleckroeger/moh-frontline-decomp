// hierobject.cpp: CHierObject, the whole file, with its vtable and the RTTI records of its
// chain (IDestructible .. CHierObject): the weak copies of its inline Init, object type and
// casts; the bullet-damage ranges; motion playback, the light volume and SwapOut; Draw, the
// inherited matrix and the bounding volumes; ConstrainVelocity and OnCollision; the
// collision state and the animated motion; the script motion; CommitUpdate, AttemptUpdate
// and the linear velocity; the per-frame begin update (with the weak copies of
// CMatrix::PostMultiply and CStaticObject::IsFollowingPath) and CleanUp; the hierarchy tree,
// the attach-point matrix, Init, the destructor and the constructors.
//
// The names come from the mangled symbols; the class chain and the vtable from
// __RTTI__11CHierObject and __vt__11CHierObject (ISceneNode's and IMovingSceneNode's slots
// from the shared headers, CStaticObject's from +0x124 as __vt__13CStaticObject, then
// CHierObject's CleanUp, BeginScriptMotionUpdate and BeginAnimMotionUpdate).
//
// Inferred, not in any mangled name: the data members (named by offset or role; this file's
// CStaticObject is the shared declaration of src/game/attachobject.cpp with the motion,
// path, script-event and light-volume members this file uses named in its padding), the
// result types, the inline helpers (damage ranges, the attached list, ResetPathMotion,
// LerpTo) and the views of the mesh data, the box volume, the motion record, the volumes'
// virtual slots and the collision. The extent nodes are ISceneNode's data words at +0x0c..
// +0x18, reached through a view. The unit is compiled -inline auto,deferred, which emits the
// functions in reverse source order, so they are written last to first.
/* inferred: this file's CVector3 is the shared game/CVector3.h with the assignment through the
   double-pair union (both doublewords loaded before either is stored) */
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
    CVector3(float ax, float ay, float az) { x = ax; y = ay; z = az; }          // original
    CVector3(const CVector3& other) : CVector3Base(other) {}
    CVector3& operator=(const CVector3& other) {
        *(CVector3Data*)this = *(const CVector3Data*)&other;
        return *this;
    }                     // original

    void Set(float ax, float ay, float az) { x = ax; y = ay; z = az; }
    float LengthSq() const { return x * x + y * y + z * z; }
    float LengthSquared() const { return x * x + y * y + z * z; }
    float Length() const { return sqrtf(x * x + y * y + z * z); }               // original
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
    void Cross(const CVector3& a, const CVector3& b) {                         // original
        x = a.y * b.z - a.z * b.y;
        y = a.z * b.x - a.x * b.z;
        z = a.x * b.y - a.y * b.x;
    }
    void DotDotDot(const CVector3& a, const CVector3& b, const CVector3& c, const CVector3& v) {
        x = a.x * v.x + a.y * v.y + a.z * v.z;
        y = b.x * v.x + b.y * v.y + b.z * v.z;
        z = c.x * v.x + c.y * v.y + c.z * v.z;
    }
    void Abs(const CVector3& o) { x = __fabs(o.x); y = __fabs(o.y); z = __fabs(o.z); } // original
    CVector3& operator+=(const CVector3& o) { x += o.x; y += o.y; z += o.z; return *this; }
    CVector3& operator-=(const CVector3& o) { x -= o.x; y -= o.y; z -= o.z; return *this; }
    CVector3& operator*=(float f) { x *= f; y *= f; z *= f; return *this; }
    void Scale(const CVector3& o, float k) { x = k * o.x; y = k * o.y; z = k * o.z; } // original
    void Sub(const CVector3& a, const CVector3& b) { x = a.x - b.x; y = a.y - b.y; z = a.z - b.z; } // original
    void Add(const CVector3& o) { x += o.x; y += o.y; z += o.z; }              // original
    void Unitize() {
        float length = sqrtf(LengthSq());
        if (length != 0.0f) {
            x *= 1.0f / length;
            y *= 1.0f / length;
            z *= 1.0f / length;
        }
    }
    void Constrain(const CVector3&, float); // a real function (Constrain__8CVector3FRC8CVector3f)
} __attribute__((aligned(8)));


#include "game/CPlayerObject.h"
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

inline CVector3 operator*(float f, const CVector3& v) { return CVector3(f * v.x, f * v.y, f * v.z); }

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

class CHierObject;

class CCSGVolume {
public:
    CCSGVolume();
    ~CCSGVolume();
    void Init(int, CHierObject*);

    unsigned char unknown00[12];
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
    unsigned char unknown00[4];
    MeshDataView* data;
};

class CStaticAnimMesh {
public:
    int numFrames() const;
    int ticksPerFrame() const;
};

/* the motion record (key ticks, key count, transform flag, keys) */
struct MotionData {
    unsigned char unknown00[4];
    int ticks;
    int count;
    int transform;
    unsigned char keys[4];
};

/* a motion key (32 bytes): position and rotation */
struct MotionKey {
    CVector3 m_position;
    CQuaternion m_rotation;
};

/* a node of the scene's extent lists, its value at +8 */
struct ExtentNodeView {
    unsigned char unknown00[8];
    float m_value;
};

class CLightVolumeManager {
public:
    BPDLightVolume* GetVolume();

    unsigned char unknown00[4];
};

/* a collision contact: position, collision state, next contact */
struct ContactView {
    unsigned char unknown00[16];
    CVector3 m_position;
    unsigned char unknown20[4];
    int m_state;
    unsigned char unknown28[4];
    ContactView* m_next;
};

class CCollision {
public:
    ISceneNode* m_node0;
    ISceneNode* m_node1;
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

    bool IsFollowingPath() const {
        bool following = false;
        if (m_pathState != 0 && m_pathState != 1)
            following = true;
        return following;
    }
    void AnimUpdate(float);
    void BeginPathMotionUpdate(float);
    void AttemptCollisionDamage(const CCollision&);

    ExtentNodeView* Extent(int i) const { return ((ExtentNodeView* const*)&data0c)[i]; }

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

class CAttachableObject : public CStaticObject {
public:
    CAttachableObject(CStaticMesh*, int, int, int, int, int, CStaticObject::MotionFrame*);
    CAttachableObject();
    virtual ~CAttachableObject() {}
    virtual void BeginUpdate(float);
    virtual void UpdateAI(float);
    virtual void CommitAI();
    virtual void AttemptUpdate(float);
    virtual void CommitUpdate();
    virtual void Draw(CDrawContext&);
    virtual bool IsVisible(CDrawContext&) const;
    virtual int GetObjectType() const { return 1; }

    void UpdateAttachedObjects();

    ISceneNode* m_attached[10];
    unsigned char unknown2a8[8];
    CMatrix m_attachMatrix[10];
    int m_attachFlag[10];
    int m_attachScriptId[10];
};

class CStaticObjectFactory {
public:
    static void DeallocateStaticObject(CStaticObject*);
};

/* an attached scene node, constrained through its slot +32 */
class ISceneNodeView {
public:
    virtual void unknown008();
    virtual void unknown00c();
    virtual void unknown010();
    virtual void unknown014();
    virtual void unknown018();
    virtual void unknown01c();
    virtual void ConstrainVelocity();
};

/* an entry of the attached list */
struct AttachedEntry {
    AttachedEntry(ISceneNodeView* node) : m_node(node) {}
    AttachedEntry& operator=(AttachedEntry other) {
        m_node = other.m_node;
        return *this;
    }

    ISceneNodeView* m_node;
};

/* the attached list at +1680 (pointer, count, capacity), with 32-entry inline storage */
class CHierObjectBuffer {
public:
    CHierObjectBuffer() : m_data(0), m_unknown04(0), m_count(0), m_capacity(0), m_owned(true), m_unknown14(-1) {}
    ~CHierObjectBuffer() {
        if (m_owned && m_data)
            MEM_free(m_data);
    }

    ISceneNodeView** m_data;
    int m_unknown04;
    int m_count;
    int m_capacity;
    bool m_owned;
    int m_unknown14;
    unsigned char m_inline[128];
};

class CHierObject : public CAttachableObject {
public:
    CHierObject(CStaticMesh*, int, int, int, int, int, CStaticObject::MotionFrame*);
    CHierObject();
    virtual ~CHierObject();
    virtual void Init() { CStaticObject::Init(); }
    virtual int GetObjectType() const { return 8; }
    virtual CHierObject* AsHierObject() { return this; }
    virtual const CHierObject* AsHierObject() const { return this; }
    virtual void BeginUpdate(float);
    virtual void ConstrainVelocity();
    virtual void AttemptUpdate(float);
    virtual void OnCollision(const CCollision&);
    virtual void CommitUpdate();
    virtual void Draw(CDrawContext&);
    virtual IVolume* GetWorldBoundingVolume(EVolumeType) const;
    virtual CVector3 GetWorldLinearVelocity() const;
    virtual BPDLightVolume* GetLightVolume();
    virtual void StartMotionPlayback(int, int, EBSEventEnum);
    virtual void AttemptScriptMotionUpdate(float);
    virtual void AttemptAnimMotionUpdate(float);
    virtual void CommitScriptMotionUpdate();
    virtual void CommitAnimMotionUpdate();
    virtual void CleanUp();
    virtual void BeginScriptMotionUpdate(float);
    virtual void BeginAnimMotionUpdate(float);

    struct DamageRange {
        int minimum;
        int maximum;
        float multiplier;
    };

    float DamageMultiplier(int bulletClass) const {
        for (int i = 0; i < 2; i++) {
            if (bulletClass >= m_damageRanges[i].minimum && bulletClass <= m_damageRanges[i].maximum)
                return m_damageRanges[i].multiplier;
        }
        return 0.0f;
    }

    void AddDamageRange(int minimum, int maximum, float multiplier) {
        for (int i = 0; i < 2; i++) {
            if (m_damageRanges[i].minimum == -1) {
                m_damageRanges[i].minimum = minimum;
                m_damageRanges[i].maximum = maximum;
                m_damageRanges[i].multiplier = multiplier;
                return;
            }
        }
    }

    /* inferred: adds a node to the attached list while there is room */
    bool AttachedFull() const { return m_buffer.m_count >= m_buffer.m_capacity; }
    void StoreAttached(AttachedEntry entry) { ((AttachedEntry*)m_buffer.m_data)[m_buffer.m_count++] = entry; }
    void AddAttached(AttachedEntry entry) {
        if (!AttachedFull())
            StoreAttached(entry);
    }
    /* inferred inline helper: its temporary's stack slot needs it */
    void ResetPathMotion() {
        m_matrix.SetPos(m_pathPosition);
        m_speed = m_pathSpeed = 0.0f;
    }

    float GetBulletDamageMultiplierByClass(int, int);
    void RegisterBulletDamage(int, int, int, float);
    void SwapOut(CStaticMesh*, MotionData*);
    void SetInheritedMatrix(CMatrix&);
    IVolume* GetSubVolume(int) const;
    bool SetHierCollisionState(int);
    void DoBeginUpdate(float, bool);
    bool RemoveObjectFromTree(CHierObject*, int);
    void GetAttachPointMatrix(int, CMatrix&);
    CHierObject* GetSubObject(int);
    void SetRotation(CQuaternion&);
    void Init(CHierObject*, int, int, int, CVector3&);

    CHierObject* m_firstChild;
    CHierObject* m_nextSibling;
    bool m_paused : 1;
    bool m_independent : 1;
    bool m_isRoot : 1;
    bool m_active : 1;
    unsigned char m_flags588b : 4;
    unsigned char unknown589[3];
    int m_index;
    CHierObject* m_root;
    CCSGVolume m_volume;
    CHierObject* m_subObjects[20];
    CMatrix m_matrix5f0;
    CMatrix m_matrix630;
    CVector3 m_animPosition;
    CVector3 m_animDelta;
    CHierObjectBuffer m_buffer;
    DamageRange m_damageRanges[2];
};
CHierObject::CHierObject() {
}

CHierObject::CHierObject(CStaticMesh* mesh, int a, int b, int c, int d, int e, CStaticObject::MotionFrame* frame)
    : CAttachableObject(mesh, a, b, c, d, e, frame) {
}

CHierObject::~CHierObject() {
}

void CHierObject::Init(CHierObject* root, int index, int parentIndex, int attachPoint, CVector3& offset) {
    CStaticObject::Init();
    m_isRoot = this == root;
    m_root = root;
    m_firstChild = 0;
    m_nextSibling = 0;
    m_active = false;
    m_paused = false;
    if (m_isRoot) {
        for (int i = 0; i < 20; i++)
            m_subObjects[i] = 0;
    }
    m_matrix5f0.Ident();
    m_index = index;
    if (!m_isRoot) {
        CMatrix attach;
        root->m_subObjects[parentIndex]->GetAttachPointMatrix(attachPoint, attach);
        m_matrix630 = attach;
        root->m_subObjects[index] = this;
        CHierObject* parent = root->m_subObjects[parentIndex];
        if (parent->m_firstChild)
            m_nextSibling = parent->m_firstChild;
        parent->m_firstChild = this;
    } else {
        m_matrix630.Ident();
    }
    m_animPosition = offset;
    if (m_isRoot) {
        m_subObjects[0] = this;
        m_volume.Init(20, this);
    }
    if (m_buffer.m_owned && m_buffer.m_data)
        MEM_free(m_buffer.m_data);
    m_buffer.m_data = (ISceneNodeView**)m_buffer.m_inline;
    m_buffer.m_capacity = 32;
    m_buffer.m_owned = false;
    m_damageRanges[0].minimum = -1;
    m_damageRanges[1].minimum = -1;
}

void CHierObject::SetRotation(CQuaternion& rotation) {
    m_animRotation = rotation;
}

CHierObject* CHierObject::GetSubObject(int index) {
    return m_subObjects[index];
}

void CHierObject::GetAttachPointMatrix(int id, CMatrix& matrix) {
    MeshDataView* table = m_mesh->data;
    AttachPoint* point = table->points;
    for (int i = 0; i < table->count; i++, point++) {
        if (point->id == id) {
            matrix = point->matrix;
            return;
        }
    }
    DebugMsg("The specified attachpoint %d was not found\n", id);
}

bool CHierObject::RemoveObjectFromTree(CHierObject* object, int index) {
    if (m_isRoot)
        m_subObjects[index] = 0;
    if (object == m_firstChild) {
        m_firstChild = object->m_nextSibling;
        return true;
    }
    CHierObject* previous = 0;
    for (CHierObject* child = m_firstChild; child; previous = child, child = child->m_nextSibling) {
        if (child == object) {
            previous->m_nextSibling = child->m_nextSibling;
            return true;
        }
        if (child->RemoveObjectFromTree(object, index))
            return true;
    }
    return false;
}

void CHierObject::CleanUp() {
    CHierObject* child = m_firstChild;
    while (child) {
        CHierObject* next = child->m_nextSibling;
        child->CleanUp();
        child = next;
    }
    if (m_isRoot)
        m_subObjects[0] = 0;
    else
        m_root->RemoveObjectFromTree(this, m_index);
    FreeBoxVolume(m_worldVolume);
    FreeBoxVolume(m_localVolume);
    CStaticObjectFactory::DeallocateStaticObject(this);
}

void CHierObject::BeginUpdate(float time) {
    if (!m_paused)
        DoBeginUpdate(time, true);
}

void CHierObject::DoBeginUpdate(float time, bool child) {
    if (child && m_independent)
        return;
    AnimUpdate(time);
    if (IsFollowingPath()) {
        BeginPathMotionUpdate(time);
    } else if (m_scriptMove || m_scriptTurn || m_flag10) {
        BeginScriptMotionUpdate(time);
    } else if (m_flag04) {
        BeginAnimMotionUpdate(time);
    } else {
        m_animRotation.GetMatrix(m_matrix);
        m_matrix.SetPos(m_animPosition);
        m_matrix.PostMultiply(m_matrix630);
        m_matrix.PostMultiply(m_matrix5f0);
        if (m_worldVolume && m_localVolume)
            m_worldVolume->TransformedCopy(*m_localVolume, m_matrix);
    }
    UpdateAttachedObjects();
    for (CHierObject* child = m_firstChild; child; child = child->m_nextSibling) {
        child->SetInheritedMatrix(m_matrix);
        child->DoBeginUpdate(time, true);
    }
}

CVector3 CHierObject::GetWorldLinearVelocity() const {
    if (IsFollowingPath())
        return CVector3(m_speed * m_matrix.GetFront());
    return m_velocity;
}

void CHierObject::AttemptUpdate(float dt) {
    if (IsFollowingPath()) {
        AttemptPathMotionUpdate(dt);
    } else if (m_scriptMove || m_scriptTurn || m_flag10) {
        AttemptScriptMotionUpdate(dt);
    } else if (m_flag04) {
        AttemptAnimMotionUpdate(dt);
    } else {
        m_animRotation.GetMatrix(m_matrix);
        m_matrix.SetPos(m_animPosition);
        m_matrix.Multiply(m_matrix, m_matrix630);
        m_matrix.Multiply(m_matrix, m_matrix5f0);
    }
    for (CHierObject* child = m_firstChild; child; child = child->m_nextSibling) {
        child->m_matrix5f0 = m_matrix;
        child->AttemptUpdate(dt);
    }
    if (m_independent)
        g_scene.GetPlayer(0)->SetPlayersMountedMGPositions(&m_matrix);
    if (m_isRoot) {
        CVector3 min;
        CVector3 max;
        ((IVolume*)&m_volume)->GetExtents(min, max);
        ((ExtentNodeView*)data0c)->m_value = min.x;
        ((ExtentNodeView*)data14)->m_value = min.y;
        ((ExtentNodeView*)data10)->m_value = max.x;
        ((ExtentNodeView*)data18)->m_value = max.y;
    }
}

void CHierObject::CommitUpdate() {
    ConstrainVelocity();
    if (m_volume110 != m_worldVolume)
        m_volume110->TransformedCopy(*m_localVolume, m_matrix);
    if (IsFollowingPath()) {
        CommitPathMotionUpdate();
        m_animPosition = m_matrix.GetPos();
        m_animRotation.SetFromMatrix(m_matrix);
    } else if (m_scriptMove || m_scriptTurn || m_flag10) {
        CommitScriptMotionUpdate();
    } else if (m_flag04) {
        CommitAnimMotionUpdate();
    }
    for (CHierObject* child = m_firstChild; child; child = child->m_nextSibling)
        child->CommitUpdate();
    m_active = false;
}

void CHierObject::BeginScriptMotionUpdate(float) {
    if (m_scriptMove && m_moveTime <= 0.0f) {
        TriggerScriptEvent(m_moveEvent, (void*)m_index, false);
        m_moveTime = 0.0f;
        m_scriptMove = false;
        m_velocity.x = m_velocity.y = m_velocity.z = 0.0f;
    }
    if (m_scriptTurn) {
        m_animRotation.Normalize();
        if (m_turnTime <= 0.0f) {
            TriggerScriptEvent(m_turnEvent, (void*)m_index, false);
            m_turnTime = 0.0f;
            m_scriptTurn = false;
            m_scriptRotation.Set(0.0f, 0.0f, 0.0f, 1.0f);
        }
    }
}

void CHierObject::AttemptScriptMotionUpdate(float dt) {
    float moveStep = dt;
    float turnStep = dt;
    if (m_scriptTurn) {
        turnStep = m_turnTime < dt ? m_turnTime : dt;
        if (turnStep < 1.0f) {
            CQuaternion identity(0.0f, 0.0f, 0.0f, 1.0f);
            m_nextRotation.Slerp(m_scriptRotation, identity, turnStep);
        }
        else
            m_nextRotation = m_scriptRotation;
        m_nextRotation.Multiply(m_animRotation);
    } else {
        m_nextRotation = m_animRotation;
    }
    m_nextRotation.GetMatrix(m_matrix);
    if (m_scriptMove) {
        moveStep = m_moveTime < dt ? m_moveTime : dt;
        m_animDelta.x = moveStep * m_velocity.x;
        m_animDelta.y = moveStep * m_velocity.y;
        m_animDelta.z = moveStep * m_velocity.z;
        m_matrix.TransformPoint(m_animDelta, m_animDelta);
    } else {
        m_animDelta.x = m_animDelta.y = m_animDelta.z = 0.0f;
    }
    m_matrix.SetPos(m_animDelta);
    m_matrix.Translate(m_animPosition);
    m_matrix.Multiply(m_matrix, m_matrix630);
    m_matrix.Multiply(m_matrix, m_matrix5f0);
    m_moveStep = moveStep;
    m_turnStep = turnStep;
    if (m_worldVolume && m_localVolume)
        m_worldVolume->TransformedCopy(*m_localVolume, m_matrix);
}

void CHierObject::CommitScriptMotionUpdate() {
    if (m_scriptMove) {
        float zero = 0.0f;
        m_moveTime -= m_moveStep;
        m_moveTime = m_moveTime < zero ? zero : m_moveTime;
        m_animPosition += m_animDelta;
    }
    if (m_scriptTurn) {
        float zero = 0.0f;
        m_animRotation = m_nextRotation;
        m_turnTime -= m_turnStep;
        m_turnTime = m_turnTime < zero ? zero : m_turnTime;
    }
}

inline void LerpTo(CVector3& r, const CVector3& a, const CVector3& b, float t) {
    float fx, fy, fz;
    fx = a.x;
    fy = a.y;
    fz = a.z;
    r.x = fx + t * (b.x - fx);
    r.y = fy + t * (b.y - fy);
    r.z = fz + t * (b.z - fz);
}

void CHierObject::BeginAnimMotionUpdate(float dt) {
    if (m_keyCount && m_keyFrames) {
        m_position = m_animPosition;
        m_rotation = m_animRotation;
        m_keyTime += dt;
        while (m_keyTime >= (float)m_keyFrames) {
            m_keyTime -= (float)m_keyFrames;
            m_key++;
        }
        int next = m_key + 1;
        if (m_lastKey) {
            if (next >= m_lastKey)
                next = m_lastKey - 1;
            if (m_key >= m_lastKey) {
                if (m_keyEvent != -1) {
                    TriggerScriptEvent(m_keyEvent, 0, false);
                    m_keyEvent = -1;
                }
                m_key = m_lastKey - 1;
                m_flag04 = false;
            }
        } else {
            m_key %= m_keyCount;
            next %= m_keyCount;
        }
        CVector3 from = m_keys[m_key].m_position;
        CVector3 to = m_keys[next].m_position;
        float t = m_keyTime / (float)m_keyFrames;
        if (t < 0.0f)
            t = 0.0f;
        if (t > 1.0f)
            t = 1.0f;
        CVector3 pos;
        LerpTo(pos, from, to, t);
        CQuaternion current;
        current = m_keys[m_key].m_rotation;
        CQuaternion target;
        target = m_keys[next].m_rotation;
        CQuaternion rotation;
        rotation.Slerp(target, current, t);
        if (m_transformDelta) {
            m_keyRotation.Rotate(pos, pos);
            pos.x += m_keyOffset.x;
            pos.y += m_keyOffset.y;
            pos.z += m_keyOffset.z;
            rotation.Multiply(m_keyRotation);
        }
        m_velocity.x = pos.x - m_position.x;
        float rate = 1.0f / dt;
        m_velocity.y = pos.y - m_position.y;
        m_velocity.z = pos.z - m_position.z;
        m_targetRotation = rotation;
        m_velocity.x *= rate;
        m_velocity.y *= rate;
        m_velocity.z *= rate;
        m_rotationRate = rate;
    }
}

void CHierObject::AttemptAnimMotionUpdate(float dt) {
    if (m_keyCount && m_keyFrames) {
        m_animDelta.x = dt * m_velocity.x;
        m_animDelta.y = dt * m_velocity.y;
        m_animDelta.z = dt * m_velocity.z;
        CQuaternion rotation;
        rotation.Slerp(m_targetRotation, m_rotation, dt * m_rotationRate);
        m_nextRotation = rotation;
        rotation.GetMatrix(m_matrix);
        if (m_transformDelta)
            m_matrix.TransformPoint(m_animDelta, m_animDelta);
        m_matrix.SetPos(m_position);
        m_matrix.Translate(m_animDelta);
        m_matrix.Multiply(m_matrix, m_matrix630);
        m_matrix.Multiply(m_matrix, m_matrix5f0);
    }
    if (m_worldVolume && m_localVolume)
        m_worldVolume->TransformedCopy(*m_localVolume, m_matrix);
}

void CHierObject::CommitAnimMotionUpdate() {
    if (!m_keyCount)
        return;
    if (!m_keyFrames)
        return;
    m_animRotation = m_nextRotation;
    m_animPosition += m_animDelta;
    m_position = m_animPosition;
    m_rotation = m_animRotation;
}

bool CHierObject::SetHierCollisionState(int state) {
    if (state == m_index) {
        m_active = 1;
        return true;
    }
    for (CHierObject* child = m_firstChild; child; child = child->m_nextSibling) {
        if (child->SetHierCollisionState(state)) {
            m_active = 1;
            return true;
        }
    }
    return false;
}

void CHierObject::OnCollision(const CCollision& collision) {
    if (m_paused)
        return;
    ISceneNode* other = collision.m_node0 == (ISceneNode*)this ? collision.m_node1 : collision.m_node0;
    if (other->AsBullet())
        return;
    if (m_isRoot) {
        for (ContactView* contact = collision.m_contacts; contact; contact = contact->m_next)
            SetHierCollisionState(contact->m_state);
        AttemptCollisionDamage(collision);
    }
    if (m_active) {
        if (m_isRoot && other->AsPlayerObject()) {
            CVector3 contactPos = collision.m_contacts->m_position;
            CVector3 otherPos;
            other->GetPosition(otherPos);
            CVector3 push(contactPos.x - otherPos.x, contactPos.y - otherPos.y, contactPos.z - otherPos.z);
            push.Normalize();
            CVector3 normal;
            if (IsFollowingPath()) {
                const CVector3& pos = m_matrix.GetPos();
                normal.x = pos.x - m_pathPosition.x;
                normal.y = pos.y - m_pathPosition.y;
                normal.z = pos.z - m_pathPosition.z;
                normal.Normalize();
            } else if (m_scriptMove || m_scriptTurn || m_flag10) {
                CMatrix inverse;
                inverse.FastInverse(m_matrix);
                CVector3 local;
                inverse.TransformPoint(local, contactPos);
                m_animRotation.GetMatrix(inverse);
                inverse.SetPos(m_pathPosition);
                inverse.TransformPoint(local, local);
                normal.x = contactPos.x - local.x;
                normal.y = contactPos.y - local.y;
                normal.z = contactPos.z - local.z;
                normal.Normalize();
            }
            if (push.Dot(normal) < 0.0f) {
                m_moveStep = 0.0f;
                m_turnStep = 0.0f;
                m_nextRotation = m_animRotation;
                m_animDelta.x = m_animDelta.y = m_animDelta.z = 0.0f;
                if (IsFollowingPath()) {
                    ResetPathMotion();
                } else if (m_scriptMove || m_scriptTurn) {
                    m_animRotation.GetMatrix(m_matrix);
                    m_matrix.Translate(m_animPosition);
                    m_matrix.Multiply(m_matrix, m_matrix630);
                    m_matrix.Multiply(m_matrix, m_matrix5f0);
                } else if (m_flag04) {
                    m_velocity.x = m_velocity.y = m_velocity.z = 0.0f;
                    m_targetRotation = m_rotation;
                }
                for (CHierObject* child = m_firstChild; child; child = child->m_nextSibling) {
                    child->m_matrix5f0 = m_matrix;
                    child->OnCollision(collision);
                }
            }
            m_active = 0;
            return;
        }
        for (ContactView* contact = collision.m_contacts; contact; contact = contact->m_next)
            AddAttached(AttachedEntry((ISceneNodeView*)other->AsAnimObject()));
    }
    for (CHierObject* child = m_firstChild; child; child = child->m_nextSibling)
        child->OnCollision(collision);
}

void CHierObject::ConstrainVelocity() {
    if (m_active) {
        ISceneNodeView** end = m_buffer.m_data + m_buffer.m_count;
        for (ISceneNodeView** it = m_buffer.m_data; it != end; ++it) {
            if (*it)
                (*it)->ConstrainVelocity();
        }
        m_moveStep = 0.0f;
        m_turnStep = 0.0f;
        m_nextRotation = m_animRotation;
        m_animDelta.x = m_animDelta.y = m_animDelta.z = 0.0f;
        if (IsFollowingPath()) {
            ResetPathMotion();
        } else if (m_scriptMove || m_scriptTurn) {
            m_animRotation.GetMatrix(m_matrix);
            m_matrix.Translate(m_animPosition);
            m_matrix.Multiply(m_matrix, m_matrix630);
            m_matrix.Multiply(m_matrix, m_matrix5f0);
        } else if (m_flag04) {
            m_velocity.x = m_velocity.y = m_velocity.z = 0.0f;
            m_targetRotation = m_rotation;
        }
    }
    for (CHierObject* child = m_firstChild; child; child = child->m_nextSibling) {
        child->m_matrix5f0 = m_matrix;
        child->ConstrainVelocity();
    }
}

IVolume* CHierObject::GetSubVolume(int index) const {
    if (m_isRoot && index > 0) {
        if (m_subObjects[index])
            return m_subObjects[index]->GetSubVolume(index);
        return 0;
    }
    return m_worldVolume;
}

IVolume* CHierObject::GetWorldBoundingVolume(ISceneNode::EVolumeType type) const {
    switch (type) {
    case 3:
        if (m_isRoot)
            return (IVolume*)&m_volume;
        return m_worldVolume;
    case 0:
    case 1:
    case 2:
        if (m_isRoot)
            return (IVolume*)&m_volume;
        return m_worldVolume;
    case 4:
        return 0;
    }
    return 0;
}

void CHierObject::SetInheritedMatrix(CMatrix& matrix) {
    m_matrix5f0 = matrix;
}

void CHierObject::Draw(CDrawContext& context) {
    CStaticObject::Draw(context);
    for (CHierObject* child = m_firstChild; child; child = child->m_nextSibling)
        child->Draw(context);
}

void CHierObject::SwapOut(CStaticMesh* mesh, MotionData* motion) {
    m_mesh = mesh;
    if (motion) {
        CStaticAnimMesh* anim = (CStaticAnimMesh*)mesh;
        m_frameCount = anim->numFrames();
        m_ticksPerFrame = anim->ticksPerFrame();
        m_keyCount = motion->count;
        m_keyFrames = motion->ticks;
        m_transformDelta = motion->transform != 0;
        m_keys = (MotionKey*)motion->keys;
    } else {
        m_frameCount = 0;
        m_ticksPerFrame = 0;
        m_keyCount = 0;
        m_keyFrames = 0;
        m_transformDelta = 0;
        m_keys = 0;
    }
    CVolBox* source = m_mesh->data->box;
    CVector3 a(source->m_axes[0]);
    CVector3 b(source->m_axes[1]);
    CVector3 c(source->m_axes[2]);
    CVector3 center(source->m_center);
    CVolBox* box = (CVolBox*)m_localVolume;
    if (m_root->AsHierTankObject()) {
        center.x += 0.0f * a.Length();
        center.y += 0.1f * c.Length();
        center.z += -0.1f * b.Length();
        box->SetWidth(1.6f * a.Length());
        box->SetHeight(1.7f * c.Length());
        box->SetDepth(1.4f * b.Length());
    } else {
        box->SetWidth(2.0f * a.Length());
        box->SetHeight(2.0f * c.Length());
        box->SetDepth(2.0f * b.Length());
    }
    a.Normalize();
    b.Normalize();
    c.Normalize();
    box->SetBasis(a, b, c);
    box->SetCenter(center);
}

BPDLightVolume* CHierObject::GetLightVolume() {
    if (m_isRoot)
        return m_lightVolumes.GetVolume();
    return m_root->GetLightVolume();
}

void CHierObject::StartMotionPlayback(int frame, int end, EBSEventEnum event) {
    if (!m_keys)
        return;
    if (frame < 0)
        frame = 0;
    if (frame >= m_keyCount)
        frame = m_keyCount - 1;
    if (end < 0)
        end = 0;
    if (end >= m_keyCount)
        end = m_keyCount;
    m_startKey = frame;
    m_key = frame;
    m_lastKey = end;
    m_keyEvent = event;
    m_keyRotation = m_animRotation;
    m_keyOffset = m_animPosition;
    m_flag04 = true;
}

void CHierObject::RegisterBulletDamage(int part, int minimum, int maximum, float multiplier) {
    if (part > 0)
        m_subObjects[part]->AddDamageRange(minimum, maximum, multiplier);
    else
    for (int i = 0; i < 2; i++) {
        if (m_damageRanges[i].minimum == -1) {
            m_damageRanges[i].minimum = minimum;
            m_damageRanges[i].maximum = maximum;
            m_damageRanges[i].multiplier = multiplier;
            return;
        }
    }

}

/* ---- definitions ---- */
float CHierObject::GetBulletDamageMultiplierByClass(int part, int bulletClass) {
    if (part > 0)
        return m_subObjects[part]->DamageMultiplier(bulletClass);
    return DamageMultiplier(bulletClass);
}
