// PENDING: not landable until the maintainer answers State question 11 (dead stack copies the original
// keeps): a constructor that writes through a cast of `this`, which also changes the landed
// AIFilter_queries.
// Function: DoTargetCheck: 0 real lines.
// Source: scratch/loop3/scripting/s12/dtc/DISC0_ctorcast.cpp (copied 2026-11-12 by loop agent land2 so
// the work is pushed).
// Not in research/drafts/index.tsv; the index draft for the function stays the landable form.
// See research/plans/three-day-loop.md, State, "Waiting on the maintainer".
// A fragment of bsbifunc.cpp (0x8001e7b8): the scheduled target-search check:
// by mode (2 field of view, 3 line of sight) the script object's AI checks its
// match list; on a hit it stores the target's script object and a random 1 to 9
// times the trigger's search scale in the two outputs and returns 1, otherwise
// 0. The file name is this project's; the original record is bsbifunc.cpp and
// the functions around it are not reconstructed. The functions, classes and
// globals are named by the mangled symbols; ISceneNode is declared with its
// virtual functions in the order of __vt__10ISceneNode (GetAIDoodad at +204)
// and BSGO_Basic in the order of __vt__10BSGO_Basic; BSObject, the AI object,
// filter, doodad, owner and trigger views and the script helper are inferred
// (members at their offsets, names not original).
enum EClsnId {};
class CCollision;
class CDrawContext;
class CMatrix;
struct Pair { double a, b; };
union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(float ax, float ay, float az) { d.v[0] = ax; d.v[1] = ay; d.v[2] = az; }
    CVector3(const CVector3& o) { *(Pair*)this = *(const Pair*)&o; }
    CVector3Data d;
} __attribute__((aligned(8)));
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
    CAIObject* CheckFieldOfViewCorpse();
    CAIObject* CheckFieldOfViewModeObject();
    CAIObject* CheckLineOfSightModeObject();
    CAIObject* CheckVisionSeekingMode();
    CAIObject* CheckFieldOfViewModeList();
    CAIObject* CheckLineOfSightModeList();

    unsigned char unknown00[8];
    AIOwnerView* m_owner;
};

struct CAIFilterView {
    unsigned char unknown00[8];
    CAIObject* object;
};

struct AIDoodadView {
    void* unknown00;
    CAIFilterView* filter;
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

class CAIFilterRealPosition {
public:
    CAIFilterRealPosition() {}
    CAIFilterRealPosition(CVector3 v) { ((float*)this)[0] = v.d.v[0]; ((float*)this)[1] = v.d.v[1]; ((float*)this)[2] = v.d.v[2]; }

    float x;
    float y;
    float z;
};

bool DoTargetCheck(BSObject* object, int mode, int distance, int target, int*, int* extra) {
    ISceneNode* node;
    bool result;
    result = 1;
    node = object->user->GetSceneNode();
    CAIFilterView* filter = node->GetAIDoodad()->filter;
    switch (mode) {
    case 1: {
        ISceneNode* other = ((BSObject*)target)->user->GetSceneNode();
        if (other) {
            CVector3 position;
            CVector3 otherPosition;
            other->GetPosition(otherPosition);
            CAIFilterRealPosition targetPosition(otherPosition);
            node->GetPosition(position);
            float dx = position.d.v[0] - otherPosition.d.v[0];
            float dy = position.d.v[1] - otherPosition.d.v[1];
            result = dx * dx + dy * dy > distance * distance;
        }
        break;
    }
    case 2:
        if (GetScript(filter->object->CheckFieldOfViewModeObject()))
            result = 0;
        break;
    case 3:
        if (GetScript(filter->object->CheckLineOfSightModeObject()))
            result = 0;
        break;
    case 4:
        result = GetScript(filter->object->CheckVisionSeekingMode());
        break;
    case 5:
        if (!GetScript(filter->object->CheckLineOfSightModeObject()))
            result = 0;
        break;
    }
    *extra = 0;
    return result;
}

long long MathFunRandomI64(long long, long long);

int DoTargetSearch(BSObject* object, int mode, int, int, int* found, int* extra) {
    BSObject* script = 0;
    CAIFilterView* filter = object->user->GetSceneNode()->GetAIDoodad()->filter;
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
