// A fragment of bsbifunc.cpp (0x800307dc): player built-ins that start a motion
// shake, allow or forbid crouching (a flag bit of the player) and stop or start
// a camera shake (on the script object's player in multiplayer, otherwise the
// first player). Each reads its arguments below the script stack top and pops
// the built-in's arguments. The file name is this project's; the original
// record is bsbifunc.cpp; PlayerScreenFlash before these uses pooled constants
// and is not reconstructed. The functions, classes and globals are named by the
// mangled symbols; ISceneNode is declared with its virtual functions in the
// order of __vt__10ISceneNode (AsPlayerObject at +156) and BSGO_Basic in the
// order of __vt__10BSGO_Basic; the player, scene and built-in record views are
// inferred (members at their offsets, names not original).
class CCollision;
class CDrawContext;
class CMatrix;
class CVector3;
class CBullet;
class CLight;
class CPlayerObject;
struct AIDoodadView;

#include "game/ISceneNode.h"



class BSGO_Basic {
    unsigned char unknown00[12];

public:
    virtual void Destroy();
    virtual int GetScriptData();
    virtual ISceneNode* GetSceneNode();
};

struct BSObjectView {
    unsigned char unknown00[12];
    BSGO_Basic* user;
};

#include "game/CPlayerObject.h"


#include "game/CScene.h"


extern CScene g_scene;
extern BSObjectView* g_pBSObject;
extern bool g_bInMultiplayerMode;

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

void BIFunc_PlayerMotionShake(int** stack, void*) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    float strength = *(float*)(*stack - (count - 1));
    float time = *(float*)(*stack - (count - 2));
    CPlayerObject* player = g_scene.GetPlayer(0);
    if (player)
        player->DoMotionShake(strength, time);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_PlayerCanCrouch(int** stack, void*) {
    bool canCrouch = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1)) != 0;
    CPlayerObject* player = g_scene.GetPlayer(0);
    if (player)
        player->m_canCrouch = canCrouch;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_PlayerStopCameraShake(int** stack, void*) {
    float time = *(float*)(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    CPlayerObject* player = g_scene.GetPlayer(0);
    if (g_bInMultiplayerMode && g_pBSObject && g_pBSObject->user && g_pBSObject->user->GetSceneNode() && g_pBSObject->user->GetSceneNode()->AsPlayerObject())
        player = g_pBSObject->user->GetSceneNode()->AsPlayerObject();
    if (player)
        player->StopCameraShake(time);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_PlayerStartCameraShake(int** stack, void*) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    float strength = *(float*)(*stack - (count - 1));
    float time = *(float*)(*stack - (count - 2));
    CPlayerObject* player = g_scene.GetPlayer(0);
    if (g_bInMultiplayerMode && g_pBSObject && g_pBSObject->user && g_pBSObject->user->GetSceneNode() && g_pBSObject->user->GetSceneNode()->AsPlayerObject())
        player = g_pBSObject->user->GetSceneNode()->AsPlayerObject();
    if (player)
        player->StartCameraShake(strength, time);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
