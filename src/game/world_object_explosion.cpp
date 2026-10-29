// A fragment of world_object.cpp (0x800b66b4): CWorldObject::HandleBulletCollision
// first lets the compartment owning the hit object swap its texture (the
// surface's texture-swap index; a swap that disables collision lets the
// bullet pass), then places an impact frame on the bullet direction reflected
// about the contact normal, spawns the surface's particle system (projectiles
// and shoot-through surfaces; a default system for unknown surfaces), adds a
// bullet decal for projectiles, plays the impact sound with an AI noise and
// lets the bullet pass through shoot-through surfaces. The surface, contact,
// particle-system and bullet views are inferred.
// CWorldObject::ServiceDataStreamer
// runs the compartment load requests until one has to wait: a request takes a
// buffer from the pool or reuses the buffer of a loaded compartment (which is
// taken out of the world volume and then freed), reads the compartment's CPT
// file and then its CDB file asynchronously in chunks of 524288 bytes per
// unit of time (128- and 16-byte aligned, adding to the load progress), then
// builds the compartment at the end of the list (an inline push), adds it to
// the world volume and retires the request. The request and list views, the
// state numbers and the filename callbacks' arguments are inferred.
// CWorldObject::HandleExplosion
// passes the explosion to each of the world object's compartments, and
// GetCompartment finds the compartment with the given index (its word at
// +0x50, the texture-swap set of compartment_texswap.cpp), or none. The
// classes and functions are named by the mangled symbols; the compartment
// list view follows world_object_draw.cpp (the compartment is a 104-byte
// view here). The rest of the file is not part of this unit.
class CExplosion;
class CCollision;
class CAIFilterObject;
class CCDBObject;
class CPTHeader;
class CDB;

inline void* operator new(unsigned long, void* place) {
    return place;
}

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
    CVector3() {}
    CVector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
    void Scale(float f) {
        x = f * x;
        y = f * y;
        z = f * z;
    }
    float Dot(const CVector3& v) const { return x * v.x + y * v.y + z * v.z; }
    void Cross(const CVector3& a, const CVector3& b) {
        x = a.y * b.z - a.z * b.y;
        y = a.z * b.x - a.x * b.z;
        z = a.x * b.y - a.y * b.x;
    }
    float Length() const { return sqrtf(x * x + y * y + z * z); }
    void Normalize() {
        float length = Length();
        if (length != 0.0f) {
            x *= 1.0f / length;
            y *= 1.0f / length;
            z *= 1.0f / length;
        }
    }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    void Ident();
    void SetRight(CVector3);
    void SetFront(CVector3);
    void SetUp(CVector3);
    void SetPos(CVector3);

    float m[4][4];
    static bool s_ClassInit;
} __attribute__((aligned(16)));

/* inferred: a surface's packed type fields (+12) */
struct SurfaceView {
    unsigned char unknown00[12];
    signed int m_surface : 11;
    signed int m_texSwap : 11;
    unsigned int m_rest : 10;
};

/* inferred: a collision contact (normal, position, surface, object) */
struct ContactView {
    CVector3 m_normal;
    CVector3 m_position;
    unsigned char unknown20[4];
    SurfaceView* m_surface;
    CCDBObject* m_object;
};

class CCollision {
public:
    unsigned char unknown00[24];
    ContactView* m_contacts;
};

class CSurfaceType {
public:
    static bool IsShootThru(int);
    static int GetParticleCRC(int);
    static int GetDecalIndex(int);
    static int GetSoundIndex(int);
};

/* inferred: a particle system's placement (slot +140 of the table at +28) */
class ParticleSystemView {
public:
    unsigned char unknown00[28];

    virtual void v008();
    virtual void v00c();
    virtual void v010();
    virtual void v014();
    virtual void v018();
    virtual void v01c();
    virtual void v020();
    virtual void v024();
    virtual void v028();
    virtual void v02c();
    virtual void v030();
    virtual void v034();
    virtual void v038();
    virtual void v03c();
    virtual void v040();
    virtual void v044();
    virtual void v048();
    virtual void v04c();
    virtual void v050();
    virtual void v054();
    virtual void v058();
    virtual void v05c();
    virtual void v060();
    virtual void v064();
    virtual void v068();
    virtual void v06c();
    virtual void v070();
    virtual void v074();
    virtual void v078();
    virtual void v07c();
    virtual void v080();
    virtual void v084();
    virtual void v088();
    virtual void SetLocalToWorld(const CMatrix&);
};

