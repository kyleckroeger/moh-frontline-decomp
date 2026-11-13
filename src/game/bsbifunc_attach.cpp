// A fragment of bsbifunc.cpp (0x8002587c): script built-ins that attach a
// script object's scene node to the calling soldier's empty hand (if the node
// is in the scene, try "rt_hand" then "lt_hand", returning 1, 0 or -1 and
// removing an attached node from the scene; otherwise report the trigger's id),
// attach a script object's scene node to the calling soldier
// (CSoldierObject::AttachObject, to "lt_hand" for mode 0 or "rt_hand" for mode
// 1) and remove it from the scene, detach the first player from a prop (the
// given script object's, otherwise the running script object's) and reset a
// float of the player (+4660) to 1,
// detach a scene node (a given script object's, otherwise the calling node)
// from a prop, attach the first player to a prop (the given script object's,
// otherwise the running script object's), detach a node from a waypoint
// (restoring collision id 5 from 27), the weak CAnimObject::SetCollisionId (the
// id at +9108), attach a node to a waypoint's trigger position and rotation
// (switching collision id 5 to 27) and attach a node to a prop as a player,
// animated or static object. Each reads its arguments below the script stack
// top and pops the built-in's arguments. The file name is this project's; the
// original record is bsbifunc.cpp and the built-ins around these are not
// reconstructed. The functions, classes and globals are named by the mangled
// symbols; ISceneNode is declared with its virtual functions in the order of
// __vt__10ISceneNode (GetCollisionId at +88, SetCollisionId at +92,
// AsStaticObject at +116, AsAnimObject at +140, AsPlayerObject at +156) and
// BSGO_Basic in the order of __vt__10BSGO_Basic; CSoldierObject is a view whose
// virtual AttachObject is at +308 of __vt__14CSoldierObject, with placeholders
// for the entries before it; CAnimObject and the CPlayerObject view derive from
// ISceneNode (CAnimObject's destructor declared first and defined elsewhere),
// the prop's scene node is used as the CAttachableObject, CMatrix's constructor
// is an inline view (initialising the class once), and the player field, the
// trigger, script-object, scene and built-in record views, the vector view
// (four floats, 8-byte aligned) and the boolean argument helper are inferred.
enum EClsnId {};
class CStaticObject;
class CAnimObject;
#include "game/CVector3.h"


#include "game/CMatrix.h"


class CQuaternion {
public:
    void GetMatrix(CMatrix&) const;

    float x;
    float y;
    float z;
    float w;
};

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
    virtual CStaticObject* AsStaticObject();
    virtual const void* AsStaticObject() const;
    virtual void* AsHierObject();
    virtual const void* AsHierObject() const;
    virtual void* AsWorldObject();
    virtual const void* AsWorldObject() const;
    virtual CAnimObject* AsAnimObject();
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


class CAnimObject : public ISceneNode {
public:
    virtual ~CAnimObject();
    virtual void SetCollisionId(EClsnId);
    void SetAttached(bool);
    void AttachToLocation(const CMatrix&);

    unsigned char unknown0004[9104];
    EClsnId m_collisionId;
};

class CPlayerObject : public ISceneNode {
public:
    unsigned char unknown0004[4656];
    float m_unknown1234;
};

class CAttachableObject {
public:
    void DetachObject(ISceneNode*);
    void AttachObject(CPlayerObject*, int, bool);
    void AttachObject(CAnimObject*, int);
    void AttachObject(CStaticObject*, int);
};

class BSGO_Basic {
    unsigned char unknown00[12];

public:
    virtual void Destroy();
    virtual int GetScriptData();
    virtual ISceneNode* GetSceneNode();
};

struct TriggerCoreView {
    unsigned char unknown00[16];
    float x;
    float y;
    float z;
    CQuaternion rotation;
    unsigned char unknown2c[12];
    short id;
};

struct TriggerObject_struct {
    unsigned char unknown00[4];
    TriggerCoreView* core;
};

