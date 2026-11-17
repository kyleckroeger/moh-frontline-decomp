// A fragment of bsbifunc.cpp (0x80033728): AI built-ins that start an A* path
// walk to the move point, to the current target, or to the AI object of a given
// script object (reached through its user object's scene node, AI doodad and
// filter; false when there is none), returning the result. Each reaches the
// script object's AI object through the scene node's AI doodad and its filter,
// and pops the built-in's arguments. The file name is this project's; the
// original record is bsbifunc.cpp and the built-ins around these are not
// reconstructed. The functions, classes and globals are named by the mangled
// symbols; ISceneNode is declared with its virtual functions in the order of
// __vt__10ISceneNode (GetAIDoodad at +204) and BSGO_Basic in the order of
// __vt__10BSGO_Basic, and the script-object, doodad, filter and CAIObject views
// are inferred (members at their offsets, names not original).
enum EClsnId {};
class CCollision;
class CDrawContext;
class CMatrix;
class CVector3;
class CBullet;
class CLight;
class CPlayerObject;
struct AIDoodadView;
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

#include "game/CAIFilterRealPosition.h"


class CAIObject {
public:
    void CommitCoverPointWalkToStored();
    bool StartCoverPointWalkToStored(bool);
    bool StartAStarPathWalk();
    bool StartAStarPathWalk(CAIObject*);
    void CleanupAStarPathWalk();

    unsigned char unknown000[24];
    CAIFilterRealPosition m_position;
    unsigned char unknown024[44];
    CAIFilterRealPosition m_moveTarget;
    unsigned char unknown05c[104];
    CAIObject* m_target;
    unsigned char unknown0c8[336];
    int m_walkState;
    unsigned char unknown21c[4];
    float m_moveTargetDistance;
    bool m_flag224;
    bool m_flag225;
    unsigned char unknown226[62];
    int m_path264;
    int m_path268;
    int m_path26c;
    unsigned char unknown270[80];
    bool m_coverFlag;
    unsigned char unknown2c1[7];
    float m_storedX;
    float m_storedY;
    float m_storedZ;
};

struct CAIFilterView {
    unsigned char unknown00[8];
    CAIObject* object;
};

struct AIDoodadView {
    void* unknown00;
    CAIFilterView* filter;
};

#include "game/BSGO_Basic.h"


struct BSObjectView {
    unsigned char unknown00[12];
    BSGO_Basic* user;
};

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

inline CAIObject* GetAIObject(void* object) {
    return ((ISceneNode*)object)->GetAIDoodad()->filter->object;
}

void BIFunc_AIStopCoverPointWalk(int** stack, void* object);

void BIFunc_AICommitCoverPointWalkToStored(int** stack, void* object);

void BIFunc_AIStartCoverPointWalkToStored(int** stack, void* object);

void BIFunc_AIStartCoverPointWalk(int** stack, void* object);

void BIFunc_AIContinueAStarPathWalk(int** stack, void* object);

void BIFunc_AIStopAStarPathWalk(int** stack, void* object);

void BIFunc_AIStartAStarPathWalkToMovePoint(int** stack, void* object) {
    bool result = GetAIObject(object)->StartAStarPathWalk();
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = result;
}

void BIFunc_AIStartAStarPathWalkWithCurrentTarget(int** stack, void* object) {
    CAIObject* ai = GetAIObject(object);
    bool result = ai->StartAStarPathWalk(ai->m_target);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = result;
}


inline CAIFilterView* GetAIFilter(BSObjectView* script) {
    if (script && script->user) {
        ISceneNode* node = script->user->GetSceneNode();
        if (node) {
            AIDoodadView* doodad = node->GetAIDoodad();
            if (doodad)
                return doodad->filter;
        }
    }
    return 0;
}

void BIFunc_AIStartAStarPathWalk(int** stack, void* object) {
    CAIObject* ai;
    CAIObject* target;
    BSObjectView* script;
    script = *(BSObjectView**)(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    ai = GetAIObject(object);
    target = 0;
    CAIFilterView* filter = GetAIFilter(script);
    if (filter)
        target = filter->object;
    bool result;
    if (target)
        result = ai->StartAStarPathWalk(target);
    else
        result = false;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = result;
}
