// A fragment of hierobject.cpp (0x80098f84): CHierObject's
// StartMotionPlayback (with motion keys, clamps the start and end keys to the
// key count, records them and the script event, takes the key rotation and
// offset from the script rotation and animated position, and starts the
// animation motion) and GetLightVolume (its own light volume when the flag
// at +1416 is set, else its parent's). The classes and functions are named
// by the mangled symbols; CHierObject declares its destructor (defined
// elsewhere) first so its table is not emitted here; CStaticObject declares
// placeholder virtual
// functions named by offset up to GetLightVolume's slot in
// __vt__11CHierObject, the members and flag names are inferred, the part
// table only fills the gap as in hierobject_weak.cpp, and the CVector3 view
// copies through a double pair (an init-list union copy). SwapOut follows:
// it installs the mesh, takes the motion's frame and key data (or clears
// them), then rebuilds the local box from the mesh's box: lengths of the
// three axes give width, height and depth (scaled 1.6, 1.7 and 1.4 with the
// centre nudged by 0.1 and -0.1 for a part of a tank, else 2.0), and the
// normalised axes and the centre become its basis and centre. The mesh,
// motion and box views and the tank scale names are inferred. The rest of
// the file is not part of this unit.
class BPDLightVolume;

class CLightVolumeManager {
public:
    BPDLightVolume* GetVolume();

    unsigned char unknown00[4];
};

enum EBSEventEnum {};

union CVector3Pair {
    double pair[2];
    float v[4];
};

