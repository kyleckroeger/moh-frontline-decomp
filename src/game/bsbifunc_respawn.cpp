// A fragment of bsbifunc.cpp (0x8002ff20): built-ins that respawn the
// player, set the first player's falling-damage state and return whether the
// player is crouching (in multiplayer the script object's player, through its
// scene node; otherwise the first player). Each pops the built-in's
// arguments. The file name is this project's; the original record is
// bsbifunc.cpp and the built-ins around these are not reconstructed. The
// functions, classes and globals are named by the mangled symbols; ISceneNode
// is declared with its virtual functions in the order of __vt__10ISceneNode
// and BSGO_Basic in the order of __vt__10BSGO_Basic, and the scene and
// built-in record views and the argument helper are inferred.
class CCollision;
class CDrawContext;
class CMatrix;
class CVector3;
class CBullet;
class CLight;
struct BSObject;
class CPlayerObject;
struct AIDoodadView;

#include "game/ISceneNode.h"


#include "game/CPlayerObject.h"


#include "game/CScene.h"


// BSGO_Basic, the object at BSObject+12: 12 bytes of members, then its
// virtual table pointer; the virtual functions it needs, in the order of
// __vt__10BSGO_Basic.
class BSGO_Basic {
    unsigned char unknown00[12];

public:
    virtual void Destroy();
    virtual void* GetScriptData();
    virtual ISceneNode* GetSceneNode();
};

struct BSObjectView {
    unsigned char unknown00[12];
    BSGO_Basic* user;
};

extern CScene g_scene;
extern bool g_bInMultiplayerMode;
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

inline int BSArgBool(int** stack, int index) {
    return *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - index)) != 0;
}

void BIFunc_PlayerRespawn(int** stack, void* object) {
    ((ISceneNode*)object)->AsPlayerObject()->Respawn();
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_SetPlayerFallingDamage(int** stack, void*) {
    bool enable = BSArgBool(stack, 1);
    g_scene.GetPlayer(0)->SetFallingDamageState(enable);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_IsPlayerCrouching(int** stack, void*) {
    CPlayerObject* player;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    if (g_bInMultiplayerMode)
        player = g_pBSObject->user->GetSceneNode()->AsPlayerObject();
    else
        player = g_scene.GetPlayer(0);
    **stack = player->IsPlayerCrouching();
}
