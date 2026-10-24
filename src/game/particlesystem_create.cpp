// A fragment of particlesystem.cpp (0x80080b30): the two CParticleSystem::Create
// functions. Create(void*) takes an object from the manager's property pool
// (the free list's link is stored just past the object), constructs it in
// place, sets its matrix from the data's rotation and position and turns the
// data's velocity into local space through the matrix's fast inverse (one
// inline setup helper; that gives the original's stack slots). Create(unsigned
// long) rejects one id, then finds the registered definition with that id and
// builds an instanced system for it from the instanced pool (inline
// constructor: random seed, velocity copied from the definition; the search
// sits in an inline helper, so every result goes through one register). The
// file name is this project's; the original record is particlesystem.cpp. The
// classes, functions and globals are named by the mangled symbols; the members,
// the pool and manager views and the helper names are inferred.
extern "C" int rand(void);

union CVector3Pair {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(float ax, float ay, float az) { d.v[0] = ax; d.v[1] = ay; d.v[2] = az; }
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
    unsigned char unknown4c[116];
    float velocity[3];
    unsigned char unknownCC[36];
};

class CDmaPacket;

struct CVelocity {
    CVelocity() {}
    CVelocity(float ax, float ay, float az) : x(ax), y(ay), z(az) {}
    float x, y, z;
};

class CRenderBin {
    unsigned char unknown00[28];

public:
    virtual ~CRenderBin() {}
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
    unsigned char unknown00[72];
    MOH_particle_Struct* m_defs;
    int m_numDefs;
    unsigned char unknown50[4];
    CObjectPool<CInstancedParticleSystem> m_instanced;
    CObjectPool<CPropertyParticleSystem> m_property;
};

extern CParticleSystemManager g_ParticleSystemManager;

class CParticleSystem : public CRenderBin {
public:
    virtual void DeActivate();
    virtual ~CParticleSystem() {}
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

    unsigned char unknown20[112];
    CMatrix m_matrix;
    unsigned char unknownD0[32];
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

CParticleSystem* CParticleSystem::Create(void* data) {
    CPropertyParticleSystem* ps = g_ParticleSystemManager.m_property.Alloc();
    if (ps) {
        ps = new (ps) CPropertyParticleSystem((MOH_particle_Struct*)data);
        Setup(ps);
    } else {
        ps = 0;
    }
    return ps;
}

static inline CParticleSystem* CreateInstance(unsigned long id) {
    for (int i = 0; i < g_ParticleSystemManager.m_numDefs; i++) {
        if (id == g_ParticleSystemManager.m_defs[i].id) {
            CParticleSystem* ps = g_ParticleSystemManager.m_instanced.Alloc();
            if (ps) {
                ps = new (ps) CInstancedParticleSystem(&g_ParticleSystemManager.m_defs[i]);
            } else {
                ps = 0;
            }
            return ps;
        }
    }
    return 0;
}

CParticleSystem* CParticleSystem::Create(unsigned long id) {
    if (id == 0x47f5df95)
        return 0;
    return CreateInstance(id);
}
