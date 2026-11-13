// A fragment of bsbifunc.cpp (0x80030db8): script built-ins that fire the
// player's weapon, set the player's wielding flag, test whether the current
// weapon fires projectiles, and send messages and events (to a given object
// unless it is null or -1, to an object immediately, after a delay through a
// timer event, to the parent object or to the script object itself). Each
// reads its arguments below the script stack top (by the built-in's parameter
// count; the parent and self forms read the top two values) and pops them.
// The file name is this project's; the original record is bsbifunc.cpp and
// the built-ins around these are not reconstructed. The functions, classes
// and globals are named by the mangled symbols; ISceneNode is declared with
// its virtual functions in the order of __vt__10ISceneNode, and the player,
// weapon, trigger and built-in record views are inferred (members at their
// offsets, names not original).
class CCollision;
class CDrawContext;
class CMatrix;
class CVector3;
class CBullet;
class CLight;
class CPlayerObject;
struct AIDoodadView;

#include "game/ISceneNode.h"


struct BSObject;
enum ETimerReplaceMethod {};

struct TriggerObject_struct {
    unsigned char unknown00[8];
    BSObject* object;
};

struct BSObjectView {
    unsigned char unknown00[8];
    TriggerObject_struct* trigger;
};

struct WeaponPropertiesView {
    unsigned char unknown00[14];
    short projectile;
};

class CWeapon {
public:
    unsigned char unknown000[640];
    WeaponPropertiesView* m_properties;
};

#include "game/CPlayerObject.h"


extern BSObjectView* g_pBSObject;

void BSSendMessage(unsigned short, BSObject*, TriggerObject_struct*, BSObject*, void*);
void BSRegisterTimerEvent(int, unsigned short, BSObject*, void*, ETimerReplaceMethod);
void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

void BIFunc_PlayerFireMyWeapon(int** stack, void* object) {
    ((ISceneNode*)object)->AsPlayerObject()->FireMyWeapon(*(float*)(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1)));
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_SetPlayerWielding(int** stack, void* object) {
    CPlayerObject* player = ((ISceneNode*)object)->AsPlayerObject();
    player->m_wielding = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1)) != 0;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_PlayerIsMyWeaponAProjectile(int** stack, void* object) {
    CPlayerObject* player = ((ISceneNode*)object)->AsPlayerObject();
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = player->GetCurrentWeapon()->m_properties->projectile != 0;
}

void BIFunc_SendMessage(int** stack, void*) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    BSSendMessage(*(*stack - (count - 1)), (BSObject*)g_pBSObject, g_pBSObject->trigger, (BSObject*)g_pBSObject,
                  (void*)*(*stack - (count - 2)));
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_SendEventDelayed(int** stack, void*) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    BSRegisterTimerEvent(*(*stack - (count - 1)), *(*stack - (count - 3)), (BSObject*)*(*stack - (count - 2)),
                         (void*)*(*stack - (count - 4)), (ETimerReplaceMethod)1);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_SendEventImmediate(int** stack, void*) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    BSObjectTriggerEvent((BSObject*)*(*stack - (count - 1)), *(*stack - (count - 2)), (void*)*(*stack - (count - 3)),
                         (BSObject*)g_pBSObject, true);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_SendEvent(int** stack, void*) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    BSObject* target = (BSObject*)*(*stack - (count - 1));
    int event = *(*stack - (count - 2));
    void* data = (void*)*(*stack - (count - 3));
    if (target && target != (BSObject*)-1)
        BSObjectTriggerEvent(target, event, data, (BSObject*)g_pBSObject, false);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_SendEventParent(int** stack, void*) {
    BSObjectTriggerEvent(g_pBSObject->trigger->object, (*stack)[-1], (void*)(*stack)[0], (BSObject*)g_pBSObject, false);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_SendEventSelf(int** stack, void*) {
    BSObjectTriggerEvent((BSObject*)g_pBSObject, (*stack)[-1], (void*)(*stack)[0], (BSObject*)g_pBSObject, false);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
