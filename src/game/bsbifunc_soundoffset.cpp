// A fragment of bsbifunc.cpp (0x8002a034): the script built-in that plays a
// sound from the script trigger or the calling node at an offset, returning the
// result (kept in memory before it is written). It reads its arguments below
// the script stack top and pops the built-in's arguments. The file name is this
// project's; the original record is bsbifunc.cpp and the built-ins around it
// are not reconstructed. The functions and globals are named by the mangled
// symbols; the vector view (four floats, 8-byte aligned, with an inline
// setter), the script-object and built-in record views are inferred.
// BIFunc_PlaySoundFromInputTrigger follows it: the same call with the
// trigger taken from the first argument, the sound and mode after it, and a
// zero offset (0.0f is a .sdata2 pool entry).
class CVector3 {
public:
    float x;
    float y;
    float z;
    float w;

    void Set(float ax, float ay, float az) {
        x = ax;
        y = ay;
        z = az;
    }
} __attribute__((aligned(8)));

struct TriggerObject_struct;
class BSObject;

struct BSObjectView {
    unsigned char unknown00[8];
    TriggerObject_struct* trigger;
};

extern BSObjectView* g_pBSObject;

int PlaySoundFromTriggerOrObject(TriggerObject_struct*, BSObject*, void*, int, int, CVector3&);

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

void BIFunc_PlaySoundAtOffset(int** stack, void* object) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    int sound = *(*stack - (count - 1));
    int mode = *(*stack - (count - 2));
    CVector3 offset;
    float x = *(float*)(*stack - (count - 3));
    float y = *(float*)(*stack - (count - 4));
    float z = *(float*)(*stack - (count - 5));
    offset.Set(x, y, z);
    int value = PlaySoundFromTriggerOrObject(g_pBSObject->trigger, (BSObject*)g_pBSObject, object, sound, mode, offset);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = *(int*)&value;
}

void BIFunc_PlaySoundFromInputTrigger(int** stack, void* object) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    TriggerObject_struct* trigger = (TriggerObject_struct*)*(*stack - (count - 1));
    int sound = *(*stack - (count - 2));
    int mode = *(*stack - (count - 3));
    CVector3 offset;
    offset.Set(0.0f, 0.0f, 0.0f);
    int value = PlaySoundFromTriggerOrObject(trigger, (BSObject*)g_pBSObject, object, sound, mode, offset);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = *(int*)&value;
}