struct BSObjectView {
    unsigned char unknown00[8];
    TriggerObject_struct* trigger;
    BSGO_Basic* user;
};

#include "game/CScene.h"


extern CScene g_scene;
extern BSObjectView* g_pBSObject;

class CSoldierObject {
public:
    virtual void unknown008();
    virtual void unknown00c();
    virtual void unknown010();
    virtual void unknown014();
    virtual void unknown018();
    virtual void unknown01c();
    virtual void unknown020();
    virtual void unknown024();
    virtual void unknown028();
    virtual void unknown02c();
    virtual void unknown030();
    virtual void unknown034();
    virtual void unknown038();
    virtual void unknown03c();
    virtual void unknown040();
    virtual void unknown044();
    virtual void unknown048();
    virtual void unknown04c();
    virtual void unknown050();
    virtual void unknown054();
    virtual void unknown058();
    virtual void unknown05c();
    virtual void unknown060();
    virtual void unknown064();
    virtual void unknown068();
    virtual void unknown06c();
    virtual void unknown070();
    virtual void unknown074();
    virtual void unknown078();
    virtual void unknown07c();
    virtual void unknown080();
    virtual void unknown084();
    virtual void unknown088();
    virtual void unknown08c();
    virtual void unknown090();
    virtual void unknown094();
    virtual void unknown098();
    virtual void unknown09c();
    virtual void unknown0a0();
    virtual void unknown0a4();
    virtual void unknown0a8();
    virtual void unknown0ac();
    virtual void unknown0b0();
    virtual void unknown0b4();
    virtual void unknown0b8();
    virtual void unknown0bc();
    virtual void unknown0c0();
    virtual void unknown0c4();
    virtual void unknown0c8();
    virtual void unknown0cc();
    virtual void unknown0d0();
    virtual void unknown0d4();
    virtual void unknown0d8();
    virtual void unknown0dc();
    virtual void unknown0e0();
    virtual void unknown0e4();
    virtual void unknown0e8();
    virtual void unknown0ec();
    virtual void unknown0f0();
    virtual void unknown0f4();
    virtual void unknown0f8();
    virtual void unknown0fc();
    virtual void unknown100();
    virtual void unknown104();
    virtual void unknown108();
    virtual void unknown10c();
    virtual void unknown110();
    virtual void unknown114();
    virtual void unknown118();
    virtual void unknown11c();
    virtual void unknown120();
    virtual void unknown124();
    virtual void unknown128();
    virtual void unknown12c();
    virtual void unknown130();
    virtual bool AttachObject(CStaticObject*, char*, int);
};

void DebugMsg(const char*, ...);

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

inline int BSArgBool(int** stack, int index) {
    return *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - index)) != 0;
}

void BIFunc_AttachToSoldierEmptyHand(int** stack, void* object) {
    BSValueView value;
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    value.i = -1;
    BSObjectView* prop = *(BSObjectView**)(*stack - (count - 1));
    int arg = *(*stack - (count - 2));
    CSoldierObject* soldier = (CSoldierObject*)object;
    ISceneNode* node = prop->user->GetSceneNode();
    if (!g_scene.IsNodeInScene(node)) {
        if (prop)
            DebugMsg("Prop %d has already been attached or destroyed. "
                     "Check with Designers for possible duplicate ID.\n",
                     prop->trigger->core->id);
    } else {
        if (!soldier->AttachObject((CStaticObject*)node, "rt_hand", arg)) {
            if (!soldier->AttachObject((CStaticObject*)node, "lt_hand", arg))
                goto done;
            value.i = 0;
        } else {
            value.i = 1;
        }
        g_scene.Remove(*node);
    }
done:
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    *(BSValueView*)*stack = value;
}

