// A fragment of bsbifunc.cpp (0x80029ee0): the script built-in that ends a
// sound (removing its handle from the sound schedule unless it is -1). It
// reads its argument below the script stack top and pops the built-in's
// arguments. BIFunc_PlaySound after it plays a sound from the script
// object's trigger or the calling object with a zero offset (unless the
// sound is -1) and writes the handle (-1 otherwise) to the new top; the
// result is declared after the arguments are read, as its register use
// shows, and 0.0f is an entry of the file's .sdata2 pool. The file name is
// this project's; the original record is bsbifunc.cpp and the built-ins
// around it are not reconstructed. The functions and globals are named by
// the mangled symbols; the built-in record, script object and vector views
// are inferred.
class SoundSchedule {
public:
    void RemoveSound(int, bool);
};

extern SoundSchedule g_SoundSchedule;

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

struct TriggerObject_struct;

// Inferred: a script object's trigger at +8.
struct BSObject {
    unsigned char unknown00[8];
    TriggerObject_struct* trigger;
};

#include "game/CVector3.h"

extern BSObject* g_pBSObject;

int PlaySoundFromTriggerOrObject(TriggerObject_struct*, BSObject*, void*, int, int, CVector3&);

void BIFunc_EndSound(int** stack, void*) {
    int handle = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    if (handle != -1)
        g_SoundSchedule.RemoveSound(handle, true);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_PlaySound(int** stack, void* object) {
    int sound = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    int flags = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 2));
    int handle = -1;
    CVector3 offset;
    offset.x = 0.0f;
    offset.y = 0.0f;
    offset.z = 0.0f;
    if (sound != -1)
        handle = PlaySoundFromTriggerOrObject(g_pBSObject->trigger, g_pBSObject, object, sound, flags, offset);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = *(int*)&handle;
}
