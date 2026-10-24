// A fragment of particlesystem.cpp (0x8007f364): the CPropertyParticleSystem
// destructor (its table pointer, each base's through the inline CRenderBin
// and CParticleSystem destructors, the object freed when asked), then its
// constructor (0x8007f3d0): the inline CRenderBinData/CRenderBin and
// CParticleSystem constructors (the state bits set as member initialisers,
// so each is a word read-modify-write; the three tick counters from the
// manager's current time), two zero vectors and the property data, the
// rotation bit (either rotation speed non-zero), the matrix from the data's
// rotation and position, the data's velocity turned into local space, both
// extent pairs of the data sorted into minimum and maximum corners (one
// inline helper; the second pair is assigned through temporaries) and the
// texture bin registered for the data's shape. The file name is this
// project's; the original record is particlesystem.cpp. The classes and
// functions are named by the mangled symbols; the members, the data, pool and
// manager views and the helper names are inferred.
extern "C" int rand(void);

union CVector3Pair {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(float ax, float ay, float az) { d.v[0] = ax; d.v[1] = ay; d.v[2] = az; }
    void Set(float ax, float ay, float az) { d.v[0] = ax; d.v[1] = ay; d.v[2] = az; }
    CVector3(const CVector3& other) { d.pair[0] = other.d.pair[0]; d.pair[1] = other.d.pair[1]; }

    CVector3Pair d;
} __attribute__((aligned(8)));

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    void SetPos(CVector3);
    void FastInverse(const CMatrix&);
    void TransformVector(CVector3&, CVector3) const;

    float m[4][4];
    static bool s_ClassInit;
} __attribute__((aligned(16)));

class CQuaternion {
public:
    void GetMatrix(CMatrix&) const;

    float x;
    float y;
    float z;
    float w;
};

struct MOH_particle_Struct {
    unsigned char unknown00[16];
    float position[3];
    CQuaternion rotation;
    unsigned char unknown2c[28];
    unsigned long id;
    unsigned char unknown4c[68];
    float extents[12];
    float velocity[3];
    unsigned char unknownCC[24];
    float rotation_speed[2];
    void* shape;
};

class CDmaPacket;

struct CVelocity {
    CVelocity() {}
    CVelocity(float ax, float ay, float az) : x(ax), y(ay), z(az) {}
    float x, y, z;
};

class CRenderBin;

struct CRenderBinData {
    CRenderBinData() : m_prev(0), m_next(0), m_children(0), data0c(0), data10(0), m_priority(3) {}

    CRenderBin* m_prev;
    CRenderBin* m_next;
    CRenderBin* m_children;
    int data0c;
    int data10;
    int m_priority;
    int data18;
};

class CRenderBin : public CRenderBinData {
public:
    virtual ~CRenderBin() {}
};

struct CTextureBinView {
    CRenderBin* m_prev;
    CTextureBinView* m_next;
    unsigned char unknown08[88];
    void* m_shape;
};

template <class T> class CObjectPool {
public:
    T* Alloc() {
        T* obj = m_free;
        if (!obj)
            return 0;
        m_free = Next(obj);
        Next(obj) = m_used;
        m_used = obj;
        m_count++;
        return obj;
    }
    static T*& Next(T* obj) { return *(T**)((char*)obj + sizeof(T)); }

    void* m_data;
    T* m_used;
    T* m_free;
    int unknown0c;
    int m_count;
};

class CPropertyParticleSystem;
class CInstancedParticleSystem;

struct CParticleSystemManager {
    unsigned char unknown00[68];
    float m_currentTicks;
    MOH_particle_Struct* m_defs;
    int m_numDefs;
    CTextureBinView* m_bins;
    CObjectPool<CInstancedParticleSystem> m_instanced;
    CObjectPool<CPropertyParticleSystem> m_property;
};

extern CParticleSystemManager g_ParticleSystemManager;

class CParticleSystem : public CRenderBin {
public:
    CParticleSystem() : m_textureBin(0), m_deactivationTicks(0.0f), m_active(1), m_emitting(1), m_boundingBoxValid(0), m_moving(0), m_hasRotation(0) {
        float ticks = g_ParticleSystemManager.m_currentTicks;
        m_activationTicks = ticks;
        m_emitTicks = ticks;
        m_ticks = ticks;
    }
    virtual void DeActivate();
    virtual ~CParticleSystem() {}

    unsigned char unknown20[84];
    CTextureBinView* m_textureBin;
    float m_activationTicks;
    float m_deactivationTicks;
    float m_emitTicks;
    float m_ticks;
    unsigned int m_active : 1;
    unsigned int m_emitting : 1;
    unsigned int m_boundingBoxValid : 1;
    unsigned int m_moving : 1;
    unsigned int m_hasRotation : 1;
    unsigned int m_flags : 27;
    static CParticleSystem* Create(void*);
    static CParticleSystem* Create(unsigned long);
};

