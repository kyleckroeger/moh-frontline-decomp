// A fragment of bsbifunc.cpp (0x8001e7b8..0x8001eae4): the scheduled target-search check:
// by mode (2 field of view, 3 line of sight) the script object's AI checks its
// match list; on a hit it stores the target's script object and a random 1 to 9
// times the trigger's search scale in the two outputs and returns 1, otherwise
// 0. DoDistanceToTarget, the scheduled distance check: the squared distance
// from the AI to its target (the player, or the target object) when it has
// one, otherwise the object leaves the distance schedule and gets script
// event 69; outside the minimum..maximum band it stores the distance (through
// the inline sqrtf) and returns 1. DoMMGPointCheck asks the AI filter whether
// the object should leave the given machine-gun point. The file name is this project's; the original record is bsbifunc.cpp and
// the functions around it are not reconstructed. The functions, classes and
// globals are named by the mangled symbols; ISceneNode is declared with its
// virtual functions in the order of __vt__10ISceneNode (GetAIDoodad at +204)
// and BSGO_Basic in the order of __vt__10BSGO_Basic; BSObject, the AI object,
// filter (CAIFilterObject, named by the ShouldILeaveMMG symbol), doodad, owner and trigger views and the script helper are inferred
// (members at their offsets, names not original), as are the AI's target
// members, the 28-byte BSSchedule view (g_DistanceToTargetSchedule is
// file-local in the original and declared extern here) and the MSL-style
// inline sqrtf (as in propdat_bsp.cpp); 0.0f, 0.5, 3.0 and the integer
// conversion bias are .sdata2 pool entries.
enum EClsnId {};
class CCollision;
class CDrawContext;
class CMatrix;
class CVector3;
class CBullet;
class CLight;
class CPlayerObject;
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
    virtual void* AsStaticObject();
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


class BSObject;

struct AIOwnerView {
    unsigned char unknown00[12];
    BSObject* script;
};

class CAIObject {
public:
    float GetDistanceSquaredXYZReal();
    float GetDistanceSquaredXYZReal(CAIObject*);
    CAIObject* CheckFieldOfViewCorpse();
    CAIObject* CheckFieldOfViewModeList();
    CAIObject* CheckLineOfSightModeList();

    unsigned char unknown00[8];
    AIOwnerView* m_owner;
    unsigned char unknown0c[180];
    bool m_targetIsPlayer;
    unsigned char unknownc1[3];
    CAIObject* m_target;
};

struct CAIFilterObject {
    int ShouldILeaveMMG(BSObject*);

    unsigned char unknown00[8];
    CAIObject* object;
};

struct AIDoodadView {
    void* unknown00;
    CAIFilterObject* filter;
};

class BSGO_Basic {
    unsigned char unknown00[12];

public:
    virtual void Destroy();
    virtual int GetScriptData();
    virtual ISceneNode* GetSceneNode();
};

struct TriggerCoreView {
    unsigned char unknown00[121];
    unsigned char searchScale;
};

struct TriggerObject_struct {
    unsigned char unknown00[4];
    TriggerCoreView* core;
};

class BSObject {
public:
    unsigned char unknown00[8];
    TriggerObject_struct* trigger;
    BSGO_Basic* user;
};

inline BSObject* GetScript(CAIObject* ai) {
    if (ai)
        return ai->m_owner->script;
    return 0;
}

long long MathFunRandomI64(long long, long long);

class BSSchedule {
public:
    void Unregister(BSObject*);

    unsigned char unknown00[28];
};

extern BSSchedule g_DistanceToTargetSchedule;

int BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);

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
        return y ;
    }
    return x;
}


int DoTargetSearch(BSObject* object, int mode, int, int, int* found, int* extra) {
    BSObject* script = 0;
    CAIFilterObject* filter = object->user->GetSceneNode()->GetAIDoodad()->filter;
    switch (mode) {
    case 2:
        script = GetScript(filter->object->CheckFieldOfViewModeList());
        break;
    case 3:
        script = GetScript(filter->object->CheckLineOfSightModeList());
        break;
    }
    if (!script)
        return 0;
    *extra = MathFunRandomI64(1, 9) * object->trigger->core->searchScale;
    *found = (int)script;
    return 1;
}

int DoDistanceToTarget(BSObject* object, int minimum, int maximum, int, int* found, int* extra) {
    float distance = 0.0f;
    CAIObject* ai = object->user->GetSceneNode()->GetAIDoodad()->filter->object;
    bool hasTarget = false;
    if (ai && (ai->m_targetIsPlayer || ai->m_target))
        hasTarget = true;
    if (hasTarget) {
        distance = ai->m_targetIsPlayer ? ai->GetDistanceSquaredXYZReal() : ai->GetDistanceSquaredXYZReal(ai->m_target);
    } else {
        g_DistanceToTargetSchedule.Unregister(object);
        BSObjectTriggerEvent(object, 69, 0, object, true);
    }
    if (distance > minimum && distance < maximum)
        return 0;
    int value = sqrtf(distance);
    *extra = 0;
    *found = value;
    return 1;
}

int DoMMGPointCheck(BSObject* object, int point, int, int, int*, int* extra) {
    BSObject* mmg = (BSObject*)point;
    if (!mmg)
        return 0;
    CAIFilterObject* filter = object->user->GetSceneNode()->GetAIDoodad()->filter;
    *extra = 0;
    return filter->ShouldILeaveMMG(mmg);
}
