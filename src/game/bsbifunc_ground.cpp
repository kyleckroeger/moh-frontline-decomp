// A fragment of bsbifunc.cpp (0x80031a00): built-ins that induce a ground
// check (an event 0x85 sent to the soldier's script object when the soldier's
// flag is set), and set the animated object's ground-check and gravity flags.
// Each reads its argument below the script stack top and pops the built-in's
// arguments. The file name is this project's; the original record is
// bsbifunc.cpp and the built-ins around these are not reconstructed. The
// functions, classes and globals are named by the mangled symbols; ISceneNode
// is declared with its virtual functions in the order of __vt__10ISceneNode
// (AsAnimObject at +140, AsSoldierObject at +148), and the soldier and
// animated object fields are inferred.
class CCollision;
class CDrawContext;
class CMatrix;
class CVector3;
class CBullet;
class CLight;
struct BSObject;
class CPlayerObject;
class CAnimObject;
class CSoldierObject;
struct AIDoodadView;

#include "game/ISceneNode.h"


class CSoldierObject {
public:
    unsigned char unknown0000[9104];
    BSObject* m_script;
    unsigned char unknown2394[192];
    bool m_groundCheckPending;
};

class CAnimObject {
public:
    unsigned char unknown0000[9394];
    bool m_gravity;
    bool m_groundCheck;
};

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

void BIFunc_InduceGroundCheck(int** stack, void* object) {
    CSoldierObject* soldier = ((ISceneNode*)object)->AsSoldierObject();
    if (soldier->m_groundCheckPending)
        BSObjectTriggerEvent(soldier->m_script, 0x85, 0, 0, true);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_SetGroundCheck(int** stack, void* object) {
    int enable = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    ((ISceneNode*)object)->AsAnimObject()->m_groundCheck = enable != 0;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_SetGravity(int** stack, void* object) {
    int enable = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    ((ISceneNode*)object)->AsAnimObject()->m_gravity = enable != 0;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
