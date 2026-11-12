// A fragment of bsbifunc.cpp (0x8002daa8): the weak CBullet::GetPosition (the
// transform's position row, copied as two doubles), then script built-ins for a
// script error and a debug print (both only pop their arguments), a break point
// (printing a message) and enabling or disabling a weapon type for the calling
// node's player (returning the result). Each pops the built-in's arguments. The
// file name is this project's; the original record is bsbifunc.cpp and the
// built-ins around these are not reconstructed. The functions, classes and
// globals are named by the mangled symbols; ISceneNode is declared with its
// virtual functions in the order of __vt__10ISceneNode (AsPlayerObject at
// +156); CBullet is an inferred non-virtual view with its transform at +80 and
// the by-value row getter inferred, and the built-in record view and argument
// helpers are inferred.
//
// CVector3 view: four floats, 8-byte aligned, overlaid with two doubles. The
// union is inferred from the code, not the original declaration: vectors are
// copied as two lfd/stfd pairs, which an implicit copy through the double pair
// reproduces.
#include "game/CVector3.h"

#include "game/CMatrix.h"


class CBullet {
public:
    void GetPosition(CVector3&) const;

    CVector3 PositionRow() const { return m_tm.pos; }

    unsigned char unknown00[80];
    CMatrix m_tm;
};

class CCollision;
class CDrawContext;
class CMatrix;
class CLight;
class CPlayerObject;
struct AIDoodadView;

#include "game/ISceneNode.h"



enum EWeaponTypes {};

class CPlayerObject {
public:
    bool SetWeaponEnable(EWeaponTypes, bool);
};

extern "C" int printf(const char*, ...);

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

inline int BSArgInt(int** stack, int index) {
    return *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - index));
}

inline int BSArgBool(int** stack, int index) {
    return *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - index)) != 0;
}

__declspec(weak) void CBullet::GetPosition(CVector3& v) const {
    v = PositionRow();
}

void BIFunc_ScriptError(int** stack, void*) {
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_DebugTTYPrint(int** stack, void*) {
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_BreakPoint(int** stack, void*) {
    printf("\nbreak point!");
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_SetWeaponEnable(int** stack, void* object) {
    EWeaponTypes type = (EWeaponTypes)BSArgInt(stack, 1);
    bool enable = BSArgBool(stack, 2);
    CPlayerObject* player = ((ISceneNode*)object)->AsPlayerObject();
    bool result = player->SetWeaponEnable(type, enable);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = result;
}