class CParticleSystem {
public:
    static ParticleSystemView* Create(unsigned long);
};

class CBulletDecalManager {
public:
    void AddBulletDecal(const CCollision&, bool);

    unsigned char unknown00[1024];
};

extern CBulletDecalManager g_BulletDecalManager;

/* inferred: the shooter's AI view (+4) */
struct ShooterAIView {
    unsigned char unknown00[4];
    CAIFilterObject* m_filter;
};

class ShooterView {
public:
    virtual void v008();
    virtual void v00c();
    virtual void v010();
    virtual void v014();
    virtual void v018();
    virtual void v01c();
    virtual void v020();
    virtual void v024();
    virtual void v028();
    virtual void v02c();
    virtual void v030();
    virtual void v034();
    virtual void v038();
    virtual void v03c();
    virtual void v040();
    virtual void v044();
    virtual void v048();
    virtual void v04c();
    virtual void v050();
    virtual void v054();
    virtual void v058();
    virtual void v05c();
    virtual void v060();
    virtual void v064();
    virtual void v068();
    virtual void v06c();
    virtual void v070();
    virtual void v074();
    virtual void v078();
    virtual void v07c();
    virtual void v080();
    virtual void v084();
    virtual void v088();
    virtual void v08c();
    virtual void v090();
    virtual void v094();
    virtual void v098();
    virtual void v09c();
    virtual void v0a0();
    virtual void v0a4();
    virtual void v0a8();
    virtual void v0ac();
    virtual void v0b0();
    virtual void v0b4();
    virtual void v0b8();
    virtual void v0bc();
    virtual void v0c0();
    virtual void v0c4();
    virtual void v0c8();
    virtual ShooterAIView* GetAI();
};

class CBullet {
public:
    virtual void v008();
    virtual void v00c();
    virtual void v010();
    virtual void v014();
    virtual void v018();
    virtual void v01c();
    virtual void v020();
    virtual void v024();
    virtual void v028();
    virtual void v02c();
    virtual void v030();
    virtual void v034();
    virtual void v038();
    virtual void v03c();
    virtual void v040();
    virtual void v044();
    virtual void GetForward(CVector3&) const;
    virtual void v04c();
    virtual void v050();
    virtual void v054();
    virtual void v058();
    virtual void v05c();
    virtual void v060();
    virtual void v064();
    virtual void v068();
    virtual void v06c();
    virtual void v070();
    virtual void v074();
    virtual void v078();
    virtual void v07c();
    virtual void v080();
    virtual void v084();
    virtual void v088();
    virtual void v08c();
    virtual void v090();
    virtual void v094();
    virtual void v098();
    virtual void v09c();
    virtual void v0a0();
    virtual void v0a4();
    virtual void v0a8();
    virtual void v0ac();
    virtual void v0b0();
    virtual void v0b4();
    virtual void v0b8();
    virtual void v0bc();
    virtual void v0c0();
    virtual void v0c4();
    virtual void v0c8();
    virtual void v0cc();
    virtual void v0d0();
    virtual void v0d4();
    virtual void v0d8();
    virtual void v0dc();
    virtual void v0e0();
    virtual void v0e4();
    virtual void v0e8();
    virtual void v0ec();
    virtual void v0f0();
    virtual void v0f4();
    virtual void v0f8();
    virtual void v0fc();
    virtual void v100();
    virtual void v104();
    virtual void v108();
    virtual void v10c();
    virtual void v110();
    virtual void v114();
    virtual void v118();
    virtual void v11c();
    virtual void v120();
    virtual void v124();
    virtual void v128();
    virtual void v12c();
    virtual void v130();
    virtual void v134();
    virtual ShooterView* GetFiredBy();
    virtual void v13c();
    virtual void* AsProjectile();
    virtual void v144();
    virtual void v148();
    virtual void PassThrough(const CCollision&);
};

void PlayBackgroundImpactSound(CBullet*, int);
void MakeNoise(void*, CAIFilterObject*, const CVector3&, int, float, bool, int);

