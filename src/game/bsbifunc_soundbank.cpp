// A fragment of bsbifunc.cpp (0x8002b064): script built-ins that initialise
// debugging from a trigger (no effect), load a one-shot sound bank (for the
// multiplayer mission and level stored in the shell, otherwise the shell's
// current mission and stage) and remember the bank, mission and level, show or
// hide the first player's letterbox, and return the index of the closest cover
// point with a free peek position (searched from the script object's position
// in its proximity leaf, within the given radius; -1 when none), followed by
// the weak BSGO_Basic::GetProximityData (0); then return the id of the closest
// valid waypoint of a type (searched the same way, any or only free ones; -1
// when none), stop the calling soldier's current voice (removing it from the
// sound schedule and returning 1) and return its voice handle (-1 without a
// soldier). Each reads its arguments below the script stack top and pops the
// built-in's arguments. The file name is this project's; the original record is
// bsbifunc.cpp and the built-ins around these are not reconstructed. The
// functions, classes and globals are named by the mangled symbols; ISceneNode
// is declared with its virtual functions in the order of __vt__10ISceneNode
// (GetPosition at +64) and BSGO_Basic in the order of __vt__10BSGO_Basic; the
// shell, scene, player, soldier (the voice handle at +17312), proximity-data,
// waypoint and trigger-object views and the argument helpers are inferred
// (members at their offsets, names not original), as are the built-in record
// view, the 16-byte, 8-byte aligned position view passed as the filter
// position, and the waypoint CRC value (0x9101c906, the enumerator's name
// unknown).
class CCollision;
class CDrawContext;
class CMatrix;
class CVector3;
class CBullet;
class CLight;
class CPlayerObject;
class CSoldierObject;
struct AIDoodadView;

#include "game/ISceneNode.h"



#include "game/CVector3.h"

class CAIFilterRealPosition;
struct _PropBSPLeaf;
enum DWI_OBJECT_CRC_ENUM {};

struct ProximityDataView {
    unsigned char unknown00[12];
    _PropBSPLeaf* leaf;
};

class BSGO_Basic {
    unsigned char unknown00[12];

public:
    virtual void Destroy();
    virtual int GetScriptData();
    virtual ISceneNode* GetSceneNode();
    virtual ProximityDataView* GetProximityData();
};

struct BSObjectView {
    unsigned char unknown00[12];
    BSGO_Basic* user;
};

struct TriggerObject_struct {
    unsigned char unknown00[12];
};

struct WaypointView {
    unsigned char unknown00[8];
    int id;
};

class CCoverPoint {
public:
    int GetAvailablePeekPosition() const;
};

class CAIFilterGlobal {
public:
    CCoverPoint* GetClosestWaypointByType(const _PropBSPLeaf*, const CAIFilterRealPosition&, DWI_OBJECT_CRC_ENUM, bool, float) const;

    unsigned char unknown00[24];
};

extern BSObjectView* g_pBSObject;
extern TriggerObject_struct g_pTriggerObjects[2000];
extern CAIFilterGlobal g_aigAIFilterGlobalObject;

class CShellMenu {
public:
    int Get_currentMission();
    int Get_currentStage();

    unsigned char unknown0000[5148];
    int m_mpMission;
    int m_mpLevel;
};

class CPlayerObject {
public:
    unsigned char unknown0000[4720];
    bool m_letterbox;
};

class CSoldierObject {
public:
    unsigned char unknown0000[17312];
    int m_voice;
};

class SoundSchedule {
public:
    void RemoveSound(int, bool);
};

extern SoundSchedule g_SoundSchedule;

class CScene {
public:
    CPlayerObject* GetPlayer(int) const;

    unsigned char unknown000[328];
};

extern CShellMenu g_Shell;
extern CScene g_scene;
extern bool g_bInMultiplayerMode;
extern int g_CurrentMission;
extern int g_CurrentLevel;
extern int g_CurrentSoundBank;

void LoadOneShotSound(int, int, int);

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

inline float BSArgFloat(int** stack, int index) {
    return *(float*)(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - index));
}

void BIFunc_DebugInitFromTrigger(int** stack, void*) {
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_LoadOneShotSoundBank(int** stack, void*) {
    int bank = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    int mission;
    int level;
    if (g_bInMultiplayerMode) {
        mission = g_Shell.m_mpMission;
        level = g_Shell.m_mpLevel;
    } else {
        mission = g_Shell.Get_currentMission();
        level = g_Shell.Get_currentStage();
    }
    LoadOneShotSound(bank, mission, level);
    g_CurrentMission = mission;
    g_CurrentLevel = level;
    g_CurrentSoundBank = bank;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_DisplayLetterbox(int** stack, void*) {
    bool show = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1)) != 0;
    g_scene.GetPlayer(0)->m_letterbox = show;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_AIGetClosestCoverPoint(int** stack, void*) {
    int index = -1;
    float radius = *(float*)(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    ProximityDataView* proximity = g_pBSObject->user->GetProximityData();
    if (proximity->leaf) {
        CVector3 position;
        g_pBSObject->user->GetSceneNode()->GetPosition(position);
        radius *= radius;
        CCoverPoint* point = g_aigAIFilterGlobalObject.GetClosestWaypointByType(proximity->leaf, (const CAIFilterRealPosition&)position, (DWI_OBJECT_CRC_ENUM)0x9101c906, true, radius);
        if (point && point->GetAvailablePeekPosition())
            index = (TriggerObject_struct*)point - g_pTriggerObjects;
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = *(int*)&index;
}

__declspec(weak) ProximityDataView* BSGO_Basic::GetProximityData() {
    return 0;
}

void BIFunc_GetClosestValidWaypoint(int** stack, void*) {
    int value;
    DWI_OBJECT_CRC_ENUM type = (DWI_OBJECT_CRC_ENUM)BSArgInt(stack, 1);
    float radius = *(float*)(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 3));
    bool any = BSArgBool(stack, 5);
    ProximityDataView* proximity = g_pBSObject->user->GetProximityData();
    if (proximity->leaf) {
        CVector3 position;
        g_pBSObject->user->GetSceneNode()->GetPosition(position);
        radius *= radius;
        WaypointView* waypoint = (WaypointView*)g_aigAIFilterGlobalObject.GetClosestWaypointByType(proximity->leaf, (const CAIFilterRealPosition&)position, type, !any, radius);
        if (waypoint)
            value = waypoint->id;
        else
            value = -1;
    } else {
        value = -1;
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = *(int*)&value;
}

void BIFunc_StopCurrentVoice(int** stack, void* object) {
    CSoldierObject* soldier = ((ISceneNode*)object)->AsSoldierObject();
    if (soldier && soldier->m_voice != -1) {
        g_SoundSchedule.RemoveSound(soldier->m_voice, true);
        soldier->m_voice = -1;
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = 1;
}

void BIFunc_GetVoiceHandle(int** stack, void* object) {
    int value = -1;
    CSoldierObject* soldier = ((ISceneNode*)object)->AsSoldierObject();
    if (soldier)
        value = soldier->m_voice;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = *(int*)&value;
}
