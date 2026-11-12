// A fragment of bsbifunc.cpp (0x8002e874): script built-ins that register the
// script object with the distance-to-target schedule when the calling node's AI
// object has a target (a stored target position or a target object; the near
// and far distances squared), otherwise trigger script event 69, returning the
// registration (0 when none); destroy a thrown object (MarkForDestruction(2));
// drop a falling object (a flag of the thrown object, without popping
// arguments); and create a child falling object and a child thrown object (a
// special thrown object for the script's trigger, placed at the calling node's
// transform or else at the trigger's position and rotation; the falling object
// gets the drop flag clear and zero velocity, the thrown one the normalized
// direction argument scaled by the speed argument; then a first update and a
// collision id chosen by the type argument). Between those two is the weak
// CStaticObject::SetCollisionId (the id at +476), emitted after its first use.
// Each reads its arguments below the script stack top. The file name is this
// project's; the original record is bsbifunc.cpp and the built-ins around
// these are not reconstructed. The functions, classes and globals are
// named by the mangled symbols; ISceneNode is declared with its virtual
// functions in the order of __vt__10ISceneNode (GetAIDoodad at +204),
// IMovingSceneNode's and CStaticObject's after them in the order of
// __vt__13CStaticObject (AsThrownObject at +320), CAIFilterObject's in the
// order of its table; the doodad, filter, AI object, schedule (BSSchedule's
// size is not known), thrown-object, trigger, script-object and built-in
// record views, the helpers, CVector3's inline Normalize and scaling (sqrtf
// is the SDK's inline) and the collision-id table's meaning are inferred.
#include <math.h>

#include "game/CVector3.h"


class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    void SetPos(CVector3);

    float m[4][4];
    static bool s_ClassInit;
} __attribute__((aligned(16)));

class CQuaternion {
public:
    void GetMatrix(CMatrix&) const;

    float x;
    float y;
    float z;
    float w;
};

enum EClsnId {};
class CCollision;
class CDrawContext;
class CMatrix;
class CBullet;
class CLight;
class CPlayerObject;
class CStaticObject;
class CThrownObject;
struct AIDoodadView;

class ISceneNode {
public:
    enum EVolumeType {};
    virtual void MarkForDestruction(int);
    virtual ~ISceneNode();
    virtual void Destroy();
    virtual void BeginUpdate(float);
    virtual void UpdateAI(float);
    virtual void CommitAI();
    virtual void ConstrainVelocity();
    virtual void AttemptUpdate(float);
    virtual void OnCollision(const CCollision&);
    virtual void CommitUpdate();
    virtual void Draw(CDrawContext&);
    virtual int GetLocalBoundingVolume(EVolumeType) const;
    virtual int GetWorldBoundingVolume(EVolumeType) const;
    virtual void GetTMLocalToWorld(CMatrix&) const;
    virtual void GetPosition(CVector3&) const;
    virtual void GetRightward(CVector3&) const;
    virtual void GetForward(CVector3&) const;
    virtual void GetUpward(CVector3&) const;
    virtual int IsVisible(CDrawContext&) const;
    virtual int IsDrawEnabled() const;
    virtual EClsnId GetCollisionId() const;
    virtual void SetCollisionId(EClsnId);
    virtual int GetScriptObject() const;
    virtual void TriggerScriptEvent(int, void*, bool);
    virtual void HandleBulletCollision(CBullet*, const CCollision&);
    virtual void* AsMovingNode();
    virtual const void* AsMovingNode() const;
    virtual CStaticObject* AsStaticObject();
    virtual const void* AsStaticObject() const;
    virtual void* AsHierObject();
    virtual const void* AsHierObject() const;
    virtual void* AsWorldObject();
    virtual const void* AsWorldObject() const;
    virtual void* AsAnimObject();
    virtual const void* AsAnimObject() const;
    virtual void* AsSoldierObject();
    virtual const void* AsSoldierObject() const;
    virtual CPlayerObject* AsPlayerObject();
    virtual const void* AsPlayerObject() const;
    virtual void* AsPlayerWeaponObject();
    virtual const void* AsPlayerWeaponObject() const;
    virtual void* AsAnimatedPlayerObject();
    virtual const void* AsAnimatedPlayerObject() const;
    virtual CLight* AsLight();
    virtual const CLight* AsLight() const;
    virtual CBullet* AsBullet();
    virtual const CBullet* AsBullet() const;
    virtual void* AsCollisionVolume();
    virtual const void* AsCollisionVolume() const;
    virtual AIDoodadView* GetAIDoodad();
    virtual const void* GetAIDoodad() const;
    virtual void SetAttachedLight(CLight*, CVector3);
    virtual CLight* GetAttachedLight() const;
};