extern "C" {
int FILESYS_opensync(const char*, int, int, int*);
int FILESYS_closesync(int, int);
int ASYNCFILE_cancel(int);
int ASYNCFILE_read(int, int, void*, int);
int ASYNCFILE_getstatus(int);
int ASYNCFILE_release(int, int*, int*);
}

class CCompartment {
public:
    CCompartment();
    CCompartment(const CCompartment&);
    ~CCompartment();
    void HandleExplosion(const CExplosion&);
    void Init(int, char*, CPTHeader*, CDB*);
    bool TextureSwapCollisionDisabled(int);
    void TextureSwap(int);

    virtual void unknown08();
    virtual void unknown0c();
    virtual void unknown10();
    virtual void unknown14();
    virtual void unknown18();
    virtual void unknown1c();
    virtual void unknown20();
    virtual void unknown24();
    virtual void unknown28();
    virtual void unknown2c();
    virtual void unknown30();
    virtual CCDBObject* GetCDBObject(int);

    unsigned char unknown04[76];
    int m_index;
    char* m_buffer;
    unsigned char unknown58[8];
    bool m_active;
    bool m_loaded;
    unsigned char unknown62[6];
} __attribute__((aligned(8)));

class CWorldVolume {
public:
    void AddSubVolume(const CCDBObject&);
    void RemoveSubVolume(const CCDBObject&);

    unsigned char unknown00[88];
};

/* inferred: a pending compartment load */
struct WorldRequestView {
    WorldRequestView* prev;
    WorldRequestView* next;
    int id;
    int state;
    int file;
    int read;
    bool reuse;
    unsigned char unknown19[3];
    char* buffer;
    char* cursor;
    int offset;
    CPTHeader* cpt;
    CDB* cdb;
};

struct WorldCompartmentNodeView {
    WorldCompartmentNodeView(WorldCompartmentNodeView* p, WorldCompartmentNodeView* n, const CCompartment& c)
        : prev(p), next(n), compartment(c) {}

    WorldCompartmentNodeView* prev;
    WorldCompartmentNodeView* next;
    CCompartment compartment;
};

struct WorldListLinkView {
    WorldCompartmentNodeView* prev;
    WorldCompartmentNodeView* next;
};

struct WorldRequestLinkView {
    WorldRequestView* prev;
    WorldRequestView* next;
};

/* inferred: the size word of a loaded file header (+4) */
struct LoadedHeaderView {
    unsigned char unknown00[4];
    int size;
};

class CWorldObject {
public:
    struct SAcquireSet {
        /* inferred: whether the set lacks the index */
        bool Lacks(const int& index) const {
            int i;
            bool lacks;
            lacks = true;
            const int* p = m_indices;
            for (i = 0; i < 4 && lacks; p++, i++)
                lacks = *p != index;
            return lacks;
        }

        int m_indices[4];
    };

    /* inferred: a compartment load request (the request view's payload) */
    struct SRequest {
        int m_index;
        int m_state;
        int m_file;
        int m_async;
        unsigned char m_reuse[4];
        char** m_buffer;
        char* m_cursor;
        int m_offset;
        CPTHeader* m_cpt;
        CDB* m_cdb;
    };

    struct RequestNodeView {
        RequestNodeView* prev;
        RequestNodeView* next;
        SRequest request;
    };

    /* inferred: the request list's push (an insert before pos) */
    void PushRequest(const SRequest& request, RequestNodeView* pos) {
        RequestNodeView* node;
        RequestNodeView* prev = pos->prev;
        node = (RequestNodeView*)m_requestFree;
        m_requestFree = (WorldRequestView*)node->next;
        if (node) {
            node->prev = 0;
            node->next = 0;
            node->request = request;
        }
        m_requestCount++;
        prev->next = node;
        node->prev = prev;
        node->next = pos;
        pos->prev = node;
    }

    void HandleBulletCollision(CBullet*, const CCollision&);
    void ServiceDataStreamer(float);
    void PushCompartment(WorldCompartmentNodeView* pos, const CCompartment& value) {
        WorldCompartmentNodeView* prev = pos->prev;
        WorldCompartmentNodeView* node = m_compartmentFree;
        m_compartmentFree = node->next;
        WorldCompartmentNodeView* added = new (node) WorldCompartmentNodeView(0, 0, value);
        m_compartmentCount++;
        prev->next = added;
        added->prev = prev;
        added->next = pos;
        pos->prev = added;
    }
    WorldCompartmentNodeView* AllocCompartmentNode() {
        WorldCompartmentNodeView* node = m_compartmentFree;
        m_compartmentFree = node->next;
        return node;
    }
    void HandleExplosion(const CExplosion&);
    CCompartment* GetCompartment(int);
    void Acquire(const SAcquireSet&, bool);
    static void (*GetCptFilename)(int, char*, int);
    static void (*GetCdbFilename)(int, char*, int);

