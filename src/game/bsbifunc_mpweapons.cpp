// A fragment of bsbifunc.cpp (0x8001f000): multiplayer weapon-set built-ins.
// Adding ammunition or a weapon gives the given script object's player the
// script trigger's item CRC and amount, returning whether it was taken (kept in
// memory before it is written). The weapon type and the ammo and weapon pickup
// prompts come from the player weapon table for the CRC in the given weapon-set
// slot; creating ammo or a weapon by type looks the script object's trigger CRC
// up in the set (the ammo entries 19 slots on, the weapons one slot on) and
// creates the object from the first match, returning it (or 0); creating ammo
// or a weapon by weapon-set id (1-based, checked against the set's count) sets
// the trigger's CRC from that slot (ammo 18 slots on) and creates the object;
// resetting a mounted machine gun finds the scene node (a static object in the
// box list or a hierarchy object in the tank list) whose script object has the
// given script object's trigger and sets its transform from the trigger's
// rotation and position. Each reads its argument below the script stack top,
// pops the built-in's arguments and writes the result to the new top through an
// integer union. The file name is this project's; the original record is
// bsbifunc.cpp and the built-ins around these are not reconstructed. The
// functions, classes and globals are named by the mangled symbols; ISceneNode
// is declared with its virtual functions in the order of __vt__10ISceneNode and
// IMovingSceneNode's after them in the order of __vt__13CStaticObject
// (SetTMLocalToWorld at +232), BSGO_Basic in the order of __vt__10BSGO_Basic
// (the list link at +4), CMatrix's constructor is an inline view (initialising
// the class once), the node search is an inferred inline helper, and the
// weapon-set, trigger, script-object and built-in record views and the argument
// helper are inferred (members at their offsets, names not original).
#include "game/CVector3.h"


class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    void SetPos(CVector3);

    float m[4][4];
    static bool s_ClassInit;
} __attribute__((aligned(16)));

class CQuaternion {
public:
    void GetMatrix(CMatrix&) const;

    float x;
    float y;
    float z;
    float w;
};

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

union BSValueView {
    int i;
    float f;
};

struct MPWeaponSlotView {
    int crc;
    int unknown04;
};

struct MPWeaponSetDataView {
    unsigned char unknown000[12];
    int count;
    unsigned char unknown010[140];
    MPWeaponSlotView slots[40];
};

struct MPWeaponSetView {
    unsigned char unknown00[8];
    MPWeaponSetDataView* data;
};

struct TriggerObject_struct;

struct TriggerCoreView {
    unsigned char unknown00[16];
    float x;
    float y;
    float z;
    CQuaternion rotation;
    unsigned char unknown2c[28];
    int crc;
    unsigned char unknown4c[4];
    int createArgument;
};

struct TriggerObject_struct {
    unsigned char unknown00[4];
    TriggerCoreView* core;
};

class BSGO_Basic;

struct BSObjectView {
    unsigned char unknown00[8];
    TriggerObject_struct* trigger;
    BSGO_Basic* user;
};

class BSGO_Basic {
    unsigned char unknown00[4];

public:
    BSGO_Basic* next;
    unsigned char unknown08[4];
    virtual void Destroy();
    virtual int GetScriptData();
    virtual ISceneNode* GetSceneNode();
};

extern BSGO_Basic* g_pBoxList;
extern BSGO_Basic* g_pTankList;

class CPlayerObject {
public:
    static int GetWeaponInfoByCRC(int, int*, int*);
    bool AddAmmoOrWeapon(int, int, void*);
};

int* CreateObject(TriggerObject_struct*, int, void*);

extern MPWeaponSetView* g_pMPWeaponSet;
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

void BIFunc_MPAddPlayerAmmo(int** stack, void*) {
    BSObjectView* other = (BSObjectView*)BSArgInt(stack, 1);
    int amount = BSArgInt(stack, 2);
    CPlayerObject* player = other->user->GetSceneNode()->AsPlayerObject();
    int result = player->AddAmmoOrWeapon(g_pBSObject->trigger->core->crc, amount, 0);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = *(int*)&result;
}

void BIFunc_MPGetWeaponTypeFromWeaponSetID(int** stack, void*) {
    BSValueView value;
    int slot = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    value.i = CPlayerObject::GetWeaponInfoByCRC(g_pMPWeaponSet->data->slots[slot].crc, 0, 0);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    *(BSValueView*)*stack = value;
}

