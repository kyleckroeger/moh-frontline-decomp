// A fragment of bsbifunc.cpp (0x8001e3ec): the scheduled buddy-player check
// (clears the second output; for a nonzero mode, 1 when the script object's AI
// does not have the target's scene-node position in line of sight within the
// distance, otherwise 1 when it does), the weak BSGO_Basic::GetSceneNode (0),
// emitted after its first use, then the scheduled corpse-search check: when
// the script object's AI sees a corpse, it stores the corpse's script object
// and 0 in the two outputs and returns 1, otherwise 0. The file name is this project's; the original record
// is bsbifunc.cpp and the functions around these are not reconstructed. The
// functions, classes and globals are named by the mangled symbols; ISceneNode
// is declared with its virtual functions in the order of __vt__10ISceneNode
// (GetAIDoodad at +204) and BSGO_Basic in the order of __vt__10BSGO_Basic;
// BSObject, the AI object, filter, doodad, owner, trigger and vector views and
// the script helper are inferred (members at their offsets,
// names not original; the position is passed to the AI as its real-position
// type).
enum EClsnId {};
class CCollision;
class CDrawContext;
class CMatrix;
#include "game/CVector3.h"

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

class CAIFilterRealPosition;

class CAIObject {
public:
    bool IsObjectInLineOfSight(CAIFilterRealPosition&, float, bool);
    CAIObject* CheckFieldOfViewCorpse();
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

int DoBuddyPlayerCheck(BSObject* object, int mode, int distance, int target, int*, int* extra) {
    CAIFilterView* filter = object->user->GetSceneNode()->GetAIDoodad()->filter;
    CVector3 position;
    ((BSObject*)target)->user->GetSceneNode()->GetPosition(position);
    *extra = 0;
    if (mode)
        return filter->object->IsObjectInLineOfSight((CAIFilterRealPosition&)position, distance, false) ? 0 : 1;
    return filter->object->IsObjectInLineOfSight((CAIFilterRealPosition&)position, distance, false) != 0;
}

__declspec(weak) ISceneNode* BSGO_Basic::GetSceneNode() {
    return 0;
}

int DoCorpseSearch(BSObject* object, int, int, int, int* found, int* extra) {
    BSObject* script = GetScript(object->user->GetSceneNode()->GetAIDoodad()->filter->object->CheckFieldOfViewCorpse());
    if (!script)
        return 0;
    *extra = 0;
    *found = (int)script;
    return 1;
}
