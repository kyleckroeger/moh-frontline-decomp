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
// zero offset (0.0f is a .sdata2 pool entry). PlaySoundFromTriggerOrObject
// follows: it reads the sound type (key 11) and bank (key 12) from the
// trigger's property list (16-bit key/value words), takes the calling node's
// position (or the trigger's), adds the offset, makes sound 82 of a player
// with no type the player's character voice (in multiplayer characters 5, 7
// and 8 use 449, the rest 454), and plays the scripted sound event when the
// sound table has it, returning the result or -1. The trigger, property list,
// node, shell and sound-table views are inferred.
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

struct PropertyListView {
    int count;
    unsigned int entries[1];
};

struct TriggerCoreView {
    unsigned char unknown00[16];
    float x;
    float y;
    float z;
    unsigned char unknown1c[40];
    PropertyListView* properties;
};

struct TriggerObject_struct {
    unsigned char unknown00[4];
    TriggerCoreView* core;
};

class CPlayerObject {
public:
    unsigned char unknown000[1896];
    int m_index;
};

class ISceneNode {
public:
    virtual void vf08();
    virtual void vf0c();
    virtual void vf10();
    virtual void vf14();
    virtual void vf18();
    virtual void vf1c();
    virtual void vf20();
    virtual void vf24();
    virtual void vf28();
    virtual void vf2c();
    virtual void vf30();
    virtual void vf34();
    virtual void vf38();
    virtual void vf3c();
    virtual void GetPosition(CVector3&) const;
    virtual void vf44();
    virtual void vf48();
    virtual void vf4c();
    virtual void vf50();
    virtual void vf54();
    virtual void vf58();
    virtual void vf5c();
    virtual void vf60();
    virtual void vf64();
    virtual void vf68();
    virtual void vf6c();
    virtual void vf70();
    virtual void vf74();
    virtual void vf78();
    virtual void vf7c();
    virtual void vf80();
    virtual void vf84();
    virtual void vf88();
    virtual void vf8c();
    virtual void vf90();
    virtual void vf94();
    virtual void vf98();
    virtual CPlayerObject* AsPlayerObject();
};

class CSoundTable {
public:
    struct SSoundParms {
        unsigned char unknown00[16];
    };

    bool GetSoundEvent(int, int, int, SSoundParms*) const;
};

extern CSoundTable g_soundMapSingleton;

struct MPPlayerView {
    int character;
    unsigned char unknown04[24];
};

class CShellMenu {
public:
    unsigned char unknown0000[5200];
    MPPlayerView m_mpPlayers[4];
    unsigned char unknown14c0[1308];
};

extern CShellMenu g_Shell;
extern bool g_bInMultiplayerMode;

int PlayScriptedSound(ISceneNode*, CVector3, const CSoundTable::SSoundParms&, int, BSObject*);

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

int PlaySoundFromTriggerOrObject(TriggerObject_struct* trigger, BSObject* script, void* object, int sound, int mode, CVector3& offset) {
    int result = -1;
    int type = -1;
    int bank = -1;
    PropertyListView* list;
    TriggerCoreView* core = trigger->core;
    list = core->properties;
    if (list) {
        unsigned int* p = list->entries;
        unsigned int* end = p + list->count;
        for (; p <= end; p++) {
            int key = (int)*p >> 16;
            int value = *p & 0xffff;
            if (key == 11)
                type = value;
            else if (key == 12)
                bank = value;
        }
    }
    ISceneNode* node = (ISceneNode*)object;
    CVector3 position;
    if (node)
        node->GetPosition(position);
    else
        position.Set(core->x, core->y, core->z);
    position.x = position.x + offset.x;
    position.y = position.y + offset.y;
    position.z = position.z + offset.z;
    if (type == -1 && sound == 82 && node && node->AsPlayerObject()) {
        if (g_bInMultiplayerMode) {
            int index = node->AsPlayerObject()->m_index;
            switch (g_Shell.m_mpPlayers[index].character) {
            case 5:
            case 7:
            case 8:
                type = 449;
                break;
            case 6:
            default:
                type = 454;
                break;
            }
        } else {
            type = 454;
        }
    }
    CSoundTable::SSoundParms parms;
    if (type != -1 && g_soundMapSingleton.GetSoundEvent(type, bank, sound, &parms))
        result = PlayScriptedSound(node, position, parms, mode, script);
    return result;
}
