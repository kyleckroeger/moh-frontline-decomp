// A fragment of bsbifunc.cpp (0x8002bd00): MakeNoise propagates a sound to
// all AI objects: its source, filter object (and that filter's object at
// +12), position, type, loudness clamped to 0..6, a radius of 10 (inferred),
// a flag and a value. Sounds of type 51 are dropped while the player's script
// data has both values at +16 and +20 set. The functions, classes and globals
// are named by the symbols; the player's GetScriptObject slot (0x60) follows
// the ISceneNode views (placeholders named by offset before it), the script
// object and BSGO_Basic views follow bsbifunc_soundbank.cpp, and the sound
// info layout, the script data view and the parameter meanings are inferred.
// The rest of the file is in other units.
class CVector3 {
public:
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

class BSGO_Basic {
    unsigned char unknown00[12];

public:
    virtual void Destroy();
    virtual ScriptDataView* GetScriptData();
};

struct BSObjectView {
    unsigned char unknown00[12];
    BSGO_Basic* user;
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
