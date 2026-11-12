// A fragment of bsbifunc.cpp (0x8002f060): script built-ins that report whether
// a child can be created (the German creation queue empty and fewer active
// soldiers than the maximum), destroy an object by the script trigger's type (a
// scene node marked for destruction for type 14, a particle system deactivated
// for type 9, otherwise the given or the running script object's user object
// destroyed later), create a child by the trigger's type (the created object
// for types 9 and 14, its first word otherwise; checked for type 3), then
// hierarchy-object built-ins that destroy a sub-object (or, for id 0, schedule
// the script object's own destruction), swap two sub-objects and create a child
// sub-object with an attachment. Each reads its arguments below the script
// stack top and pops the built-in's arguments. The file name is this project's;
// the original record is bsbifunc.cpp and the built-ins around these are not
// reconstructed. The functions, classes and globals are named by the mangled
// symbols; ISceneNode is declared with its virtual functions in the order of
// __vt__10ISceneNode (MarkForDestruction at +8, AsHierObject at +124) and
// CParticleSystem with its first virtual functions in the order of
// __vt__15CParticleSystem (DeActivate at +28, the pointer at +28; the base
// class's functions are folded in and their parameters omitted); the trigger,
// script-object and built-in record views are inferred.
class CCollision;
class CDrawContext;
class CMatrix;
class CVector3;
class CBullet;
class CLight;
class CPlayerObject;
class CHierObject;
struct AIDoodadView;

#include "game/ISceneNode.h"


enum DWI_ATTACHMENT_CRC_ENUM {};
class BSGO_Basic;

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
    BSGO_Basic* user;
};

class CParticleSystem {
    unsigned char unknown00[28];

public:
    virtual ~CParticleSystem();
    virtual void Render();
    virtual void Init();
    virtual void Link();
    virtual int IsUsed();
    virtual void DeActivate();
};

extern int g_iGermanCreationQueueHeadIndex;
extern int g_iGermanCreationQueueTailIndex;
extern int g_numActiveSoldiers;
extern int g_maxActiveSoldiers;

int* CreateObject(TriggerObject_struct*, int, void*);

extern BSObjectView* g_pBSObject;

void DelayDestroyObject(BSGO_Basic*);
void DestroySubHierObject(CHierObject*, int);
void SwapHierObject(CHierObject*, int, int);
void CreateSubHierObject(CHierObject*, int, int, DWI_ATTACHMENT_CRC_ENUM);

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

void BIFunc_CanICreateChild(int** stack, void*) {
    bool can = false;
    if (g_iGermanCreationQueueHeadIndex == g_iGermanCreationQueueTailIndex && g_numActiveSoldiers < g_maxActiveSoldiers)
        can = true;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = can;
}

void BIFunc_DestroyObject(int** stack, void*) {
    int target = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    switch (g_pBSObject->trigger->core->type) {
    case 14:
        ((ISceneNode*)target)->MarkForDestruction(0);
        break;
    case 9:
        ((CParticleSystem*)target)->DeActivate();
        break;
    default: {
        BSObjectView* script = (BSObjectView*)target;
        if (!script)
            script = g_pBSObject;
        if (script->user)
            DelayDestroyObject(script->user);
        break;
    }
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_CreateChild(int** stack, void*) {
    int value = 0;
    int argument = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    switch (g_pBSObject->trigger->core->type) {
    case 9:
    case 14:
        value = (int)CreateObject(g_pBSObject->trigger, argument, 0);
        break;
    case 3: {
        int* created = CreateObject(g_pBSObject->trigger, argument, 0);
        if (created)
            value = *created;
        break;
    }
    default:
        value = *CreateObject(g_pBSObject->trigger, argument, 0);
        break;
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = *(int*)&value;
}

void BIFunc_DestroySubHierObject(int** stack, void* object) {
    int id = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    CHierObject* hier = ((ISceneNode*)object)->AsHierObject();
    if (id == 0)
        DelayDestroyObject(g_pBSObject->user);
    else
        DestroySubHierObject(hier, id);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_SwapHierObject(int** stack, void* object) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    int first = *(*stack - (count - 1));
    int second = *(*stack - (count - 2));
    CHierObject* hier = ((ISceneNode*)object)->AsHierObject();
    SwapHierObject(hier, first, second);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_CreateChildSubHierObject(int** stack, void* object) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    int parent = *(*stack - (count - 1));
    int child = *(*stack - (count - 2));
    int attachment = *(*stack - (count - 3));
    CHierObject* hier = ((ISceneNode*)object)->AsHierObject();
    CreateSubHierObject(hier, parent, child, (DWI_ATTACHMENT_CRC_ENUM)attachment);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
