// A fragment of bsbifunc.cpp (0x80030c0c): the player reload built-ins. Each
// takes the script object's player (ISceneNode::AsPlayerObject) and its
// current weapon, then finishes reloading, clears the reloading bit, reloads
// a single round or starts reloading, and pops the built-in's arguments from
// the script stack. The file name is this project's; the original record is
// bsbifunc.cpp and the functions around these are not reconstructed. The
// classes, functions and globals are named by the mangled symbols; ISceneNode
// is declared with its virtual functions in the order of __vt__10ISceneNode
// (as in the propdat.cpp fragments), CWeapon is an inferred view (the
// reloading bit in the flag byte at +704, cleared through an inferred inline
// helper), and the built-in record view is
// the one bsbifunc.cpp uses.
class CCollision;
class CDrawContext;
class CMatrix;
class CVector3;
class CBullet;
class CLight;
class CPlayerObject;

#include "game/ISceneNode.h"


class CWeapon {
public:
    void DoneReloading();
    void ReloadSingle();
    void StartReloading();
    void ClearReloading() { reloading = 0; }

    unsigned char unknown000[704];
    unsigned char reloading : 1;
    unsigned char flags : 7;
};

#include "game/CPlayerObject.h"


struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    unsigned char unknown0c[4];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

void BIFunc_PlayerFinishedReloading(int** stack, void* object) {
    ((ISceneNode*)object)->AsPlayerObject()->GetCurrentWeapon()->DoneReloading();
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_PlayerClearReloading(int** stack, void* object) {
    ((ISceneNode*)object)->AsPlayerObject()->GetCurrentWeapon()->ClearReloading();
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_PlayerReloadSingle(int** stack, void* object) {
    ((ISceneNode*)object)->AsPlayerObject()->GetCurrentWeapon()->ReloadSingle();
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_PlayerStartReloading(int** stack, void* object) {
    ((ISceneNode*)object)->AsPlayerObject()->GetCurrentWeapon()->StartReloading();
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
