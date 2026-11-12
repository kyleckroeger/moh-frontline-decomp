// A fragment of bsbifunc.cpp (0x8002f8dc): script built-ins for the player's
// melee weapon check, non-visible destruction, timer events and facial
// animation. Each reads its arguments below the script stack top (by the
// built-in's parameter count), pops the arguments and, for a result, writes it
// to the new top. The file name is this project's; the original record is
// bsbifunc.cpp and the built-ins around these are not reconstructed. The
// functions, classes and globals are named by the mangled symbols; ISceneNode
// is declared with its virtual functions in the order of __vt__10ISceneNode,
// the shell, player, weapon and soldier views are inferred (members at their
// offsets, names not original), as are the built-in record view, the
// integer/float value union and the random-range helpers.
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

// The object at BSObject+12: 12 bytes of members, then its virtual table
// pointer; the second virtual function returns the instance data.
class BSObjectUserView {
    unsigned char unknown00[12];

public:
    virtual void unknownVirtual0();
    virtual int GetInstanceData();
};

struct BSObjectView {
    unsigned char unknown00[8];
    TriggerObject_struct* trigger;
    BSObjectUserView* user;
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

void BIFunc_CreateChildRootHierObject(int** stack, void*);

void BIFunc_GetMyInstanceData(int** stack, void*);

void BIFunc_GetRandomFloat(int** stack, void*);

void BIFunc_GetRandomInteger(int** stack, void*);

void BIFunc_PlayerSwitchWeapons(int** stack, void* object);

void BIFunc_PlayerCanSwitchWeapon(int** stack, void* object);

void BIFunc_PlayerCanCookGrenades(int** stack, void* object);

void BIFunc_PlayerHasMeleeWeapon(int** stack, void* object) {
    bool melee = false;
    CPlayerObject* player = ((ISceneNode*)object)->AsPlayerObject();
    if (player->GetCurrentWeapon()) {
        switch (player->GetCurrentWeapon()->m_type) {
        case 33:
        case 34:
        case 37:
            melee = false;
            break;
        default:
            melee = true;
            break;
        }
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = melee;
}

void BIFunc_EnableNonVisDestruct(int** stack, void* object) {
    ((NonVisDestructView*)object)->nonVisDestruct = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1)) == 0;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_RegisterTimerEvent(int** stack, void*) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    BSRegisterTimerEvent(*(*stack - (count - 1)), *(*stack - (count - 2)), (BSObject*)g_pBSObject,
                         (void*)*(*stack - (count - 3)), (ETimerReplaceMethod)0);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_PlayFacialAnimation(int** stack, void* object) {
    ((CSoldierObject*)object)->SetFaceAnimationState(*(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1)), -1);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

