// A fragment of bsbifunc.cpp (0x80020030): the script built-in that
// registers bullet damage on the calling node's hierarchy object (three
// integers and a scale, a negative scale meaning its reciprocal; the
// constants are entries of the file's .sdata2 pool), the one that fires
// a sub-object weapon of the calling node's hierarchy tank (when it is one),
// followed by the weak CStaticObject::AsHierTankObject (0), and the one that
// returns the aim angle in degrees (pan, or negated tilt) from a sub-object
// of the tank to its aim target (or alternate target) raised by a height.
// The angle helpers are declared returning float. Each reads its
// arguments below the script stack top and pops the built-in's arguments.
// The file name is this project's; the original record is bsbifunc.cpp and
// the built-ins around these are not reconstructed. The functions, classes
// and globals are named by the mangled symbols; ISceneNode is declared with
// its virtual functions in the order of __vt__10ISceneNode (AsHierObject at
// +124), IMovingSceneNode's and CStaticObject's after them in the order of
// __vt__13CStaticObject (AsHierTankObject at +328; the unnamed entry at +300
// as a placeholder); the tank-object and built-in record views and the
// argument helper are inferred.


#include "game/CVector3.h"

class CCollision;
class CDrawContext;
class CMatrix;
class CBullet;
class CLight;
class CPlayerObject;
class CStaticObject;
class CHierObject;
class CTankObject;
struct AIDoodadView;

#include "game/ISceneNode.h"



class IMovingSceneNode : public ISceneNode {
public:
    virtual ~IMovingSceneNode();
    virtual void Reset();
    virtual void Halt();
    virtual void ApplyForceTo(CVector3, float);
    virtual void SetTMLocalToWorld(const CMatrix&);
};

class BSObject;
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
    virtual void* AsThrownObject();
    virtual void* AsWeaponObject();
    virtual CTankObject* AsHierTankObject();
};

class CTankObject {
public:
    void FireSubObject(int);
    void MapWeaponSlot(int, int);

    unsigned char unknown000[1940];
    int m_lookObject;
    unsigned char unknown798[120];
    CVector3 m_aimTarget;
    CVector3 m_altAimTarget;
};

// CHierObject derives from CStaticObject (its destructor declared and defined
// elsewhere).
class CHierObject : public CStaticObject {
public:
    virtual ~CHierObject();
    CStaticObject* GetSubObject(int);
    void RegisterBulletDamage(int, int, int, float);
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

void BIFunc_HierObjectRegisterBulletDamage(int** stack, void* object) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    int a = *(*stack - (count - 1));
    int b = *(*stack - (count - 2));
    int c = *(*stack - (count - 3));
    float scale = *(float*)(*stack - (count - 4));
    CHierObject* hier = ((ISceneNode*)object)->AsHierObject();
    if (scale < 0.0f)
        scale = 1.0f / -scale;
    hier->RegisterBulletDamage(a, b, c, scale);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_HierObjectFire(int** stack, void* object) {
    int weapon = BSArgInt(stack, 1);
    CTankObject* tank = ((ISceneNode*)object)->AsHierObject()->AsHierTankObject();
    if (tank)
        tank->FireSubObject(weapon);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

__declspec(weak) CTankObject* CStaticObject::AsHierTankObject() {
    return 0;
}

float MathFunGetTiltAngleDiffNoRoll(CVector3*, CVector3*, CVector3*, CVector3*);
float MathFunGetPanAngleDiffNoRoll(CVector3*, CVector3*, CVector3*);

void BIFunc_HierObjectGetAimAngle(int** stack, void* object) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    float height;
    bool alternate;
    bool tilt;
    int index;
    index = *(*stack - (count - 1));
    tilt = *(*stack - (count - 2)) != 0;
    alternate = *(*stack - (count - 3)) != 0;
    height = *(float*)(*stack - (count - 4));
    float angle;
    CVector3 aim;
    CVector3 position;
    CVector3 target;
    CVector3 rightward;
    CVector3 forward;
    CTankObject* tank = ((ISceneNode*)object)->AsHierObject()->AsHierTankObject();
    CStaticObject* sub = ((CHierObject*)tank)->GetSubObject(index);
    sub->GetPosition(position);
    if (alternate)
        aim = tank->m_altAimTarget;
    else
        aim = tank->m_aimTarget;
    target = aim;
    target.z += height;
    sub->GetRightward(rightward);
    if (tilt) {
        sub->GetForward(forward);
        angle = MathFunGetTiltAngleDiffNoRoll(&position, &target, &rightward, &forward);
        angle = -angle;
    } else {
        angle = MathFunGetPanAngleDiffNoRoll(&position, &target, &rightward);
    }
    angle = 57.295776f * angle;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = *(int*)&angle;
}
