// A fragment of bsbifunc.cpp (0x8002b60c): the built-in script functions
// MakeNoise and MakeNoiseFromInputTrigger. MakeNoise's arguments are a flag
// (noise type 52, or 51 when it is zero) and a loudness; the noise is made by the running script object's scene
// node (MakeNoise(ISceneNode*, ...), inlined with the general MakeNoise it
// inlines in turn) or, for a script object without a scene node, at its
// trigger's position with the calling object as the source (the general
// MakeNoise, inlined); the value passed is 1 for type 51 and 3 otherwise.
// Both MakeNoise overloads are defined here because they are inlined; their
// own copies are placed by bsbifunc_makenoise (file duplicates). The file is
// compiled with deferred inlining, so the source is in reverse image order.
// The functions, classes and globals are named by the symbols; the views
// follow bsbifunc_makenoise.cpp and bsbifunc_attach.cpp (script object:
// trigger +8, user +12; the user's GetSceneNode slot +0x10), and the noise
// types' meanings are inferred. MakeNoiseFromInputTrigger takes a trigger,
// a flag and a loudness and makes the noise at the trigger's position with
// the value 1; its type local has an enum type (the enum is inferred: an int
// if/else of two adjacent constants compiles to branch-free arithmetic, and
// the original branches). The rest of the file is in other units.
class CVector3 {
public:
    CVector3() {}
    CVector3(float x, float y, float z) : x(x), y(y), z(z) {}
    CVector3(const CVector3& o) {
        float y_;
        float z_ = o.z;
        y_ = o.y;
        float x_ = o.x;
        x = x_;
        y = y_;
        z = z_;
    }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CAIFilterRealPosition {
public:
    float x;
    float y;
    float z;
};

/* inferred: the script data's two values at +16 and +20 */
struct ScriptDataView {
    unsigned char unknown00[16];
    int m_value10;
    int m_value14;
};

class ISceneNode;

class BSGO_Basic {
    unsigned char unknown00[12];

public:
    virtual void Destroy();
    virtual ScriptDataView* GetScriptData();
    virtual ISceneNode* GetSceneNode();
};

struct TriggerCoreView {
    unsigned char unknown00[16];
    float x;
    float y;
    float z;
};

struct TriggerObject_struct {
    unsigned int flags;
    TriggerCoreView* core;
};

struct BSObjectView {
    unsigned char unknown00[8];
    TriggerObject_struct* trigger;
    BSGO_Basic* user;
};

extern BSObjectView* g_pBSObject;

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

class CAIFilterObject;

/* inferred: an AI doodad's filter object at +4 */
class CAIDoodadView {
public:
    unsigned char unknown00[4];
    CAIFilterObject* filter;
};

/* inferred: the scene node's position getter (slot +64) and AI doodad (slot +204) */
class ISceneNode {
public:
    virtual void unknown008();
    virtual void unknown00c();
    virtual void unknown010();
    virtual void unknown014();
    virtual void unknown018();
    virtual void unknown01c();
    virtual void unknown020();
    virtual void unknown024();
    virtual void unknown028();
    virtual void unknown02c();
    virtual void unknown030();
    virtual void unknown034();
    virtual void unknown038();
    virtual void unknown03c();
    virtual void GetPosition(CVector3&);
    virtual void unknown044();
    virtual void unknown048();
    virtual void unknown04c();
    virtual void unknown050();
    virtual void unknown054();
    virtual void unknown058();
    virtual void unknown05c();
    virtual void unknown060();
    virtual void unknown064();
    virtual void unknown068();
    virtual void unknown06c();
    virtual void unknown070();
    virtual void unknown074();
    virtual void unknown078();
    virtual void unknown07c();
    virtual void unknown080();
    virtual void unknown084();
    virtual void unknown088();
    virtual void unknown08c();
    virtual void unknown090();
    virtual void unknown094();
    virtual void unknown098();
    virtual void unknown09c();
    virtual void unknown0a0();
    virtual void unknown0a4();
    virtual void unknown0a8();
    virtual void unknown0ac();
    virtual void unknown0b0();
    virtual void unknown0b4();
    virtual void unknown0b8();
    virtual void unknown0bc();
    virtual void unknown0c0();
    virtual void unknown0c4();
    virtual void unknown0c8();
    virtual CAIDoodadView* GetAIDoodad();
};

class CPlayerObject {
public:
    virtual void unknown008();
    virtual void unknown00c();
    virtual void unknown010();
    virtual void unknown014();
    virtual void unknown018();
    virtual void unknown01c();
    virtual void unknown020();
    virtual void unknown024();
    virtual void unknown028();
    virtual void unknown02c();
    virtual void unknown030();
    virtual void unknown034();
    virtual void unknown038();
    virtual void unknown03c();
    virtual void unknown040();
    virtual void unknown044();
    virtual void unknown048();
    virtual void unknown04c();
    virtual void unknown050();
    virtual void unknown054();
    virtual void unknown058();
    virtual void unknown05c();
    virtual BSObjectView* GetScriptObject() const;
};

class CScene {
public:
    CPlayerObject* GetPlayer(int) const;