class CPropertyParticleSystem : public CParticleSystem {
public:
    CPropertyParticleSystem(MOH_particle_Struct*);
    virtual void Render(CDmaPacket&, void*);
    virtual ~CPropertyParticleSystem();
    virtual void v14();
    virtual void v18();
    virtual void v1c();
    virtual void v20();
    virtual void v24();
    virtual void v28();
    virtual void v2c();
    virtual void v30();
    virtual void v34();
    virtual void v38();
    virtual void v3c();
    virtual void v40();
    virtual void v44();
    virtual void v48();
    virtual void v4c();
    virtual void v50();
    virtual void v54();
    virtual void v58();
    virtual void v5c();
    virtual void v60();
    virtual void v64();
    virtual void v68();
    virtual void v6c();
    virtual void GetVelocity(CVector3&);
    void* operator new(unsigned long, void* p) { return p; }

    unsigned char unknown8c[4];
    CMatrix m_matrix;
    CVector3 m_v208;
    CVector3 m_v224;
    MOH_particle_Struct* m_props;
    unsigned char unknownF4[12];
};

class CInstancedParticleSystem : public CPropertyParticleSystem {
public:
    CInstancedParticleSystem(MOH_particle_Struct* props) : CPropertyParticleSystem(props) {
        m_seed = rand();
        m_velocity = CVelocity(props->velocity[0], props->velocity[1], props->velocity[2]);
    }
    virtual ~CInstancedParticleSystem();
    void* operator new(unsigned long, void* p) { return p; }

    int m_seed;
    unsigned char unknown104[4];
    CVelocity m_velocity;
    unsigned char unknown118[12];
};

static inline void Setup(CPropertyParticleSystem* ps) {
    ps->m_props->rotation.GetMatrix(ps->m_matrix);
    ps->m_matrix.SetPos(CVector3(ps->m_props->position[0], ps->m_props->position[1], ps->m_props->position[2]));
    CMatrix inverse;
    inverse.FastInverse(ps->m_matrix);
    CVector3 velocity;
    ps->GetVelocity(velocity);
    inverse.TransformVector(velocity, velocity);
    ps->m_props->velocity[0] = velocity.d.v[0];
    ps->m_props->velocity[1] = velocity.d.v[1];
    ps->m_props->velocity[2] = velocity.d.v[2];
}


CPropertyParticleSystem::~CPropertyParticleSystem() {
}

static inline CTextureBinView* FindBin(void* shape) {
    for (CTextureBinView* bin = g_ParticleSystemManager.m_bins; bin; bin = bin->m_next) {
        if (bin->m_shape == shape)
            return bin;
    }
    return 0;
}

static inline void SortExtents(CPropertyParticleSystem* ps, int i, const CVector3& a, const CVector3& b) {
    float lx = a.d.v[0] < b.d.v[0] ? a.d.v[0] : b.d.v[0];
    float ly = a.d.v[1] < b.d.v[1] ? a.d.v[1] : b.d.v[1];
    float lz = a.d.v[2] < b.d.v[2] ? a.d.v[2] : b.d.v[2];
    float hx = a.d.v[0] > b.d.v[0] ? a.d.v[0] : b.d.v[0];
    float hy = a.d.v[1] > b.d.v[1] ? a.d.v[1] : b.d.v[1];
    float hz = a.d.v[2] > b.d.v[2] ? a.d.v[2] : b.d.v[2];
    ps->m_props->extents[i + 0] = lx;
    ps->m_props->extents[i + 1] = ly;
    ps->m_props->extents[i + 2] = lz;
    ps->m_props->extents[i + 3] = hx;
    ps->m_props->extents[i + 4] = hy;
    ps->m_props->extents[i + 5] = hz;
}

CPropertyParticleSystem::CPropertyParticleSystem(MOH_particle_Struct* props) {
    m_v208.Set(0.0f, 0.0f, 0.0f);
    m_v224.Set(0.0f, 0.0f, 0.0f);
    m_props = props;
    m_hasRotation = 0.0f != m_props->rotation_speed[0] || 0.0f != m_props->rotation_speed[1];
    props->rotation.GetMatrix(m_matrix);
    m_matrix.SetPos(CVector3(props->position[0], props->position[1], props->position[2]));
    CMatrix inverse;
    inverse.FastInverse(m_matrix);
    CVector3 velocity;
    GetVelocity(velocity);
    inverse.TransformVector(velocity, velocity);
    m_props->velocity[0] = velocity.d.v[0];
    m_props->velocity[1] = velocity.d.v[1];
    m_props->velocity[2] = velocity.d.v[2];
    CVector3 a(m_props->extents[0], m_props->extents[1], m_props->extents[2]);
    CVector3 b(m_props->extents[3], m_props->extents[4], m_props->extents[5]);
    SortExtents(this, 0, a, b);
    a = CVector3(m_props->extents[6], m_props->extents[7], m_props->extents[8]);
    b = CVector3(m_props->extents[9], m_props->extents[10], m_props->extents[11]);
    SortExtents(this, 6, a, b);
    m_textureBin = FindBin(m_props->shape);
}
