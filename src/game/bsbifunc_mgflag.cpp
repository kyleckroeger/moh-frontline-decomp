// A fragment of bsbifunc.cpp (0x800213e8): ApplyForceToObject, which applies
// a force along the calling object's forward direction (normalised, lifted
// by a random 0.5..0.65 for mode 0, whose target defaults to the player, or
// by 0.5 + (rand() % 100) / 500 for mode 4; mode 2 does nothing) to a moving
// scene node through IMovingSceneNode::ApplyForceTo, then built-ins that
// force the player to a weapon (cycling to it) and set or clear machine-gun
// waypoint flags in the script object's trigger. The mode, target and force
// are the top three stack words; the empty case 2 and the case order are as
// the dispatch shows. Each reads its arguments below the script
// stack top and pops the built-in's arguments. The file name is this
// project's; the original record is bsbifunc.cpp. GetBSObject after these
// returns a scene node's script object when its second argument is 0 (a
// one-case switch, as the branch shape shows). The functions, classes and
// globals are named by the mangled symbols; ISceneNode is declared with its
// virtual functions in the order of __vt__10ISceneNode, and the trigger,
// player and built-in record views are inferred.
class CCollision;
class CDrawContext;
class CMatrix;



#include "game/CVector3.h"

class CBullet;
class CLight;
struct BSObject;
class CPlayerObject;
struct AIDoodadView;

#include "game/ISceneNode.h"


struct TriggerFlagsView {
    unsigned int flags;
};

struct BSObjectView {
    unsigned char unknown00[8];
    TriggerFlagsView* trigger;
};

#include "game/CPlayerObject.h"


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



#include "game/CScene.h"


extern CScene g_scene;
extern "C" int rand();
float MathFunRandomReal(float, float);

void BIFunc_ApplyForceToObject(int** stack, void* object) {
    int* args = *stack;
    int mode = args[-2];
    IMovingSceneNode* target = (IMovingSceneNode*)args[-1];
    float force = *(float*)args;
    CVector3 direction;

    ((ISceneNode*)object)->GetForward(direction);
    switch (mode) {
    case 2:
        break;
    case 4:
        direction.Normalize();
        direction.z += 0.5f + (rand() % 100) / 500.0f;
        target->ApplyForceTo(direction, force);
        break;
    case 0:
        if (!target)
            target = (IMovingSceneNode*)g_scene.GetPlayer(0);
        direction.Normalize();
        direction.z += MathFunRandomReal(0.5f, 0.65f);
        target->ApplyForceTo(direction, force);
        break;
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_PlayerForceToWeapon(int** stack, void* object) {
    int weapon = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    ((ISceneNode*)object)->AsPlayerObject()->CycleWeapon(1, weapon);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_SetMGWaypointFlag(int** stack, void*) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    TriggerFlagsView* trigger = g_pBSObject->trigger;
    unsigned int flag = *(*stack - (count - 1));
    if (*(*stack - (count - 2)) != 0)
        trigger->flags |= flag;
    else
        trigger->flags &= flag ^ 0xffffffff;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_GetBSObject(int** stack, void*);

void BIFunc_GetBSObject(int** stack, void*) {
    BSObject* object = 0;
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    switch (*(*stack - (count - 2))) {
    case 0:
        object = ((ISceneNode*)*(*stack - (count - 1)))->GetScriptObject();
        break;
    }
    **stack = *(int*)&object;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
