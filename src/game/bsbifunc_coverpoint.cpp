// A fragment of bsbifunc.cpp (0x80033110): AI built-ins that report whether
// the AI object has arrived at a cover point (the cover point with the given
// index is its current one and its cover flag is set), continue a
// cover-point walk (walk state 9) invalidate a cover point by id (when
// non-zero), and snap to the current cover point's position (while in cover:
// the modified position becomes the cover target, with the two arguments
// from the stack top; beyond 0.6 in XY the script gets event 66, otherwise
// flag 2 is set; 0.6f is an entry of the file's .sdata2 pool). Each reads
// its arguments below the script stack top and pops the built-in's
// arguments. The file name is this project's; the original record is
// bsbifunc.cpp and the built-ins around these are not reconstructed. The
// functions, classes and globals are named by the mangled symbols;
// ISceneNode is declared with its virtual functions in the order of
// __vt__10ISceneNode (GetAIDoodad at +204), and the doodad, filter,
// CAIObject and built-in record views are inferred.
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


class CCoverPoint;

class CAIFilterGlobal {
public:
    CCoverPoint* GetCoverPointByIndex(int) const;
    void InvalidateCoverPointByID(int);

    unsigned char unknown00[24];
};

class BSObject;

void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);

#include "game/CAIFilterRealPosition.h"


// Inferred: a direction vector usable as a position (16 bytes in the object).
class CAIFilterRealVector3 : public CAIFilterRealPosition {
public:
    float w;
};

enum COVERPOINT_TYPE {};

class CCoverPoint {
public:
    void GetModifiedPosition(CAIFilterRealVector3&, COVERPOINT_TYPE) const;
};

struct AIFilterScriptView {
    unsigned char unknown00[12];
    BSObject* script;
};

class CAIObject {
public:
    unsigned char unknown000[8];
    AIFilterScriptView* m_filter;
    unsigned char unknown00c[12];
    CAIFilterRealPosition m_position;
    unsigned char unknown024[20];
    CAIFilterRealVector3 m_coverTarget;
    float m_coverValue;
    bool m_coverOption;
    unsigned char unknown04d[347];
    unsigned int m_flags1a8;
    unsigned char unknown1ac[108];
    int m_walkState;
    unsigned char unknown21c[116];
    CCoverPoint* m_coverPoint;
    COVERPOINT_TYPE m_coverType;
    unsigned char unknown298[40];
    bool m_coverFlag;
};

struct CAIFilterView {
    unsigned char unknown00[8];
    CAIObject* object;
};

struct AIDoodadView {
    void* unknown00;
    CAIFilterView* filter;
};

extern CAIFilterGlobal g_aigAIFilterGlobalObject;

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

void BIFunc_AIArrivedAtCoverpoint(int** stack, void* object) {
    CAIFilterView* filter;
    int index = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    filter = ((ISceneNode*)object)->GetAIDoodad()->filter;
    CCoverPoint* point = g_aigAIFilterGlobalObject.GetCoverPointByIndex(index);
    CAIObject* ai = filter->object;
    bool arrived = false;
    if (point == ai->m_coverPoint && ai->m_coverFlag)
        arrived = true;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = arrived;
}

void BIFunc_AIContinueCoverPointWalk(int** stack, void* object) {
    GetAIObject(object)->m_walkState = 9;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_AIInvalidateCoverPointByID(int** stack, void*) {
    int id = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    if (id != 0)
        g_aigAIFilterGlobalObject.InvalidateCoverPointByID(id);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

// Inferred: an argument read at an offset from the stack top as a 0/1 int.
inline int BSTopArgBool(int** stack, int index) {
    return (*stack)[index] != 0;
}

void BIFunc_AISnapToCoverPointPosition(int** stack, void* object) {
    bool option = BSTopArgBool(stack, -1);
    float value = *(float*)*stack;
    CAIObject* ai = GetAIObject(object);
    if (ai->m_coverFlag) {
        ai->m_coverPoint->GetModifiedPosition(ai->m_coverTarget, ai->m_coverType);
        ai->m_coverOption = option;
        ai->m_coverValue = value;
        if (ai->m_position.GetDistanceXYReal(ai->m_coverTarget) > 0.6f)
            BSObjectTriggerEvent(ai->m_filter->script, 66, 0, 0, false);
        else
            ai->m_flags1a8 |= 2;
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