class IMovingSceneNode : public ISceneNode {
public:
    virtual ~IMovingSceneNode();
    virtual void Reset();
    virtual void Halt();
    virtual void ApplyForceTo(CVector3, float);
    virtual void SetTMLocalToWorld(const CMatrix&);
};

struct TriggerCoreView {
    unsigned char unknown00[16];
    float x;
    float y;
    float z;
    CQuaternion rotation;
    unsigned char unknown2c[28];
    int crc;
};

struct TriggerObject_struct {
    unsigned char unknown00[4];
    TriggerCoreView* core;
};

class BSObject {
public:
    unsigned char unknown00[8];
    TriggerObject_struct* trigger;
};
class BPDLightVolume;
enum EBSEventEnum {};

class CStaticObject : public IMovingSceneNode {
public:
    virtual ~CStaticObject();
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
    virtual int GetWorldLinearVelocity() const;
    virtual void EnterLightVolume(BPDLightVolume*);
    virtual void ExitLightVolume(BPDLightVolume*);
    virtual int GetLightVolume();
    virtual void Init();
    virtual void SetScript(BSObject*);
    virtual void unknown12c();
    virtual void TranslateFromScript(CVector3&, CVector3&, EBSEventEnum);
    virtual void RotateFromScript(CVector3&, CVector3&, EBSEventEnum);
    virtual void ScaleFromScript(float, float, EBSEventEnum);
    virtual void StartMotionPlayback(int, int, EBSEventEnum);
    virtual CThrownObject* AsThrownObject();
    virtual void* AsWeaponObject();
    virtual void* AsHierTankObject();
    virtual void SetCollisionId(EClsnId);

    unsigned char unknown004[472];
    EClsnId m_collisionId;
};

class CThrownObject : public IMovingSceneNode {
public:
    void SetVelocity(CVector3&);

    unsigned char unknown004[756];
    int m_drop;
};

struct aistatus_match;

struct MatchListView {
    aistatus_match* list;
};

class CAIObject {
public:
    void SetupFieldOfViewCorpse(float, float);
    void SetupFieldOfViewModeList(aistatus_match*, int, float, float);
    void SetupLineOfSightModeList(aistatus_match*, int, float);

    unsigned char unknown000[192];
    bool m_targetStored;
    unsigned char unknown0c1[3];
    CAIObject* m_target;
};

class CAIFilterObject {
public:
    virtual ~CAIFilterObject();
    virtual void ChooseTarget(float);
    virtual void SetTargetMode(int);

    unsigned char unknown04[4];
    CAIObject* m_object;
};

struct AIDoodadView {
    void* unknown00;
    CAIFilterObject* filter;
};

class BSSchedule {
public:
    int Register(BSObject*, unsigned short, bool, int, int, int);

    unsigned char unknown00[40];
};

extern BSSchedule g_CorpseSearchSchedule;
extern BSSchedule g_TargetSearchSchedule;
extern BSSchedule g_DistanceToTargetSchedule;
extern BSObject* g_pBSObject;

CThrownObject* CreateSpecialThrownObject(TriggerObject_struct*, int, int);

void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

inline int BSArgBool(int** stack, int index) {
    return *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - index)) != 0;
}

inline CAIObject* GetAIObject(void* object) {
    return ((ISceneNode*)object)->GetAIDoodad()->filter->m_object;
}

inline bool HasTarget(CAIObject* ai) {
    return ai && (ai->m_targetStored || ai->m_target);
}

void BIFunc_RegisterDistanceToTarget(int** stack, void* object) {
    int result = 0;
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    unsigned short time = *(*stack - (count - 1));
    int nearDistance = *(*stack - (count - 2));
    int nearSquared = nearDistance * nearDistance;
    int farDistance = *(*stack - (count - 3));
    int farSquared = farDistance * farDistance;
    int repeat = *(*stack - (count - 4));
    CAIObject* ai = ((ISceneNode*)object)->GetAIDoodad()->filter->m_object;
    if (HasTarget(ai))
        result = g_DistanceToTargetSchedule.Register(g_pBSObject, time, (bool)repeat, nearSquared, farSquared, 0);
    else
        BSObjectTriggerEvent(g_pBSObject, 69, 0, g_pBSObject, true);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = result;
}

