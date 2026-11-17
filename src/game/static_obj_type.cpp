// A fragment of static_obj.cpp (0x800ae804): CStaticObject::GetObjectType (a weak copy of
// a header inline returning 0, so it is defined __declspec(weak)), Destroy (the script owner
// destroys the object when there is one; a dumped object goes back to the dump list, others
// to the static-object factory) and MarkForDestruction (ISubject's, then the object leaves
// g_scene and its script owner forgets its index). MarkForDestruction is the class's key
// function, so this unit compiles with RTTI on and also owns static_obj.cpp's CStaticObject
// virtual table and the RTTI records of its chain (IDestructible .. CStaticObject), as the
// original file did, while the rest of the file stays in its other fragments (a stepping
// stone, accepted 2026-11-13: docs/Remaining.md).
//
// The names come from the mangled symbols; the class chain and the table from
// __RTTI__13CStaticObject and __vt__13CStaticObject (ISceneNode's and IMovingSceneNode's slots
// from the shared headers). CStaticObject is hierobject.cpp's declaration (the shared one of
// attachobject.cpp), with the script object seen as its owner view and the dumped flag named in
// its padding; the members and views are inferred. The compiled copies of the inline virtuals
// are weak or file duplicates, linked to the original copies.
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
/* inferred: the object's script owner, its table at +12 */
class StaticOwnerView {
public:
    unsigned char unknown00[12];

    virtual void Destroy();

    unsigned char unknown10[4];
    int m_index;
};

/* inferred: the script object as this file sees it: its owner at +12, the owner's index at +16 */
struct BSObject {
    unsigned char unknown00[12];
    StaticOwnerView* m_owner;
    int m_index;
};

#include "game/CQuaternion.h"


inline CVector3 operator*(float f, const CVector3& v) { return CVector3(f * v.x, f * v.y, f * v.z); }

/* inferred: the volumes' virtual slots used here (TransformedCopy +0x10, GetExtents +0x48) */
#include "game/IVolume.h"


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
    unsigned char m_flags1e1a : 1;
    bool m_dumped : 1;
    unsigned char m_flags1e1b : 6;
    unsigned char unknown1e2[22];
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


void DestroyDumpObject(CStaticObject*);

class CStaticObjectFactory {
public:
    static void DeallocateStaticObject(CStaticObject*);
};

__declspec(weak) int CStaticObject::GetObjectType() const {
    return 0;
}

void CStaticObject::Destroy() {
    if (m_script && m_script->m_owner)
        m_script->m_owner->Destroy();
    else if (m_dumped)
        DestroyDumpObject(this);
    else
        CStaticObjectFactory::DeallocateStaticObject(this);
}

void CStaticObject::MarkForDestruction(int flag) {
    ISubject::MarkForDestruction(flag);
    g_scene.Remove(*this);
    if (m_script)
        m_script->m_index = -1;
}