extern inline float sqrtf(float x)
{
    const double _half = .5;
    const double _three = 3.0;
    volatile float y;
    if (x > 0.0f)
    {
        double guess = __frsqrte((double)x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        y = (float)(x*guess);
        return y;
    }
    return x;
}

class CVector3 {
public:
    CVector3(const CVector3& other) : d(other.d) {}
    float Length() const { return sqrtf(d.v[0] * d.v[0] + d.v[1] * d.v[1] + d.v[2] * d.v[2]); }
    void Normalize() {
        float length = Length();
        if (length != 0.0f) {
            float scale = 1.0f / length;
            d.v[0] *= scale;
            d.v[1] *= scale;
            d.v[2] *= scale;
        }
    }
    CVector3& operator=(const CVector3& other) {
        d = other.d;
        return *this;
    }

    union {
        double pair[2];
        float v[4];
    } d;
} __attribute__((aligned(8)));

class CVolBox {
public:
    void SetWidth(float);
    void SetHeight(float);
    void SetDepth(float);
    void SetBasis(CVector3, CVector3, CVector3);
    void SetCenter(CVector3);

    unsigned char unknown00[16];
    CVector3 m_axes[3];
    CVector3 m_center;
};

/* inferred: the mesh's data record and its box (+32) */
struct MeshDataView {
    unsigned char unknown00[32];
    CVolBox* m_box;
};

class CStaticMesh {
public:
    unsigned char unknown00[4];
    MeshDataView* m_data;
};

class CStaticAnimMesh {
public:
    int numFrames() const;
    int ticksPerFrame() const;
};

/* inferred: the motion record (key ticks, key count, transform flag, keys) */
struct MotionData {
    unsigned char unknown00[4];
    int ticks;
    int count;
    int transform;
    unsigned char keys[4];
};

class CHierObject;

class CQuaternion {
public:
    float w;
    float x;
    float y;
    float z;
};

class CStaticObject {
public:
    virtual void vf008();
    virtual void vf00c();
    virtual void vf010();
    virtual void vf014();
    virtual void vf018();
    virtual void vf01c();
    virtual void vf020();
    virtual void vf024();
    virtual void vf028();
    virtual void vf02c();
    virtual void vf030();
    virtual void vf034();
    virtual void vf038();
    virtual void vf03c();
    virtual void vf040();
    virtual void vf044();
    virtual void vf048();
    virtual void vf04c();
    virtual void vf050();
    virtual void vf054();
    virtual void vf058();
    virtual void vf05c();
    virtual void vf060();
    virtual void vf064();
    virtual void vf068();
    virtual void vf06c();
    virtual void vf070();
    virtual void vf074();
    virtual void vf078();
    virtual void vf07c();
    virtual void vf080();
    virtual void vf084();
    virtual void vf088();
    virtual void vf08c();
    virtual void vf090();
    virtual void vf094();
    virtual void vf098();
    virtual void vf09c();
    virtual void vf0a0();
    virtual void vf0a4();
    virtual void vf0a8();
    virtual void vf0ac();
    virtual void vf0b0();
    virtual void vf0b4();
    virtual void vf0b8();
    virtual void vf0bc();
    virtual void vf0c0();
    virtual void vf0c4();
    virtual void vf0c8();
    virtual void vf0cc();
    virtual void vf0d0();
    virtual void vf0d4();
    virtual void vf0d8();
    virtual void vf0dc();
    virtual void vf0e0();
    virtual void vf0e4();
    virtual void vf0e8();
    virtual void vf0ec();
    virtual void vf0f0();
    virtual void vf0f4();
    virtual void vf0f8();
    virtual void vf0fc();
    virtual void vf100();
    virtual void vf104();
    virtual void vf108();
    virtual void vf10c();
    virtual void vf110();
    virtual void vf114();
    virtual void vf118();
    virtual void vf11c();
    virtual BPDLightVolume* GetLightVolume();
    virtual void vf124();
    virtual void vf128();
    virtual void vf12c();
    virtual void vf130();
    virtual void vf134();
    virtual void vf138();
    virtual void vf13c();
    virtual void vf140();
    virtual void vf144();
    virtual CHierObject* AsHierTankObject();
};

class CHierObject : public CStaticObject {
public:
    virtual ~CHierObject();
    void StartMotionPlayback(int, int, EBSEventEnum);
    BPDLightVolume* GetLightVolume();
    void SwapOut(CStaticMesh*, MotionData*);

    struct DamageRange {
        int minimum;
        int maximum;
        float multiplier;
    };

    unsigned char unknown004[44];
    CStaticMesh* m_mesh;
    unsigned char unknown034[216];
    CVolBox* m_localBox;
    unsigned char unknown110[32];
    CVector3 m_keyOffset;
    CQuaternion m_keyRotation;
    unsigned char unknown150[48];
    CQuaternion m_scriptRotation;
    unsigned char unknown190[80];
    unsigned char m_flags1e0a : 4;
    unsigned char m_transformDelta : 1;
    unsigned char m_animMove : 1;
    unsigned char m_flags1e0b : 2;
    unsigned char unknown1e1[39];
    int m_frameCount;
    unsigned char unknown20c[4];
    int m_ticksPerFrame;
    unsigned char unknown214[4];
    int m_keyEvent;
    int m_startKey;
    int m_lastKey;
    int m_keyCount;
    unsigned char unknown228[4];
    int m_keyTicks;
    int m_key;
    unsigned char unknown234[20];
    void* m_keys;
    unsigned char unknown24c[20];
    CLightVolumeManager m_lightVolumes;
    unsigned char unknown264[796];
    unsigned char m_flags580[8];
    unsigned char m_flags588a : 2;
    unsigned char m_ownLight : 1;
    unsigned char m_flags588b : 5;
    unsigned char unknown589[7];
    CHierObject* m_parent;
    unsigned char unknown594[12];
    CHierObject* m_parts[52];
    CVector3 m_animPosition;
    unsigned char unknown680[168];
    DamageRange m_damageRanges[2];
};

void CHierObject::StartMotionPlayback(int frame, int end, EBSEventEnum event) {
    if (!m_keys)
        return;
    if (frame < 0)
        frame = 0;
    if (frame >= m_keyCount)
        frame = m_keyCount - 1;
    if (end < 0)
        end = 0;
    if (end >= m_keyCount)
        end = m_keyCount;
    m_startKey = frame;
    m_key = frame;
    m_lastKey = end;
    m_keyEvent = event;
    m_keyRotation = m_scriptRotation;
    m_keyOffset = m_animPosition;
    m_animMove = true;
}

BPDLightVolume* CHierObject::GetLightVolume() {
    if (m_ownLight)
        return m_lightVolumes.GetVolume();
    return m_parent->GetLightVolume();
}

void CHierObject::SwapOut(CStaticMesh* mesh, MotionData* motion) {
    m_mesh = mesh;
    if (motion) {
        CStaticAnimMesh* anim = (CStaticAnimMesh*)mesh;
        m_frameCount = anim->numFrames();
        m_ticksPerFrame = anim->ticksPerFrame();
        m_keyCount = motion->count;
        m_keyTicks = motion->ticks;
        m_transformDelta = motion->transform != 0;
        m_keys = motion->keys;
    } else {
        m_frameCount = 0;
        m_ticksPerFrame = 0;
        m_keyCount = 0;
        m_keyTicks = 0;
        m_transformDelta = 0;
        m_keys = 0;
    }
    CVolBox* source = m_mesh->m_data->m_box;
    CVector3 a(source->m_axes[0]);
    CVector3 b(source->m_axes[1]);
    CVector3 c(source->m_axes[2]);
    CVector3 center(source->m_center);
    CVolBox* box = m_localBox;
    if (m_parent->AsHierTankObject()) {
        center.d.v[0] += 0.0f * a.Length();
        center.d.v[1] += 0.1f * c.Length();
        center.d.v[2] += -0.1f * b.Length();
        box->SetWidth(1.6f * a.Length());
        box->SetHeight(1.7f * c.Length());
        box->SetDepth(1.4f * b.Length());
    } else {
        box->SetWidth(2.0f * a.Length());
        box->SetHeight(2.0f * c.Length());
        box->SetDepth(2.0f * b.Length());
    }
    a.Normalize();
    b.Normalize();
    c.Normalize();
    box->SetBasis(a, b, c);
    box->SetCenter(center);
}