    unsigned char unknown000[40];
    char** m_bufferFree;
    unsigned char unknown02c[4];
    int m_bufferCount;
    int m_buffersUsed;
    unsigned char unknown038[8];
    int m_compartmentCount;
    unsigned char unknown044[8];
    WorldCompartmentNodeView* m_compartmentFree;
    WorldListLinkView m_compartments;
    unsigned char unknown058[112];
    int m_requestCount;
    unsigned char unknown0cc[8];
    WorldRequestView* m_requestFree;
    WorldRequestLinkView m_requests;
    unsigned char unknown0e0[48];
    CWorldVolume m_world;
    float m_progress;
    int m_pending;
};

void CWorldObject::HandleBulletCollision(CBullet* bullet, const CCollision& collision) {
    ContactView* contact;
    WorldCompartmentNodeView* node;
    WorldCompartmentNodeView* end;
    bool enabled;
    SurfaceView* surface;
    enabled = true;
    contact = collision.m_contacts;
    surface = contact->m_surface;
    if (surface->m_texSwap >= 0) {
        end = (WorldCompartmentNodeView*)&m_compartments;
        for (node = m_compartments.next; node != end; node = node->next) {
            if (contact->m_object == node->compartment.GetCDBObject(0)) {
                enabled = !node->compartment.TextureSwapCollisionDisabled(surface->m_texSwap);
                if (enabled)
                    node->compartment.TextureSwap(surface->m_texSwap);
                break;
            }
        }
    }
    if (enabled) {
        CVector3 right;
        CVector3 front;
        CMatrix matrix;
        matrix.Ident();
        CVector3 velocity;
        bullet->GetForward(velocity);
        float d = contact->m_normal.Dot(velocity);
        CVector3 up(-2.0f * d * contact->m_normal.x, -2.0f * d * contact->m_normal.y, -2.0f * d * contact->m_normal.z);
        up.x += velocity.x;
        up.y += velocity.y;
        up.z += velocity.z;
        up.Normalize();
        bool decal = false;
        if (d > 0.0001f)
            decal = true;
        if ((float)__fabs(up.Dot(CVector3(1.0f, 0.0f, 0.0f))) <= 0.9999f) {
            front.Cross(up, CVector3(1.0f, 0.0f, 0.0f));
            front.Normalize();
            right.Cross(front, up);
        } else {
            right.Cross(up, CVector3(0.0f, 0.0f, 1.0f));
            right.Normalize();
            front.Cross(up, right);
        }
        matrix.SetRight(right);
        matrix.SetFront(front);
        matrix.SetUp(up);
        matrix.SetPos(contact->m_position);
        bool projectile = bullet->AsProjectile() != 0;
        if (surface->m_surface >= 0) {
            bool shootThru = CSurfaceType::IsShootThru(surface->m_surface);
            if (projectile || shootThru) {
                int crc = CSurfaceType::GetParticleCRC(surface->m_surface);
                if (crc) {
                    ParticleSystemView* system = CParticleSystem::Create(crc);
                    if (system)
                        system->SetLocalToWorld(matrix);
                }
            }
            if (projectile) {
                if (CSurfaceType::GetDecalIndex(surface->m_surface) != -1)
                    g_BulletDecalManager.AddBulletDecal(collision, decal);
            }
            int sound = CSurfaceType::GetSoundIndex(surface->m_surface);
            if (sound) {
                PlayBackgroundImpactSound(bullet, sound);
                CAIFilterObject* filter = 0;
                if (bullet->GetFiredBy())
                    filter = bullet->GetFiredBy()->GetAI()->m_filter;
                MakeNoise(0, filter, contact->m_position, 52, 0.6f, false, 2);
            }
            if (shootThru)
                bullet->PassThrough(collision);
        } else {
            if (projectile) {
                ParticleSystemView* system = CParticleSystem::Create(0xe9ac3f2e);
                if (system)
                    system->SetLocalToWorld(matrix);
            }
            PlayBackgroundImpactSound(bullet, 1);
            CAIFilterObject* filter = 0;
            if (bullet->GetFiredBy())
                filter = bullet->GetFiredBy()->GetAI()->m_filter;
            MakeNoise(0, filter, contact->m_position, 52, 0.6f, false, 2);
        }
    } else {
        bullet->PassThrough(collision);
    }
}

