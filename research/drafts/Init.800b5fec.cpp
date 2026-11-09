// CThrownObject::CleanUp (a fragment of thrown_obj.cpp): free the local and
// world bounding spheres (once when they are the same) and clear the sphere
// and collider pointers. The names come from the mangled symbols;
// ISceneNode's virtual functions follow __vt__13CThrownObject (slots through
// GetWorldBoundingVolume, +48 as a placeholder) and CThrownObject declares
// OnCollision first so this file does not emit its vtable. BSGO_Basic keeps
// its three words before its vtable pointer, as the constructors store them;
// its first virtual entry is a placeholder. The members are inferred from
// offsets (the mesh-constructor that sets the orientation reads constants
// pooled with the rest of the file and is not part of these fragments).
class CCollision;
class CStaticMesh;
class IVolume;

namespace std {
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
}

union CVector3Pair {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(const CVector3& other) { *(CVector3Pair*)this = *(const CVector3Pair*)&other; }
    float Length() const { float a = x; return std::sqrtf(a * a + y * y + this->z * this->z); }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CVolSphere {
public:
    void SetRadius(float);
    void SetCenter(CVector3);
    CVolSphere& operator=(const CVolSphere&);
};

void* AllocateSphereVolume();

/* inferred: the mesh's bounding frame (axes and centre) */
struct ThrownMeshFrameView {
    unsigned char unknown00[16];
    CVector3 right;
    CVector3 front;
    CVector3 up;
    CVector3 centre;
};

struct ThrownMeshDataView {
    unsigned char unknown00[32];
    ThrownMeshFrameView* frame;
};

struct ThrownMeshView {
    unsigned char unknown00[4];
    ThrownMeshDataView* data;
};

/* inferred: the thrown-object record's values at +76 and +92 */
struct ThrownRecordView {
    unsigned char unknown00[76];
    int m_value4c;
    unsigned char unknown50[12];
    int m_value5c;
};

struct ThrownDefinitionView {
    unsigned char unknown00[8];
    ThrownRecordView* record;
};
struct LevelFileContentsStruct_;

class ISceneNode {
public:
    enum EVolumeType {};

    virtual void MarkForDestruction(int);
    virtual ~ISceneNode();
    virtual void Destroy();
    virtual void BeginUpdate(float);
    virtual void UpdateAI(float);
    virtual void CommitAI();
    virtual void ConstrainVelocity();
    virtual void AttemptUpdate(float);
    virtual void OnCollision(const CCollision&);
    virtual void CommitUpdate();
    virtual void unknown30();
    virtual void* GetLocalBoundingVolume(EVolumeType) const;
    virtual void* GetWorldBoundingVolume(EVolumeType) const;
};

class CStaticObject : public ISceneNode {
public:
    struct MotionFrame;

    CStaticObject(CStaticMesh*, int, int, int, int, int, MotionFrame*);
    CStaticObject();
    virtual ~CStaticObject();
    void Init();
    void SetLocalBoundingVolume(EVolumeType, IVolume*);
    void SetWorldBoundingVolume(EVolumeType, IVolume*);
};

class BSGO_Basic {
public:
    int m_field0;
private:
    int m_field4;
    int m_field8;

public:
    BSGO_Basic() : m_field0(0), m_field4(0), m_field8(0) {}
    virtual void unknown08();
};

class BSGO_Dummy : public BSGO_Basic {
public:
    virtual void unknown08();
};

void FreeSphereVolume(void*);

class CThrownObject : public CStaticObject {
public:
    void OnCollision(const CCollision&);
    void CleanUp();
    void Init();
    virtual ~CThrownObject();
    CThrownObject(CStaticMesh*, int, int, int, int, int, CStaticObject::MotionFrame*, LevelFileContentsStruct_*);
    CThrownObject();

    unsigned char unknown004[44];
    ThrownMeshView* m_mesh;
    unsigned char unknown034[216];
    void* m_sphere;
    void* m_collider;
    void* m_lastCollider;
    unsigned char unknown118[360];
    float m_value280;
    unsigned char unknown284[4];
    float m_vector288[3];
    unsigned char unknown294[20];
    float m_vector2a8[3];
    unsigned char unknown2b4[4];
    float m_orientationX;
    float m_orientationY;
    float m_orientationZ;
    float m_orientationW;
    ThrownDefinitionView* m_definition;
    int m_pendingSound;
    BSGO_Dummy m_scriptObject;
    CThrownObject* m_self;
    int m_value2e4;
    int m_value2e8;
    int m_value2ec;
    int m_value2f0;
    unsigned char flag0 : 1;
    unsigned char flag1 : 1;
    unsigned char flag2 : 1;
    unsigned char flag3 : 1;
    unsigned char deleted : 1;
    unsigned char markedForDestruction : 1;
    unsigned char flag6 : 1;
    unsigned char flag7 : 1;
    unsigned char unknown2f5[3];
    int m_value2f8;
    int m_value2fc;
};

void CThrownObject::CleanUp() {
    void* local = GetLocalBoundingVolume((EVolumeType)3);
    void* world;

    if (local)
        FreeSphereVolume(local);
    world = GetWorldBoundingVolume((EVolumeType)3);
    if (world && world != local)
        FreeSphereVolume(world);
    m_sphere = 0;
    m_lastCollider = 0;
}

void CThrownObject::Init() {
    ((CStaticObject*)this)->Init();
    m_value280 = m_definition->record->m_value5c;
    flag3 = m_definition->record->m_value4c != 0;
    m_vector288[0] = m_vector288[1] = m_vector288[2] = 0.0f;
    m_vector2a8[0] = m_vector2a8[1] = m_vector2a8[2] = 0.0f;
    flag0 = 1;
    flag1 = 0;
    flag2 = 0;
    deleted = 0;
    m_value2fc = 0;
    markedForDestruction = 0;
    m_value2f8 = -1;
    ThrownMeshFrameView* frame = m_mesh->data->frame;
    CVector3 right = frame->right;
    CVector3 front = frame->front;
    CVector3 up = frame->up;
    float a = right.Length();
    float b = front.Length();
    float c = up.Length();
    float radius = a > b ? a : b;
    radius = radius > c ? radius : c;
    CVolSphere* local = (CVolSphere*)AllocateSphereVolume();
    local->SetRadius(radius);
    local->SetCenter(m_mesh->data->frame->centre);
    SetLocalBoundingVolume((EVolumeType)3, (IVolume*)local);
    SetLocalBoundingVolume((EVolumeType)0, (IVolume*)local);
    CVolSphere* world = (CVolSphere*)AllocateSphereVolume();
    *world = *local;
    SetWorldBoundingVolume((EVolumeType)3, (IVolume*)world);
    SetWorldBoundingVolume((EVolumeType)0, (IVolume*)world);
    m_value2e4 = m_value2e8 = 0;
    m_value2ec = m_value2f0 = 0;
    m_scriptObject.m_field0 = 0;
    m_self = this;
    m_pendingSound = 0;
}
