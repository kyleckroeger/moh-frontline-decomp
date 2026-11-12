// A fragment of bsbifunc.cpp (0x80028f8c): built-ins that add an objective
// (and raise the highest objective number), return the script object's
// embedded hit-reaction info, followed by BSGO_Basic's weak default for it
// (none), return an event-system memory block of 32 bytes, and destroy a
// light (marking it for destruction). Each reads its arguments below the
// script stack top, pops the built-in's arguments and writes a result to the
// new top through an integer union. The file name is this project's; the
// original record is bsbifunc.cpp and the built-ins around these are not
// reconstructed. The functions, classes and globals are named by the mangled
// symbols; BSGO_Basic's virtual functions are declared in the order of
// __vt__10BSGO_Basic (its virtual table pointer follows 12 bytes of
// members), ISceneNode in the order of __vt__10ISceneNode, and the built-in
// record and value views are inferred.
class CCollision;
class CDrawContext;
class CMatrix;
class CVector3;
class CBullet;
class CLight;
class CPlayerObject;
struct AIDoodadView;

#include "game/ISceneNode.h"


union BSValueView {
    int i;
    float f;
};

class BSGO_Basic {
    unsigned char unknown00[12];

public:
    virtual void Destroy();
    virtual void* GetScriptData();
    virtual void* GetSceneNode();
    virtual void* GetProximityData();
    virtual void* GetEmbeddedHitReactionInfo();
};

struct BSObjectView {
    unsigned char unknown00[12];
    BSGO_Basic* user;
};

extern BSObjectView* g_pBSObject;
extern int g_iHighestObjectiveNumber;

void AddObjective(unsigned int, unsigned int);
void* BSEventGetEventMemoryBlock(unsigned int);

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

void BIFunc_AddObjective(int** stack, void*) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    int objective = *(*stack - (count - 1));
    AddObjective(objective, *(*stack - (count - 2)));
    if (objective > g_iHighestObjectiveNumber)
        g_iHighestObjectiveNumber = objective;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_GetEmbeddedHitReactionInfo(int** stack, void*) {
    void* info = 0;
    info = g_pBSObject->user->GetEmbeddedHitReactionInfo();
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = *(int*)&info;
}

__declspec(weak) void* BSGO_Basic::GetEmbeddedHitReactionInfo() {
    return 0;
}

void BIFunc_GetEventSystemMemBlock(int** stack, void*) {
    BSValueView value;
    value.i = (int)BSEventGetEventMemoryBlock(32);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    *(BSValueView*)*stack = value;
}

void BIFunc_DestroyLight(int** stack, void*) {
    ((ISceneNode*)*(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1)))->MarkForDestruction(0);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
