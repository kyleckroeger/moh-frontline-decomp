// A fragment of bsbifunc.cpp (0x80029a3c): script built-ins that send a music
// (pathfinder) event and set the music level, and the built-in AEMSEvent. Each
// reads its arguments below the script stack top and pops the built-in's
// arguments. AEMSEvent takes a mode, an AEMS event record, a handle, a schedule
// flag, a script event and a volume scale: it sets the AEMS object (the calling
// node's soldier, if any), scales the record's volume field (index from
// GetAEMSStructureValues), takes the script trigger's position for a
// scheduled event without a calling node (positional), marks the event
// persistent for a soldier's event types 8/15 (and 7/14 when the record's word
// 1 is 1), then sends (0), begins (1), updates (2) or ends (3) the event and
// adds/updates/removes it in the sound schedule when it is scheduled or
// persistent; the result (-1 by default) is pushed. The file name is this
// project's; the original record is bsbifunc.cpp and the built-ins around these
// are not reconstructed. The functions and globals are named by the mangled
// symbols; the built-in record, trigger and record-field views and the
// argument meanings are inferred. The calling object is read through an
// ISceneNode local and the result is declared between the third and fourth
// argument reads (both set the register use).
void MUSIC_SendEvent(int);
void MUSIC_SetLevel(int);

enum EBSEventEnum {};
class CCollision;
class CDrawContext;
class CMatrix;
class CBullet;
class CLight;
class CPlayerObject;
class CSoldierObject;

#include "game/CVector3.h"

#include "game/ISceneNode.h"


// Inferred: the script object's trigger (+8) and its core (+4), whose position
// is at +16.
struct TriggerCoreView {
    unsigned char unknown00[16];
    float x;
    float y;
    float z;
};
struct TriggerObject_struct {
    unsigned char unknown00[4];
    TriggerCoreView* core;
};
struct BSObject {
    unsigned char unknown00[8];
    TriggerObject_struct* trigger;
};

extern BSObject* g_pBSObject;

class SoundSchedule {
public:
    enum ESoundScheduleObjectType {};
    void AddSound(ESoundScheduleObjectType, ISceneNode*, CVector3*, void*, int, int, int, int, int, BSObject*,
                  EBSEventEnum, float, bool, int);
    void UpdateSound(int, void*, int);
    void RemoveSound(int, bool);
};

extern SoundSchedule g_SoundSchedule;
typedef SoundSchedule::ESoundScheduleObjectType SoundObjectType;

void AEMS_SetObject(CSoldierObject*);
int AEMS_SendEvent(void*);
int AEMS_BeginEvent(void*);
int AEMS_UpdateEvent(void*, int);
int AEMS_EndEvent(void*, int);
void GetAEMSStructureValues(int, int*, int*, int*, int*);

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

void BIFunc_PathfinderEvent(int** stack, void*) {
    MUSIC_SendEvent(*(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1)));
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_PathfinderSetLevel(int** stack, void*) {
    MUSIC_SetLevel(*(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1)));
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_AEMSEvent(int** stack, void* object) {
    bool persistent;
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    int mode = *(*stack - (count - 1));
    int* data = (int*)*(*stack - (count - 2));
    int handle = *(*stack - (count - 3));
    int result = -1;
    int schedule = *(*stack - (count - 4));
    int event = *(*stack - (count - 5));
    float scale = *(float*)(*stack - (count - 6));
    CSoldierObject* soldier;
    ISceneNode* node = (ISceneNode*)object;
    int first;
    int second;
    int third;
    int fourth;
    CVector3 position;
    int positional;

    if (node)
        soldier = node->AsSoldierObject();
    else
        soldier = 0;
    AEMS_SetObject(soldier);
    GetAEMSStructureValues(data[0], &first, &second, &third, &fourth);
    data[second] *= scale;
    positional = false;
    if (schedule && !node) {
        positional = true;
        position.Set(g_pBSObject->trigger->core->x, g_pBSObject->trigger->core->y, g_pBSObject->trigger->core->z);
    }
    persistent = false;
    if (soldier) {
        switch (data[0]) {
        case 8:
        case 15:
            persistent = true;
            break;
        case 7:
        case 14:
            persistent = data[1] == 1;
            break;
        }
    }
    switch (mode) {
    case 0:
        result = AEMS_SendEvent(data);
        if (schedule || persistent) {
            GetAEMSStructureValues(data[0], &first, &second, &third, &fourth);
            g_SoundSchedule.AddSound((SoundObjectType)positional, node,
                                     &position, data, fourth, result, first, second, third, g_pBSObject,
                                     (EBSEventEnum)event, scale, persistent, 0);
        }
        break;
    case 1:
        result = AEMS_BeginEvent(data);
        if (schedule || persistent) {
            GetAEMSStructureValues(data[0], &first, &second, &third, &fourth);
            g_SoundSchedule.AddSound((SoundObjectType)positional, node,
                                     &position, data, fourth, result, first, second, third, g_pBSObject,
                                     (EBSEventEnum)event, scale, persistent, 0);
        }
        break;
    case 2:
        result = AEMS_UpdateEvent(data, handle);
        if (schedule || persistent) {
            GetAEMSStructureValues(data[0], &first, &second, &third, &fourth);
            g_SoundSchedule.UpdateSound(handle, data, fourth);
        }
        break;
    case 3:
        result = AEMS_EndEvent(data, handle);
        if (schedule || persistent)
            g_SoundSchedule.RemoveSound(handle, false);
        break;
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = *(int*)&result;
}
