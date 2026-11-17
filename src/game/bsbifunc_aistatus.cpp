// A fragment of bsbifunc.cpp (0x80031bc4): AI built-ins that set up a grenade
// shot (for each of flags 1, 2 and 4 in turn, while the AI object has the
// pointer at +196, test a grenade shot at 20, 45 or 5 degrees and return the
// first flag whose test gives an angle other than -1, else 0; the last tested
// angle is stored in the script data at +184), set a status
// variable (status 13 set to 15 also clears status 0 and switches the animated
// object's high-detail or player collision id to 25), send a flag (flags 1, 4
// and others set bits; flags 2 and 8 set or clear a bit by the second argument,
// clearing 2 also clears a bit of another flag word), return the planar
// distance to the move point (the float result through its address), and snap
// to the move point (copying it to the snap position and setting a flag bit)
// with or without an angle. Each reaches the AI object through the scene node's
// AI doodad and its filter, reads its arguments below the script stack top and
// pops the built-in's arguments. The file name is this project's; the original
// record is bsbifunc.cpp and the built-ins around these are not reconstructed.
// The functions, classes and globals are named by the mangled symbols;
// ISceneNode is declared with its virtual functions in the order of
// __vt__10ISceneNode (GetCollisionId at +88, SetCollisionId at +92,
// AsAnimObject at +140, GetAIDoodad at +204); the doodad, filter, AI object
// (members at their offsets, names not original) and built-in record views, the
// argument helpers, the grenade-shot helper, the script-object, script-data
// and result views and the reversed copy of the move point are inferred.
enum EClsnId {};
class CCollision;
class CDrawContext;
class CMatrix;
class CVector3;
class CBullet;
class CLight;
class CPlayerObject;
struct AIDoodadView;
struct BSObjectView;

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
    virtual BSObjectView* GetScriptObject() const;
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
    virtual ISceneNode* AsAnimObject();
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
    float TestGrenadeShot(float);

    unsigned char unknown000[24];
    CAIFilterRealPosition m_position;
    unsigned char unknown024[20];
    CAIFilterRealPosition m_snapPosition;
    unsigned char unknown044[4];
    float m_snapAngle;
    bool m_snapToAngle;
    unsigned char unknown04d[119];
    void* m_unknown0c4;
    unsigned char unknown0c8[224];
    unsigned int m_flags424;
    unsigned char unknown1ac[4];
    unsigned int m_flags432;
    int m_status[69];
    CAIFilterRealPosition m_moveTarget;
};

struct CAIFilterView {
    unsigned char unknown00[8];
    CAIObject* object;
};

struct AIDoodadView {
    void* unknown00;
    CAIFilterView* filter;
};

struct ScriptDataView {
    unsigned char unknown00[184];
    float m_unknown0b8;
};

class BSGO_Basic {
    unsigned char unknown00[12];

public:
    virtual void Destroy();
    virtual ScriptDataView* GetScriptData();
};

struct BSObjectView {
    unsigned char unknown00[12];
    BSGO_Basic* user;
};

union BSValueView {
    int i;
    float f;
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

inline int BSArgInt(int** stack, int index) {
    return *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - index));
}

inline float BSArgFloat(int** stack, int index) {
    return *(float*)(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - index));
}

inline CAIObject* GetAIObject(void* object) {
    return ((ISceneNode*)object)->GetAIDoodad()->filter->object;
}

inline int ChooseGrenadeShot(CAIObject* ai, int flags, float& angle) {
    if (ai->m_unknown0c4) {
        if ((flags & 1) && (angle = ai->TestGrenadeShot(0.34906584f)) != -1.0f)
            return 1;
        if ((flags & 2) && (angle = ai->TestGrenadeShot(0.7853982f)) != -1.0f)
            return 2;
        if ((flags & 4) && (angle = ai->TestGrenadeShot(0.08726646f)) != -1.0f)
            return 4;
    }
    return 0;
}

void BIFunc_SetupGrenadeShot(int** stack, void* object) {
    BSValueView value;
    float angle;
    int flags = BSArgInt(stack, 1);
    value.i = ChooseGrenadeShot(GetAIObject(object), flags, angle);
    ((ISceneNode*)object)->GetScriptObject()->user->GetScriptData()->m_unknown0b8 = angle;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    *(BSValueView*)*stack = value;
}

void BIFunc_AISetStatusVar(int** stack, void* object) {
    int var = BSArgInt(stack, 1);
    int value = BSArgInt(stack, 2);
    CAIObject* ai = GetAIObject(object);
    if (var == 13 && value == 15)
        ai->m_status[0] = 0;
    ai->m_status[var] = value;
    if (var == 13 && value == 15) {
        ISceneNode* anim = ((ISceneNode*)object)->AsAnimObject();
        if (anim && (anim->GetCollisionId() == 13 || anim->GetCollisionId() == 2))
            anim->SetCollisionId((EClsnId)25);
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_AISendFlag(int** stack, void* object) {
    CAIFilterView* filter;
    int flag;
    int* top;
    int count;
    filter = ((ISceneNode*)object)->GetAIDoodad()->filter;
    top = *stack;
    count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    flag = *(top - (count - 1));
    switch (flag) {
    default:
        filter->object->m_flags432 |= flag;
        break;
    case 1:
        filter->object->m_flags432 |= 1;
        break;
    case 2: {
        CAIObject* ai = filter->object;
        if (*(top - (count - 2))) {
            ai->m_flags432 |= 2;
        } else {
            ai->m_flags432 &= ~2;
            ai->m_flags424 &= ~1;
        }
        break;
    }
    case 4:
        filter->object->m_flags432 |= 4;
        break;
    case 8:
        if (*(top - (count - 2)))
            filter->object->m_flags432 |= 8;
        else
            filter->object->m_flags432 &= ~8;
        break;
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_AIGetDistanceToMovePoint(int** stack, void* object) {
    CAIObject* ai = GetAIObject(object);
    float distance = ai->m_position.GetDistanceXYReal(ai->m_moveTarget);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = *(int*)&distance;
}

void BIFunc_AISnapToMovePointAndAngle(int** stack, void* object) {
    float angle = BSArgFloat(stack, 1);
    CAIObject* ai = GetAIObject(object);
    ai->m_snapToAngle = true;
    ai->m_snapAngle = angle;
    float z = ai->m_moveTarget.z;
    float y = ai->m_moveTarget.y;
    float x = ai->m_moveTarget.x;
    ai->m_snapPosition.x = x;
    ai->m_snapPosition.y = y;
    ai->m_snapPosition.z = z;
    ai->m_flags424 |= 2;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_AISnapToMovePoint(int** stack, void* object) {
    CAIObject* ai = GetAIObject(object);
    ai->m_snapToAngle = false;
    float z = ai->m_moveTarget.z;
    float y = ai->m_moveTarget.y;
    float x = ai->m_moveTarget.x;
    ai->m_snapPosition.x = x;
    ai->m_snapPosition.y = y;
    ai->m_snapPosition.z = z;
    ai->m_flags424 |= 2;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