void CWorldObject::ServiceDataStreamer(float dt) {
    WorldRequestView* request;
    WorldCompartmentNodeView* node;
    WorldCompartmentNodeView* end;
    int chunk;
    bool done;
    char cptName[128];
    char cdbName[128];

    if (dt < 1.0f)
        dt = 1.0f;
    end = (WorldCompartmentNodeView*)&m_compartments;
    done = false;
    chunk = (int)(524288.0f * dt);
    while (!done && m_requestCount != 0) {
        request = m_requests.next;
        switch (request->state) {
        case 0:
            if (m_buffersUsed < m_bufferCount) {
                char** buffer = m_bufferFree;
                m_bufferFree = (char**)*buffer;
                m_buffersUsed++;
                request->buffer = (char*)buffer;
            } else {
                WorldCompartmentNodeView* node;
                for (node = m_compartments.next; node != end; node = node->next) {
                    if (node->compartment.m_loaded)
                        break;
                }
                CCompartment* compartment = &node->compartment;
                request->buffer = compartment->m_buffer;
                request->reuse = true;
                compartment->m_active = false;
                CCDBObject* object = compartment->GetCDBObject(0);
                m_world.RemoveSubVolume(*object);
                done = true;
            }
            request->cursor = request->buffer;
            request->state = 1;
            break;
        case 1:
            if (request->reuse) {
                for (node = m_compartments.next; node != end; node = node->next) {
                    if (node->compartment.m_buffer == request->buffer) {
                        WorldCompartmentNodeView* next;
                        WorldCompartmentNodeView* prev = node->prev;
                        next = node->next;
                        prev->next = next;
                        next->prev = prev;
                        if (node)
                            node->compartment.~CCompartment();
                        node->prev = 0;
                        node->next = m_compartmentFree;
                        m_compartmentFree = node;
                        m_compartmentCount--;
                        break;
                    }
                }
            }
            request->state = 2;
            break;
        case 2: {
            request->cursor = (char*)(((int)request->cursor + 127) & ~127);
            request->offset = 0;
            request->cpt = (CPTHeader*)request->cursor;
            request->state = 3;
            GetCptFilename(request->id, cptName, 128);
            FILESYS_opensync(cptName, 33, 100, &request->file);
            break;
        }
        case 3:
            request->read = ASYNCFILE_read(request->file, request->offset, request->cursor, chunk);
            request->state = 4;
            done = true;
            break;
        case 4:
            if (ASYNCFILE_getstatus(request->read) == 1) {
                int got = 0;
                ASYNCFILE_release(request->read, 0, &got);
                request->cursor += got;
                request->offset += got;
                m_progress += (float)got / (float)((LoadedHeaderView*)request->cpt)->size;
                if (request->cursor - (char*)request->cpt < ((LoadedHeaderView*)request->cpt)->size) {
                    request->state = 3;
                } else {
                    FILESYS_closesync(request->file, 100);
                    request->state = 5;
                }
            } else {
                done = true;
            }
            break;
        case 5: {
            request->cursor = (char*)(((int)request->cursor + 15) & ~15);
            request->offset = 0;
            request->cdb = (CDB*)request->cursor;
            request->state = 6;
            GetCdbFilename(request->id, cdbName, 128);
            FILESYS_opensync(cdbName, 33, 100, &request->file);
            break;
        }
        case 6:
            request->read = ASYNCFILE_read(request->file, request->offset, request->cursor, chunk);
            request->state = 7;
            done = true;
            break;
        case 7:
            if (ASYNCFILE_getstatus(request->read) == 1) {
                int got = 0;
                int status = 0;
                ASYNCFILE_release(request->read, &status, &got);
                request->cursor += got;
                request->offset += got;
                m_progress += (float)got / (float)((LoadedHeaderView*)request->cdb)->size;
                if (request->cursor - (char*)request->cdb < ((LoadedHeaderView*)request->cdb)->size) {
                    request->state = 6;
                } else {
                    FILESYS_closesync(request->file, 100);
                    request->state = 8;
                }
            } else {
                done = true;
            }
            break;
        case 8: {
            {
                CCompartment compartment;
                PushCompartment(end, compartment);
            }
            CCompartment* added = &m_compartments.prev->compartment;
            added->Init(request->id, request->buffer, request->cpt, request->cdb);
            CCDBObject* object = added->GetCDBObject(0);
            m_world.AddSubVolume(*object);
            done = true;
            WorldRequestView* next;
            WorldRequestView* prev;
            WorldRequestView* first = m_requests.next;
            prev = first->prev;
            next = first->next;
            prev->next = next;
            next->prev = prev;
            first->prev = 0;
            first->next = m_requestFree;
            m_requestFree = first;
            m_requestCount--;
            break;
        }
        }
    }
}

