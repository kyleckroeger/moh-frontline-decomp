// A fragment of bsbifunc.cpp (0x8002dc48): BIFunc_PlayerStartGrenadeCook,
// which starts the script object's player cooking a grenade and pops the
// built-in's arguments. The file name is this project's; the original record
// is bsbifunc.cpp. The grenade-hold built-ins after it stop the player's
// grenade hold (time -1) and start it (from 0 when it was stopped); their
// constants are entries of the file's .sdata2 pool. ISceneNode is declared
// with its virtual functions in the order of __vt__10ISceneNode
// (AsPlayerObject at +156); the built-in record view is the one bsbifunc.cpp
// uses.
class CCollision;
class CDrawContext;
class CMatrix;
class CVector3;
class CBullet;
class CLight;
class CPlayerObject;

#include "game/ISceneNode.h"


#include "game/CPlayerObject.h"


struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    unsigned char unknown0c[4];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

void BIFunc_PlayerStartGrenadeCook(int** stack, void* object) {
    ((ISceneNode*)object)->AsPlayerObject()->StartGrenadeCook();
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_StopGrenadeHold(int** stack, void* object) {
    CPlayerObject* player = ((ISceneNode*)object)->AsPlayerObject();
    player->m_grenadeHoldTime = -1.0f;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_StartGrenadeHold(int** stack, void* object) {
    CPlayerObject* player = ((ISceneNode*)object)->AsPlayerObject();
    if (player->m_grenadeHoldTime < 0.0f)
        player->m_grenadeHoldTime = 0.0f;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
