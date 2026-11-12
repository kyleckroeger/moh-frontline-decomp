// A fragment of bsbifunc.cpp (0x8002f4cc): script built-ins that create a
// child root hierarchy object (for the script object's trigger), return the
// script data, return a random float or integer in a range, and switch the
// player's weapon or test whether the player can. Each reads its arguments
// below the script stack top (by the built-in's parameter count), pops the
// arguments and, for a result, writes it to the new top (through its address
// where the original keeps the value in memory). The file name is this
// project's; the original record is bsbifunc.cpp. PlayerCanCookGrenades
// after these tests the shell's cook-grenades setting, per player in
// multiplayer (the player index read into a local first, as the register use
// shows). The functions, classes and globals are named by the mangled
// symbols; ISceneNode is declared with its virtual functions in the order of
// __vt__10ISceneNode and BSGO_Basic in the order of __vt__10BSGO_Basic; the
// player and weapon views are inferred (members at their offsets, names not
// original), as are the built-in record view, the integer/float value union
// and the random-range helpers.
class CCollision;
class CDrawContext;
class CMatrix;
class CVector3;
class CBullet;
class CLight;
class CPlayerObject;
struct AIDoodadView;

#include "game/ISceneNode.h"


struct TriggerObject_struct;
struct BSObject;
enum ETimerReplaceMethod {};

union BSValueView {
    int i;
    float f;
};

// BSGO_Basic, the object at BSObject+12: 12 bytes of members, then its
// virtual table pointer; the virtual functions it needs, in the order of
// __vt__10BSGO_Basic.
class BSGO_Basic {
    unsigned char unknown00[12];

public:
    virtual void Destroy();
    virtual int GetScriptData();
};

struct BSObjectView {
    unsigned char unknown00[8];
    TriggerObject_struct* trigger;
    BSGO_Basic* user;
};

class CWeapon {
public:
    unsigned char unknown000[664];
    int m_type;
};

class CPlayerObject {
public:
    void CycleWeapon(int, int);
    bool CanPlayerSwitchWeapon();
    CWeapon* GetCurrentWeapon() const;

    unsigned char unknown000[1892];
    int m_playerIndex;
};

class CSoldierObject {
public:
    void SetFaceAnimationState(unsigned short, long);
};

struct ShellMPPlayerView {
    unsigned char unknown00[21];
    unsigned char canCookGrenades;
    unsigned char unknown16[6];
};

struct ShellView {
    unsigned char unknown0000[535];
    unsigned char canCookGrenades;
    unsigned char unknown0218[4660];
    ShellMPPlayerView players[4];
};

struct NonVisDestructView {
    unsigned char unknown0000[12640];
    bool nonVisDestruct;
};

extern BSObjectView* g_pBSObject;
extern ShellView g_Shell;
extern bool g_bInMultiplayerMode;

int* CreateRootHierObject(TriggerObject_struct*, int);
float MathFunRandomReal(float, float);
long long MathFunRandomI64(long long, long long);
void BSRegisterTimerEvent(int, unsigned short, BSObject*, void*, ETimerReplaceMethod);

inline float MathFunRandom(float low, float high) {
    if (low == high)
        return low;
    return MathFunRandomReal(low, high);
}

inline int MathFunRandom(int low, int high) {
    if (low == high)
        return low;
    return MathFunRandomI64(low, high);
}

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

void BIFunc_CreateChildRootHierObject(int** stack, void*) {
    int object = 0;
    object = *CreateRootHierObject(g_pBSObject->trigger, *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1)));
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = *(int*)&object;
}

void BIFunc_GetMyInstanceData(int** stack, void*) {
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = g_pBSObject->user->GetScriptData();
}

void BIFunc_GetRandomFloat(int** stack, void*) {
    BSValueView value;
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    float low = *(float*)(*stack - (count - 1));
    float high = *(float*)(*stack - (count - 2));
    value.f = MathFunRandom(low, high);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    *(BSValueView*)*stack = value;
}

void BIFunc_GetRandomInteger(int** stack, void*) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    int low = *(*stack - (count - 1));
    int high = *(*stack - (count - 2));
    int value = MathFunRandom(low, high);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = value;
}

void BIFunc_PlayerSwitchWeapons(int** stack, void* object) {
    int direction = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    ((ISceneNode*)object)->AsPlayerObject()->CycleWeapon(direction, -1);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_PlayerCanSwitchWeapon(int** stack, void* object) {
    CPlayerObject* player = ((ISceneNode*)object)->AsPlayerObject();
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = player->CanPlayerSwitchWeapon();
}

void BIFunc_PlayerCanCookGrenades(int** stack, void* object);

void BIFunc_PlayerHasMeleeWeapon(int** stack, void* object);

void BIFunc_EnableNonVisDestruct(int** stack, void* object);

void BIFunc_RegisterTimerEvent(int** stack, void*);

void BIFunc_PlayFacialAnimation(int** stack, void* object);

void BIFunc_PlayerCanCookGrenades(int** stack, void* object) {
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    bool canCook;
    if (g_bInMultiplayerMode) {
        int player = ((ISceneNode*)object)->AsPlayerObject()->m_playerIndex;
        canCook = g_Shell.players[player].canCookGrenades != 0;
    }
    else
        canCook = g_Shell.canCookGrenades != 0;
    **stack = canCook;
}