void CWorldObject::HandleExplosion(const CExplosion& explosion) {
    WorldCompartmentNodeView* node = m_compartments.next;
    WorldCompartmentNodeView* end = (WorldCompartmentNodeView*)&m_compartments;
    for (; node != end; node = node->next)
        node->compartment.HandleExplosion(explosion);
}

CCompartment* CWorldObject::GetCompartment(int index) {
    WorldCompartmentNodeView* node = m_compartments.next;
    WorldCompartmentNodeView* end = (WorldCompartmentNodeView*)&m_compartments;
    for (; node != end; node = node->next) {
        if (index == node->compartment.m_index)
            return &node->compartment;
    }
    return 0;
}

void CWorldObject::Acquire(const SAcquireSet& set, bool reset) {
    if (reset) {
        m_progress = 0.0f;
        m_pending = 0;
    }
    RequestNodeView* it = (RequestNodeView*)m_requests.next;
    RequestNodeView* node;
    RequestNodeView* end = (RequestNodeView*)&m_requests;
    while (it != end) {
        node = it;
        it = it->next;
        if (set.Lacks(node->request.m_index)) {
            ASYNCFILE_cancel(node->request.m_async);
            FILESYS_closesync(node->request.m_file, 100);
            char** buffer = node->request.m_buffer;
            if (buffer) {
                m_buffersUsed--;
                *buffer = (char*)m_bufferFree;
                m_bufferFree = buffer;
            }
            RequestNodeView* next;
            RequestNodeView* prev;
            prev = node->prev;
            next = node->next;
            prev->next = next;
            next->prev = prev;
            node->prev = 0;
            node->next = (RequestNodeView*)m_requestFree;
            m_requestFree = (WorldRequestView*)node;
            m_requestCount--;
        }
    }
    WorldCompartmentNodeView* cend = (WorldCompartmentNodeView*)&m_compartments;
    for (WorldCompartmentNodeView* c = m_compartments.next; c != cend; c = c->next) {
        bool lacks = true;
        const int* p = set.m_indices;
        for (int j = 0; j < 4 && lacks; p++, j++)
            lacks = *p != c->compartment.m_index;
        c->compartment.m_loaded = lacks;
    }
    for (int i = 0; i < 4; i++) {
        int index = set.m_indices[i];
        CCompartment* compartment;
        WorldCompartmentNodeView* c2;
        for (c2 = m_compartments.next; c2 != cend; c2 = c2->next) {
            if (index == c2->compartment.m_index) {
                compartment = &c2->compartment;
                goto found;
            }
        }
        compartment = 0;
    found:
        if (compartment)
            continue;
        RequestNodeView* rn;
        bool requested = false;
        for (rn = (RequestNodeView*)m_requests.next; rn != end; rn = rn->next) {
            if (index == rn->request.m_index) {
                requested = true;
                break;
            }
        }
        if (requested)
            continue;
        SRequest request;
        request.m_state = 0;
        request.m_index = index;
        request.m_file = 0;
        request.m_async = 0;
        request.m_reuse[0] = 0;
        request.m_buffer = 0;
        request.m_cursor = 0;
        request.m_offset = 0;
        request.m_cpt = 0;
        request.m_cdb = 0;
        PushRequest(request, end);
        m_pending++;
    }
}
