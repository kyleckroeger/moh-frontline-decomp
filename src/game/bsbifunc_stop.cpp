// A fragment of bsbifunc.cpp (0x8002ddfc): built-ins that stop the buddy
// player, distance-to-target, target and target-search checks: a given
// registration record is unregistered from its schedule, otherwise every
// registration of the object (the script's object for the buddy check, the
// scene node's script object for the others), and the built-in's arguments are
// popped; then register the script object with the buddy-player check schedule
// (after fetching the scene node's AI doodad, unused), returning the
// registration. The file name is this project's; the original record is
// bsbifunc.cpp and the built-ins around these are not reconstructed. The
// functions, classes and globals are named by the mangled symbols; ISceneNode
// is declared with its virtual functions in the order of __vt__10ISceneNode
// (GetScriptObject at +96, GetAIDoodad at +204), and the schedule (BSSchedule's
// size is not known) and built-in record views and the argument helpers are
// inferred.
class CCollision;
class CDrawContext;
class CMatrix;
class CVector3;
class CBullet;
class CLight;
class CPlayerObject;
struct BSObject;
struct AIDoodadView;

#include "game/ISceneNode.h"


struct BSScheduleRegistrationRecord_struct;

class BSSchedule {
public:
    int Register(BSObject*, unsigned short, bool, int, int, int);
    void Unregister(BSScheduleRegistrationRecord_struct*);
    void Unregister(BSObject*);

    unsigned char unknown00[16];
};

extern BSSchedule g_BuddyPlayerCheckSchedule;
extern BSSchedule g_DistanceToTargetSchedule;
extern BSSchedule g_TargetCheckSchedule;
extern BSSchedule g_TargetSearchSchedule;
extern BSObject* g_pBSObject;

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

void BIFunc_StopBuddyPlayerCheck(int** stack, void*) {
    BSScheduleRegistrationRecord_struct* record =
        (BSScheduleRegistrationRecord_struct*)*(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    if (record)
        g_BuddyPlayerCheckSchedule.Unregister(record);
    else
        g_BuddyPlayerCheckSchedule.Unregister(g_pBSObject);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_StopDistanceToTarget(int** stack, void* object) {
    BSScheduleRegistrationRecord_struct* record =
        (BSScheduleRegistrationRecord_struct*)*(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    if (record)
        g_DistanceToTargetSchedule.Unregister(record);
    else
        g_DistanceToTargetSchedule.Unregister(((ISceneNode*)object)->GetScriptObject());
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_StopTargetCheck(int** stack, void* object) {
    BSScheduleRegistrationRecord_struct* record =
        (BSScheduleRegistrationRecord_struct*)*(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    if (record)
        g_TargetCheckSchedule.Unregister(record);
    else
        g_TargetCheckSchedule.Unregister(((ISceneNode*)object)->GetScriptObject());
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_StopTargetSearch(int** stack, void* object) {
    BSScheduleRegistrationRecord_struct* record =
        (BSScheduleRegistrationRecord_struct*)*(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    if (record)
        g_TargetSearchSchedule.Unregister(record);
    else
        g_TargetSearchSchedule.Unregister(((ISceneNode*)object)->GetScriptObject());
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

inline int BSArgInt(int** stack, int index) {
    return *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - index));
}

inline int BSArgBool(int** stack, int index) {
    return *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - index)) != 0;
}

inline float BSArgFloat(int** stack, int index) {
    return *(float*)(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - index));
}

void BIFunc_RegisterBuddyPlayerCheck(int** stack, void* object) {
    ((ISceneNode*)object)->GetAIDoodad();
    int result = g_BuddyPlayerCheckSchedule.Register(g_pBSObject, BSArgInt(stack, 4), BSArgBool(stack, 5), BSArgBool(stack, 1), BSArgFloat(stack, 2), BSArgInt(stack, 3));
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = result;
}