void BIFunc_AttachToSoldier(int** stack, void* object) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    BSObjectView* prop = *(BSObjectView**)(*stack - (count - 1));
    int mode = *(*stack - (count - 2));
    int arg = *(*stack - (count - 3));
    CSoldierObject* soldier = (CSoldierObject*)object;
    ISceneNode* node = prop->user->GetSceneNode();
    switch (mode) {
    case 0:
        soldier->AttachObject((CStaticObject*)node, "lt_hand", arg);
        break;
    case 1:
        soldier->AttachObject((CStaticObject*)node, "rt_hand", arg);
        break;
    }
    g_scene.Remove(*node);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_DetachPlayerFromProp(int** stack, void*) {
    BSObjectView* prop = *(BSObjectView**)(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    if (!prop)
        prop = g_pBSObject;
    CAttachableObject* attachable = (CAttachableObject*)prop->user->GetSceneNode();
    CPlayerObject* player = g_scene.GetPlayer(0);
    attachable->DetachObject(player);
    player = g_scene.GetPlayer(0);
    player->m_unknown1234 = 1.0f;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_DetachFromProp(int** stack, void* object) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    BSObjectView* other = *(BSObjectView**)(*stack - (count - 1));
    CAttachableObject* prop = (CAttachableObject*)(*(BSObjectView**)(*stack - (count - 2)))->user->GetSceneNode();
    ISceneNode* node;
    if (other)
        node = other->user->GetSceneNode();
    else
        node = (ISceneNode*)object;
    prop->DetachObject(node);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_AttachPlayerToProp(int** stack, void*) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    int slot = *(*stack - (count - 1));
    bool flag = BSArgBool(stack, 2);
    BSObjectView* prop = *(BSObjectView**)(*stack - (count - 3));
    if (!prop)
        prop = g_pBSObject;
    CAttachableObject* attachable = (CAttachableObject*)prop->user->GetSceneNode();
    CPlayerObject* player = g_scene.GetPlayer(0);
    attachable->AttachObject(player, slot, flag);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_DetachFromWaypoint(int** stack, void* object) {
    BSObjectView* other = *(BSObjectView**)(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    ISceneNode* node;
    if (other)
        node = other->user->GetSceneNode();
    else
        node = (ISceneNode*)object;
    CAnimObject* anim = node->AsAnimObject();
    if (anim->GetCollisionId() == 27)
        anim->SetCollisionId((EClsnId)5);
    anim->SetAttached(false);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

__declspec(weak) void CAnimObject::SetCollisionId(EClsnId id) {
    m_collisionId = id;
}

void BIFunc_AttachToWaypoint(int** stack, void* object) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    BSObjectView* other = *(BSObjectView**)(*stack - (count - 1));
    BSObjectView* waypoint = *(BSObjectView**)(*stack - (count - 2));
    ISceneNode* node;
    if (other)
        node = other->user->GetSceneNode();
    else
        node = (ISceneNode*)object;
    CAnimObject* anim = node->AsAnimObject();
    CMatrix matrix;
    if (waypoint) {
        TriggerCoreView* core = waypoint->trigger->core;
        CVector3 position(core->x, core->y, core->z);
        core->rotation.GetMatrix(matrix);
        matrix.SetPos(position);
        if (anim->GetCollisionId() == 5)
            anim->SetCollisionId((EClsnId)27);
        anim->AttachToLocation(matrix);
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_AttachToProp(int** stack, void* object) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    BSObjectView* other = *(BSObjectView**)(*stack - (count - 1));
    int slot = *(*stack - (count - 2));
    bool flag = BSArgBool(stack, 3);
    BSObjectView* prop = *(BSObjectView**)(*stack - (count - 4));
    ISceneNode* node;
    if (other)
        node = other->user->GetSceneNode();
    else
        node = (ISceneNode*)object;
    CAttachableObject* attachable = (CAttachableObject*)prop->user->GetSceneNode();
    bool isAnim = node->AsAnimObject() != 0;
    bool isPlayer = node->AsPlayerObject() != 0;
    if (isPlayer)
        attachable->AttachObject(node->AsPlayerObject(), slot, flag);
    else if (isAnim)
        attachable->AttachObject(node->AsAnimObject(), slot);
    else
        attachable->AttachObject(node->AsStaticObject(), slot);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