    unsigned char unknown000[328];
};

extern CScene g_scene;

/* inferred: the filter object's object at +12 */
class CAIFilterObject {
public:
    unsigned char unknown00[12];
    void* m_object;
};

/* inferred layout */
struct CAIFilterSoundInfo {
    void* m_source;
    CAIFilterObject* m_filter;
    CAIFilterRealPosition m_position;
    unsigned char unknown14[4];
    short m_type;
    unsigned char unknown1a[2];
    void* m_object;
    float m_radius;
    float m_loudness;
    bool m_flag;
    int m_value;
};

class CAIFilterGlobal {
public:
    static void PropagateSoundToAllObjects(const CAIFilterSoundInfo&);
};

void MakeNoise(void* source, CAIFilterObject* filter, const CVector3& position, int type, float loudness, bool flag,
               int value) {
    if (type == 51) {
        ScriptDataView* data = g_scene.GetPlayer(0)->GetScriptObject()->user->GetScriptData();
        if (data->m_value10 != 0 && data->m_value14 != 0)
            return;
    }
    if (loudness < 0.0f)
        loudness = 0.0f;
    if (loudness > 6.0f)
        loudness = 6.0f;
    CAIFilterSoundInfo info;
    info.m_source = source;
    info.m_filter = filter;
    info.m_type = type;
    CVector3 p(position);
    info.m_position = *(CAIFilterRealPosition*)&p;
    if (filter && filter->m_object)
        info.m_object = filter->m_object;
    else
        info.m_object = 0;
    info.m_loudness = loudness;
    info.m_radius = 10.0f;
    info.m_flag = flag;
    info.m_value = value;
    CAIFilterGlobal::PropagateSoundToAllObjects(info);
}

void MakeNoise(ISceneNode* node, int type, float loudness, int value) {
    CAIFilterObject* filter = 0;
    if (node && node->GetAIDoodad())
        filter = node->GetAIDoodad()->filter;
    CVector3 position;
    if (node)
        node->GetPosition(position);
    const CVector3& pos = position;
    MakeNoise(node, filter, pos, type, loudness, true, value);
}

enum ENoiseType { NOISE_TYPE_51 = 51, NOISE_TYPE_52 = 52 };

void BIFunc_MakeNoiseFromInputTrigger(int** stack, void* object) {
    ENoiseType type;
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    TriggerObject_struct* trigger = *(TriggerObject_struct**)(*stack - (count - 1));
    float loudness = *(float*)(*stack - (count - 3));
    if (*(*stack - (count - 2)) == 0)
        type = NOISE_TYPE_51;
    else
        type = NOISE_TYPE_52;
    TriggerCoreView* core = trigger->core;
    CVector3 position(core->x, core->y, core->z);
    MakeNoise(object, 0, position, type, loudness, true, 1);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_MakeNoise(int** stack, void* object) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    int* args;
    long flag;
    int type = 52;
    float loudness;
    args = *stack;
    args = args - (count - 1);
    loudness = *(float*)(*stack - (count - 2));
    flag = *args;
    if (flag == 0)
        type = 51;
    if (g_pBSObject && g_pBSObject->user && g_pBSObject->user->GetSceneNode()) {
        ISceneNode* node = g_pBSObject->user->GetSceneNode();
        MakeNoise(node, type, loudness, type == 51 ? 1 : 3);
    } else if (g_pBSObject && g_pBSObject->trigger && g_pBSObject->trigger->core) {
        TriggerCoreView* core = g_pBSObject->trigger->core;
        CVector3 position(core->x, core->y, core->z);
        MakeNoise(object, 0, position, type, loudness, true, type == 51 ? 1 : 3);
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