void BIFunc_MPGetAmmoPickupPromptFromWeaponSetID(int** stack, void*) {
    int slot = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    int weaponPrompt = 0;
    int ammoPrompt = 0;
    CPlayerObject::GetWeaponInfoByCRC(g_pMPWeaponSet->data->slots[slot].crc, &weaponPrompt, &ammoPrompt);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = ammoPrompt;
}

void BIFunc_MPGetWeaponPickupPromptFromWeaponSetID(int** stack, void*) {
    int slot = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    int weaponPrompt = 0;
    int ammoPrompt = 0;
    CPlayerObject::GetWeaponInfoByCRC(g_pMPWeaponSet->data->slots[slot].crc, &weaponPrompt, &ammoPrompt);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = weaponPrompt;
}

void BIFunc_MPCreateAmmoByType(int** stack, void*) {
    BSValueView value;
    int count = g_pMPWeaponSet->data->count;
    TriggerObject_struct* trigger = g_pBSObject->trigger;
    TriggerCoreView* core = trigger->core;
    int crc = core->crc;
    value.i = 0;
    for (int i = 0; i < count; i++) {
        if (crc == g_pMPWeaponSet->data->slots[i + 19].crc) {
            value.i = *CreateObject(trigger, core->createArgument, 0);
            break;
        }
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    *(BSValueView*)*stack = value;
}

void BIFunc_MPCreateWeaponByType(int** stack, void*) {
    BSValueView value;
    int count = g_pMPWeaponSet->data->count;
    TriggerObject_struct* trigger = g_pBSObject->trigger;
    TriggerCoreView* core = trigger->core;
    int crc = core->crc;
    value.i = 0;
    for (int i = 0; i < count; i++) {
        if (crc == g_pMPWeaponSet->data->slots[i + 1].crc) {
            value.i = *CreateObject(trigger, core->createArgument, 0);
            break;
        }
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    *(BSValueView*)*stack = value;
}

void BIFunc_MPCreateAmmoByWeaponSetID(int** stack, void*) {
    BSValueView value;
    int slot = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    value.i = 0;
    if (slot > 0 && slot <= g_pMPWeaponSet->data->count) {
        g_pBSObject->trigger->core->crc = g_pMPWeaponSet->data->slots[slot + 18].crc;
        value.i = *CreateObject(g_pBSObject->trigger, g_pBSObject->trigger->core->createArgument, 0);
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    *(BSValueView*)*stack = value;
}

void BIFunc_MPCreateWeaponByWeaponSetID(int** stack, void*) {
    BSValueView value;
    int slot = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    value.i = 0;
    if (slot > 0 && slot <= g_pMPWeaponSet->data->count) {
        g_pBSObject->trigger->core->crc = g_pMPWeaponSet->data->slots[slot].crc;
        value.i = *CreateObject(g_pBSObject->trigger, g_pBSObject->trigger->core->createArgument, 0);
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    *(BSValueView*)*stack = value;
}

inline ISceneNode* FindMMGNode(TriggerObject_struct* trigger) {
    BSGO_Basic* user;
    ISceneNode* node;
    BSObjectView* script;
    for (user = g_pBoxList; user; user = user->next) {
        node = user->GetSceneNode();
        if (node && node->AsStaticObject() && (script = node->GetScriptObject()) && script->trigger == trigger)
            return node;
    }
    for (user = g_pTankList; user; user = user->next) {
        node = user->GetSceneNode();
        if (node && node->AsHierObject() && (script = node->GetScriptObject()) && script->trigger == trigger)
            return node;
    }
    return 0;
}

void BIFunc_ResetMMG(int** stack, void*) {
    TriggerObject_struct* trigger;
    BSObjectView* object = *(BSObjectView**)(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    if (object) {
        trigger = object->trigger;
        ISceneNode* node = FindMMGNode(trigger);
        CMatrix matrix;
        TriggerCoreView* core = node->GetScriptObject()->trigger->core;
        core->rotation.GetMatrix(matrix);
        CVector3 position(core->x, core->y, core->z);
        matrix.SetPos(position);
        ((IMovingSceneNode*)node)->SetTMLocalToWorld(matrix);
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
