// A fragment of bsbifunc.cpp (0x8002dd88): BIFunc_GetGrenadeHoldTime, which
// pops the built-in's arguments and writes the player's grenade hold time
// (+1112, an inferred member) to the top of the script stack as its bit
// pattern. The file name is this project's; the original record is
// bsbifunc.cpp and the built-ins around it are not reconstructed.
// ISceneNode is declared with its virtual functions in the order of
// __vt__10ISceneNode (AsPlayerObject at +156); the built-in record view is the
// one bsbifunc.cpp uses.
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

void BIFunc_GetGrenadeHoldTime(int** stack, void* object) {
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    float time = ((ISceneNode*)object)->AsPlayerObject()->m_grenadeHoldTime;
    **stack = *(int*)&time;
}
