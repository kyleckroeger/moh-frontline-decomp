// A fragment of bsbifunc.cpp (0x800312e0): the built-in that sets the first
// player's collision id by mode (1, -1 or 21; the -1 kept in a second local
// and copied back for mode 1, which reproduces the target's empty case
// block), the weak CPlayerObject::SetCollisionId (the id at +1740) it
// emits, then built-ins that set the
// soldier's head as the animation root and allow a path mechanic's collision
// damage (a flag bit of the static object), and the one that sets the
// collision id of the calling node (or its static object, unless the script
// trigger is of type 8) by mode, with the same kept -1 and an equivalent
// copy for the type-8 case. Each reads its argument below the
// script stack top and pops the built-in's arguments. The file name is this
// project's; the original record is bsbifunc.cpp. The
// functions, classes and globals are named by the mangled symbols; ISceneNode
// is declared with its virtual functions in the order of __vt__10ISceneNode
// (AsStaticObject at +116), CPlayerObject derives from it (its destructor is
// declared first and defined elsewhere), and the static-object, soldier, scene
// and built-in record views are inferred.
enum EClsnId {};
class CCollision;
class CDrawContext;
class CMatrix;
class CVector3;
class CBullet;
class CLight;
class CPlayerObject;
struct CStaticFlagsView;
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
    virtual CStaticFlagsView* AsStaticObject();
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

class CPlayerObject : public ISceneNode {
public:
    virtual ~CPlayerObject();
    void SetCollisionId(EClsnId);

    unsigned char unknown0004[1736];
    EClsnId m_collisionId;
};

struct CStaticFlagsView {
    unsigned char unknown000[480];
    unsigned char unknown1e0 : 7;
    unsigned char allowCollisionDamage : 1;
};

class CSoldierObject {
public:
    void SetHeadTrans(bool);
};

#include "game/CScene.h"


extern CScene g_scene;

struct TriggerCoreView {
    unsigned char unknown00[12];
    int type;
};

struct TriggerObject_struct {
    unsigned char unknown00[4];
    TriggerCoreView* core;
};

struct BSObjectView {
    unsigned char unknown00[8];
    TriggerObject_struct* trigger;
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

void BIFunc_SetPlayerCollision(int** stack, void*) {
    int mode = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    CPlayerObject* player = g_scene.GetPlayer(0);
    if (player) {
        int id;
        int none = id = -1;
        switch (mode) {
        case 0:
            id = 1;
            break;
        case 1:
            id = none;
            break;
        case 2:
            id = 21;
            break;
        }
        ((ISceneNode*)player)->SetCollisionId((EClsnId)id);
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

__declspec(weak) void CPlayerObject::SetCollisionId(EClsnId id) {
    m_collisionId = id;
}

void BIFunc_SetHeadAsRoot(int** stack, void* object) {
    ((CSoldierObject*)object)->SetHeadTrans(*(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1)) != 0);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_PathMechAllowCollisionDamage(int** stack, void* object) {
    int allow = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    ((ISceneNode*)object)->AsStaticObject()->allowCollisionDamage = allow != 0;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_SetMechanicCollision(int** stack, void* object) {
    ISceneNode* node;
    ISceneNode* self = node = (ISceneNode*)object;
    int mode = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    switch (g_pBSObject->trigger->core->type) {
    case 8:
        node = self;
        break;
    default:
        node = (ISceneNode*)node->AsStaticObject();
        break;
    }
    int id;
    int none = id = -1;
    switch (mode) {
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
    node->SetCollisionId((EClsnId)id);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