void BIFunc_DestroyThrownObject(int** stack, void* object) {
    ((ISceneNode*)object)->AsStaticObject()->AsThrownObject()->MarkForDestruction(2);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_DropFallingObject(int**, void* object) {
    CThrownObject* thrown = ((ISceneNode*)object)->AsStaticObject()->AsThrownObject();
    thrown->m_drop = 1;
}

void BIFunc_CreateChildFallingObject(int** stack, void* object) {
    CThrownObject* thrown;
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    int type = *(*stack - (count - 2));
    thrown = CreateSpecialThrownObject(g_pBSObject->trigger, g_pBSObject->trigger->core->crc, *(*stack - (count - 1)));
    CMatrix matrix;
    if (object) {
        ((ISceneNode*)object)->GetTMLocalToWorld(matrix);
        thrown->SetTMLocalToWorld(matrix);
    } else {
        TriggerCoreView* core = g_pBSObject->trigger->core;
        CVector3 position(core->x, core->y, core->z);
        core->rotation.GetMatrix(matrix);
        matrix.SetPos(position);
        thrown->SetTMLocalToWorld(matrix);
    }
    thrown->m_drop = 0;
    CVector3 velocity(0.0f, 0.0f, 0.0f);
    thrown->SetVelocity(velocity);
    thrown->BeginUpdate(1.0f);
    int id;
    int none = id = -1;
    switch (type) {
    case 0:
        id = 0;
        break;
    case 1:
        id = none;
        break;
    case 2:
        id = 15;
        break;
    case 3:
        id = 17;
        break;
    case 4:
        id = 16;
        break;
    case 5:
        id = 19;
        break;
    case 6:
        id = 18;
        break;
    case 7:
        id = 20;
        break;
    case 8:
        id = 24;
        break;
    case 9:
        id = 26;
        break;
    case 10:
        id = 27;
        break;
    case 11:
        id = 1;
        break;
    }
    thrown->SetCollisionId((EClsnId)id);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

__declspec(weak) void CStaticObject::SetCollisionId(EClsnId id) {
    m_collisionId = id;
}

void BIFunc_CreateChildThrownObject(int** stack, void* object) {
    CThrownObject* thrown;
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    float speed = *(float*)(*stack - (count - 2));
    float dx = *(float*)(*stack - (count - 3));
    float dy = *(float*)(*stack - (count - 4));
    float dz = *(float*)(*stack - (count - 5));
    int type = *(*stack - (count - 6));
    thrown = CreateSpecialThrownObject(g_pBSObject->trigger, g_pBSObject->trigger->core->crc, *(*stack - (count - 1)));
    CMatrix matrix;
    if (object) {
        ((ISceneNode*)object)->GetTMLocalToWorld(matrix);
        thrown->SetTMLocalToWorld(matrix);
    } else {
        TriggerCoreView* core = g_pBSObject->trigger->core;
        CVector3 position(core->x, core->y, core->z);
        core->rotation.GetMatrix(matrix);
        matrix.SetPos(position);
        thrown->SetTMLocalToWorld(matrix);
    }
    CVector3 velocity(dx, dy, dz);
    velocity.Normalize();
    velocity *= speed;
    thrown->SetVelocity(velocity);
    thrown->BeginUpdate(1.0f);
    int id;
    int none = id = -1;
    switch (type) {
    case 0:
        id = 0;
        break;
    case 1:
        id = none;
        break;
    case 2:
        id = 15;
        break;
    case 3:
        id = 17;
        break;
    case 4:
        id = 16;
        break;
    case 5:
        id = 19;
        break;
    case 6:
        id = 18;
        break;
    case 7:
        id = 20;
        break;
    case 8:
        id = 24;
        break;
    case 9:
        id = 26;
        break;
    case 10:
        id = 27;
        break;
    case 11:
        id = 1;
        break;
    }
    thrown->SetCollisionId((EClsnId)id);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
