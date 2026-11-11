// A fragment of bsbifunc.cpp (0x800203e0): script built-ins that map a weapon
// slot of the calling node's hierarchy tank (the weapon given 1-based), set
// the tank's look object, and copy property values of the running script
// object's trigger into its script data (bytes and shorts, four of the shorts
// shifted right by 4 and stored as floats, the larger of two of them taken,
// and two of the floats given defaults of 100 and 50 when at most 0.001). Each
// reads its arguments below the script stack top and pops the built-in's
// arguments. The file name is this project's; the original record is
// bsbifunc.cpp and the built-ins around these are not reconstructed. The
// functions, classes and globals are named by the mangled symbols; ISceneNode
// is declared with its virtual functions in the order of __vt__10ISceneNode
// (AsHierObject at +124), IMovingSceneNode's and CStaticObject's after them in
// the order of __vt__13CStaticObject (AsHierTankObject at +328; the unnamed
// entry at +300 as a placeholder), and BSGO_Basic in the order of
// __vt__10BSGO_Basic; the tank-object, trigger, script-object, script-data
// (fields named by offset; their meanings are not known) and built-in record
// views and the argument helper are inferred.
#include "game/CVector3.h"

enum EClsnId {};
class CCollision;
class CDrawContext;
class CMatrix;
class CBullet;
class CLight;
class CPlayerObject;
class CStaticObject;
class CTankObject;
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
    virtual CStaticObject* AsHierObject();
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
};

struct TriggerCoreView {
    unsigned char unknown00[118];
    unsigned char unknown76;
    unsigned char unknown77;
    unsigned char unknown78;
    unsigned char unknown79;
    unsigned char unknown7a;
    unsigned char unknown7b[3];
    short unknown7e;
    short unknown80;
    unsigned char unknown82[2];
    short unknown84;
    unsigned char unknown86[2];
    short unknown88;
    short unknown8a;
    unsigned char unknown8c[12];
    int unknown98;
};

struct TriggerObject_struct {
    unsigned char unknown00[4];
    TriggerCoreView* core;
};

struct SharedDataView {
    unsigned char unknown00[52];
    int unknown34;
    int unknown38;
    unsigned char unknown3c[12];
    float unknown48;
    int unknown4c;
    float unknown50;
    float unknown54;
    int unknown58;
    int unknown5c;
    int unknown60;
};

class BSGO_Basic {
    unsigned char unknown00[12];

public:
    virtual void Destroy();
    virtual SharedDataView* GetScriptData();
};

struct BSObjectView {
    unsigned char unknown00[8];
    TriggerObject_struct* trigger;
    BSGO_Basic* user;
};

extern BSObjectView* g_pBSObject;

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

void BIFunc_HierObjectMapWeapon(int** stack, void* object) {
    int slot = BSArgInt(stack, 1);
    int weapon = BSArgInt(stack, 2);
    CTankObject* tank = ((ISceneNode*)object)->AsHierObject()->AsHierTankObject();
    tank->MapWeaponSlot(slot, weapon - 1);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_HierObjectSetLookObject(int** stack, void* object) {
    int look = BSArgInt(stack, 1);
    CTankObject* tank = ((ISceneNode*)object)->AsHierObject()->AsHierTankObject();
    tank->m_lookObject = look;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_SetAIObjectSharedDataPropVars(int** stack, void*) {
    SharedDataView* data = g_pBSObject->user->GetScriptData();
    TriggerCoreView* core = g_pBSObject->trigger->core;
    data->unknown34 = core->unknown77;
    data->unknown38 = core->unknown78;
    data->unknown48 = core->unknown7e >> 4;
    data->unknown4c = core->unknown76;
    int a = core->unknown80 >> 4;
    int b = core->unknown88 >> 4;
    if (a > b)
        data->unknown50 = a;
    else
        data->unknown50 = b;
    data->unknown54 = core->unknown8a >> 4;
    data->unknown58 = core->unknown98;
    data->unknown5c = core->unknown84;
    data->unknown60 = core->unknown7a;
    if (data->unknown50 <= 0.001f)
        data->unknown50 = 100.0f;
    if (data->unknown54 <= 0.001f)
        data->unknown54 = 50.0f;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
