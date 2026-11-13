// world_object.cpp (0x800b64c4..0x800b8418): CWorldObject, the scene node that owns the world's
// compartments and streams them in. The identity and scene-node queries (AsWorldObject as weak
// in-class copies), DrawShadows/Draw over the compartment list, BeginUpdate (services the data
// streamer while requests are pending, then stores the world volume's extents in the sweep-extent
// records ISceneNode's words +0x0c..+0x18 point at), HandleBulletCollision (texture swap on the
// hit compartment, impact frame, particles, decal, sound and AI noise, pass-through),
// ServiceDataStreamer (the compartment load state machine: buffer, CPT and CDB reads in chunks
// of 524288 bytes per unit of time, then the compartment pushed and added to the world volume),
// HandleExplosion, GetCompartment, Acquire, the destructor and the constructor, the weak
// dwi::list<CWorldObject::SRequest>/dwi::list<CCompartment> reserve and destructors,
// dwi::list_node<CCompartment>'s and dwi::CMemFactory's weak destructors, the two big-file
// filename hooks and the static initialiser of the filename tables (with CFilenameTable's weak
// destructor). Merged from the fragments world_object_draw, world_object_explosion,
// world_object_dtor, world_object_request_reserve, world_object_list_reserve,
// world_object_list, world_object_node_dtor and world_object; it defines __vt__12CWorldObject,
// the file's RTTI records and the class's statics.
//
// Names from the mangled symbols and the vtables (__vt__12CWorldObject, __vt__12CCompartment).
// Inferred: CWorldObject's members past ISceneNode (the memory factory +0x24, the compartment
// list +0x40, the request list +0xc8, the world volume +0x110, the draw flag +0x160, the load
// progress +0x168 and pending count +0x16c), SRequest's fields, the dwi::list/CMemFactory
// layouts and inline members, and the contact, surface, particle-system and bullet views.
// CCompartment is a 104-byte ISceneNode view; GetLocalBoundingVolume(0) gives its CDB object.
//
// The file compiles with RTTI on and deferred inlining, so the definitions below are written
// last to first (the compiler emits them in reverse; the original numbers the destructor-chain
// records and RTTI records before every function's extab label). Every list member is defined
// in the class, so the weak reserve and destructor copies come out right after the constructor,
// their first user. The range constructor of dwi::list is split into the inline helpers
// init_free() and link_range() (as in pumhandler.cpp and alarm.cpp), takes the sentinel from
// the end iterator and the tail from the sentinel ('head->m_prev'), and end() computes the
// sentinel's address from 'this' (+16): with that, reserve<SRequest> keeps the sentinel in its
// own register and reserve<CCompartment> loads the tail through 'this', as in the original.
// The list empties itself through clear(), which destroys each node with the node's destructor
// (a direct ~CCompartment call). SRequest keeps its reuse flag as a 4-byte array (Acquire builds
// the request on the stack and copies it word by word).
#include "game/CVector3.h"
#include "game/CMatrix.h"
#include "game/ISceneNode.h"

class CExplosion;
class CAIFilterObject;
class CCDBObject;
class CPTHeader;
class CDB;
class CTriangle;
class CPlane;
class CLine3;
class CVolBox;
class CVolSphere;
class CVolCapsule;
class CVolFiber;
class CAnimatedVolume;
class CWorldVolume;

extern "C" void MEM_free(void*);
void* DWI_allocalign(const char*, int, int, int);
int GetCompartmentSizeFromIndex(int);
void GetCompartmentCDBNameFromIndex(int, char*, int);
void GetCompartmentCPTNameFromIndex(int, char*, int);
void GetCptFilenameFromBigFile(int, char*, int);
void GetCdbFilenameFromBigFile(int, char*, int);
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

inline void* operator new(unsigned long, void* place) {
    return place;
}
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


class IVolume {
public:
    IVolume() {}
    virtual ~IVolume() {}
    virtual IVolume* Create() const;
    virtual void TransformedCopy(const IVolume&, const CMatrix&);
    virtual int TestCollision(const IVolume&, CCollision&, bool) const;
    virtual int TestCollision(const CVolBox&, CCollision&, bool) const;
    virtual int TestCollision(const CVolSphere&, CCollision&, bool) const;
    virtual int TestCollision(const CVolCapsule&, CCollision&, bool) const;
    virtual int TestCollision(const CCDBObject&, CCollision&, bool) const;
    virtual int TestCollision(const CVolFiber&, CCollision&, bool) const;
    virtual int TestCollision(const CAnimatedVolume&, CCollision&, bool) const;
    virtual int TestCollision(const CWorldVolume&, CCollision&, bool) const;
    virtual int TestCollision(CVector3, CCollision&, bool) const;
    virtual int TestCollision(const CLine3&, CCollision&, bool) const;
    virtual int TestCollision(const CPlane&, CCollision&, bool) const;
    virtual int TestCollision(const CTriangle&, CCollision&, bool) const;
    virtual bool TestVisibility(CDrawContext&) const;
    virtual void GetExtents(CVector3&, CVector3&) const;
};

class CWorldVolume : public IVolume {
public:
    CWorldVolume(int);
    virtual ~CWorldVolume();
    void AddSubVolume(const CCDBObject&);
    void RemoveSubVolume(const CCDBObject&);

    unsigned char unknown04[76];
};

class CCompartment : public ISceneNode {
public:
    CCompartment();
    CCompartment(const CCompartment&);
    virtual ~CCompartment();
    virtual void Draw(CDrawContext&);
    virtual IVolume* GetLocalBoundingVolume(EVolumeType) const;
    virtual IVolume* GetWorldBoundingVolume(EVolumeType) const;
    virtual bool IsVisible(CDrawContext&) const;
    virtual bool IsDrawEnabled() const;
    void DrawShadows(CDrawContext&);
    void HandleExplosion(const CExplosion&);
    void Init(int, char*, CPTHeader*, CDB*);
    bool TextureSwapCollisionDisabled(int);
    void TextureSwap(int);
    static void LoadShared();
    static int GetSharedSize();
    CCDBObject* GetCDBObject() { return (CCDBObject*)GetLocalBoundingVolume((EVolumeType)0); }

    unsigned char unknown24[44];
    int m_index;
    char* m_buffer;
    unsigned char unknown58[8];
    bool m_active;
    bool m_loaded;
} __attribute__((aligned(8)));

/* inferred: the extent record a scene node's words +0x0c..+0x18 point at */
struct WorldBoundView {
    unsigned char unknown0[8];
    float key;
};

/* inferred: the size word of a loaded file header (+4) */
struct LoadedHeaderView {
    unsigned char unknown00[4];
    int size;
};

namespace dwi {
template <class T>
struct list_node {
    list_node(const T& data) : m_prev(this), m_next(this), m_data(data) {}
    list_node(list_node* prev, list_node* next, const T& data) : m_prev(prev), m_next(next), m_data(data) {}

    list_node* m_prev;
    list_node* m_next;
    T m_data;
};

template <class T>
class list_iter {
public:
    list_iter(list_node<T>* node) : m_node(node) {}
    list_iter(const list_iter& other) : m_node(other.m_node) {}
    T& operator*() const { return m_node->m_data; }
    list_iter& operator++() {
        m_node = m_node->m_next;
        return *this;
    }
    bool operator!=(const list_iter& other) const { return m_node != other.m_node; }

    list_node<T>* m_node;
};

template <class T>
class list {
public:
    list() : m_count(0), m_capacity(0), m_pool(0), m_free(0), m_head(T()) { m_owned = true; }
    template <class I>
    list(int capacity, I first, I last, char* name)
        : m_count(0), m_capacity(capacity),
          m_pool((list_node<T>*)DWI_allocalign(name, capacity * sizeof(list_node<T>), 16, 1024)), m_free(0),
          m_head(&m_head, &m_head, T()) {
        m_owned = true;
        init_free();
        link_range(end(), first, last);
    }
    template <class I2> void link_range(I2 end, I2 first, I2 last) {
        list_node<T>* head = end.m_node;
        list_node<T>* stop = last.m_node;
        list_node<T>* tail = head->m_prev;
        for (I2 it = first; it.m_node != stop; ++it) {
            const T& value = *it;
            list_node<T>* added = new (alloc_node()) list_node<T>(0, 0, value);
            tail->m_next = added;
            added->m_prev = tail;
            tail = added;
            m_count++;
        }
        tail->m_next = head;
        head->m_prev = tail;
    }
    void init_free() {
        list_node<T>* next = 0;
        list_node<T>* node = m_pool + (m_capacity - 1);
        while (node != m_pool) {
            node->m_next = next;
            next = node;
            node--;
        }
        node->m_next = next;
        m_free = node;
    }
    list_node<T>* alloc_node() {
        list_node<T>* node = m_free;
        m_free = node->m_next;
        return node;
    }
    list_iter<T> begin() { return list_iter<T>(m_head.m_next); }
    list_iter<T> end() { return list_iter<T>((list_node<T>*)((char*)this + 16)); }
    void clear() {
        list_node<T>* node = m_head.m_next;
        list_node<T>* next;

        while (node != &m_head) {
            next = node->m_next;
            node->~list_node<T>();
            node->m_prev = 0;
            node->m_next = m_free;
            m_free = node;
            node = next;
        }
        m_head.m_next = &m_head;
        m_head.m_prev = &m_head;
        m_count = 0;
    }
    ~list() {
        clear();
        if (m_owned)
            MEM_free(m_pool);
        m_pool = 0;
    }

    void swap(list& other) {
        int count = m_count;
        m_count = other.m_count;
        other.m_count = count;
        int capacity = m_capacity;
        m_capacity = other.m_capacity;
        other.m_capacity = capacity;
        list_node<T>* pool = m_pool;
        m_pool = other.m_pool;
        other.m_pool = pool;
        list_node<T>* free = m_free;
        m_free = other.m_free;
        other.m_free = free;
        bool owned = m_owned;
        m_owned = other.m_owned;
        other.m_owned = owned;
        if (m_count != 0) {
            if (other.m_count != 0) {
                list_node<T>* prev = m_head.m_prev;
                m_head.m_prev = other.m_head.m_prev;
                other.m_head.m_prev = prev;
                list_node<T>* next = m_head.m_next;
                m_head.m_next = other.m_head.m_next;
                other.m_head.m_next = next;
                m_head.m_prev->m_next = &m_head;
                other.m_head.m_prev->m_next = &other.m_head;
            } else {
                other.m_head.m_prev = m_head.m_prev;
                other.m_head.m_next = m_head.m_next;
                other.m_head.m_prev->m_next = &other.m_head;
                m_head.m_next = &m_head;
                m_head.m_prev = &m_head;
            }
        } else if (other.m_count != 0) {
            m_head.m_prev = other.m_head.m_prev;
            m_head.m_next = other.m_head.m_next;
            m_head.m_prev->m_next = &m_head;
            other.m_head.m_next = &other.m_head;
            other.m_head.m_prev = &other.m_head;
        }
    }

    void reserve(int capacity, char* name) {
        if (capacity > m_capacity) {
            list<T> grown(capacity, begin(), end(), name);
            swap(grown);
        }
    }

    int m_count;
    int m_capacity;
    list_node<T>* m_pool;
    list_node<T>* m_free;
    list_node<T> m_head;
    bool m_owned;
};

class CMemFactory {
public:
    CMemFactory() : m_data(0), m_free(0), m_size(0), m_count(0), m_used(0), m_owned(false) {}
    void Init(void* data, int size, int count) {
        m_data = data;
        m_free = 0;
        m_size = size;
        m_count = count;
        m_used = 0;
        m_owned = true;
        for (int i = count - 1; i >= 0; i--) {
            *(void**)((char*)m_data + m_size * i) = (void*)m_free;
            m_free = (int)((char*)m_data + m_size * i);
        }
    }
    ~CMemFactory() {
        if (m_owned)
            MEM_free(m_data);
        m_data = 0;
        m_free = 0;
        m_size = 0;
        m_count = 0;
        m_used = 0;
    }

    void* m_data;
    int m_free;
    int m_size;
    int m_count;
    int m_used;
    bool m_owned;
};
}

class CWorldObject : public ISceneNode {
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

    /* inferred: a compartment load request */
    struct SRequest {
        int m_index;
        int m_state;
        int m_file;
        int m_async;
        unsigned char m_reuse[4];
        char* m_buffer;
        char* m_cursor;
        int m_offset;
        CPTHeader* m_cpt;
        CDB* m_cdb;
    };

    typedef dwi::list_node<CCompartment> CompartmentNode;
    typedef dwi::list_node<SRequest> RequestNode;

    CWorldObject();
    virtual ~CWorldObject();
    virtual void BeginUpdate(float);
    virtual void Draw(CDrawContext&);
    virtual IVolume* GetLocalBoundingVolume(EVolumeType) const;
    virtual IVolume* GetWorldBoundingVolume(EVolumeType) const;
    virtual bool IsVisible(CDrawContext&) const;
    virtual bool IsDrawEnabled() const;
    virtual EClsnId GetCollisionId() const;
    virtual void HandleBulletCollision(CBullet*, const CCollision&);
    virtual CWorldObject* AsWorldObject() { return this; }
    virtual const CWorldObject* AsWorldObject() const { return this; }

    /* inferred: the request list's push (an insert before pos) */
    void PushRequest(const SRequest& request, RequestNode* pos) {
        RequestNode* node;
        RequestNode* prev = pos->m_prev;
        node = m_requests.m_free;
        m_requests.m_free = node->m_next;
        if (node) {
            node->m_prev = 0;
            node->m_next = 0;
            node->m_data = request;
        }
        m_requests.m_count++;
        prev->m_next = node;
        node->m_prev = prev;
        node->m_next = pos;
        pos->m_prev = node;
    }
    void PushCompartment(CompartmentNode* pos, const CCompartment& value) {
        CompartmentNode* prev = pos->m_prev;
        CompartmentNode* node = m_compartments.m_free;
        m_compartments.m_free = node->m_next;
        CompartmentNode* added = new (node) CompartmentNode(0, 0, value);
        m_compartments.m_count++;
        prev->m_next = added;
        added->m_prev = prev;
        added->m_next = pos;
        pos->m_prev = added;
    }

    void DrawShadows(CDrawContext&);
    void ServiceDataStreamer(float);
    void HandleExplosion(const CExplosion&);
    CCompartment* GetCompartment(int);
    void Acquire(const SAcquireSet&, bool);
    static int m_sCompartmentSize;
    static void (*GetCptFilename)(int, char*, int);
    static void (*GetCdbFilename)(int, char*, int);

    dwi::CMemFactory m_factory;
    unsigned char unknown3c[4];
    dwi::list<CCompartment> m_compartments;
    dwi::list<SRequest> m_requests;
    unsigned char unknown10c[4];
    CWorldVolume m_volume;
    bool m_drawEnabled;
    int m_unknown164;
    float m_progress;
    int m_pending;
};

class CFilenameTable {
public:
    CFilenameTable() : m_count(0), m_names(0) {}
    ~CFilenameTable() { delete[] m_names; }

    int m_count;
    char* m_names;
};
// GetCptFilenameFromBigFile
void GetCptFilenameFromBigFile(int index, char* name, int size) {
    GetCompartmentCPTNameFromIndex(index, name, size);
}

// GetCdbFilenameFromBigFile
void GetCdbFilenameFromBigFile(int index, char* name, int size) {
    GetCompartmentCDBNameFromIndex(index, name, size);
}

// ctor
CWorldObject::CWorldObject() : m_volume(4) {
    m_drawEnabled = true;
    m_unknown164 = 0;
    m_compartments.reserve(4, "World Object Cache");
    m_requests.reserve(4, "World Object Requests");
    CCompartment::LoadShared();
    m_sCompartmentSize = (0x900000 - CCompartment::GetSharedSize()) / 4;
    int largest = 0;
    for (int i = 0;; i++) {
        int size = GetCompartmentSizeFromIndex(i);
        if (size > largest)
            largest = size;
        else if (size < 0)
            break;
    }
    if (largest < m_sCompartmentSize)
        m_sCompartmentSize = largest;
    m_sCompartmentSize += 128;
    m_factory.Init(DWI_allocalign(0, m_sCompartmentSize * 4, 16, 1024), m_sCompartmentSize, 4);
}

// dtor
CWorldObject::~CWorldObject() {
}

// Acquire
void CWorldObject::Acquire(const SAcquireSet& set, bool reset) {
    if (reset) {
        m_progress = 0.0f;
        m_pending = 0;
    }
    RequestNode* it = m_requests.m_head.m_next;
    RequestNode* node;
    RequestNode* end = &m_requests.m_head;
    while (it != end) {
        node = it;
        it = it->m_next;
        if (set.Lacks(node->m_data.m_index)) {
            ASYNCFILE_cancel(node->m_data.m_async);
            FILESYS_closesync(node->m_data.m_file, 100);
            char** buffer = (char**)node->m_data.m_buffer;
            if (buffer) {
                m_factory.m_used--;
                *buffer = (char*)m_factory.m_free;
                m_factory.m_free = (int)buffer;
            }
            RequestNode* next;
            RequestNode* prev;
            prev = node->m_prev;
            next = node->m_next;
            prev->m_next = next;
            next->m_prev = prev;
            node->m_prev = 0;
            node->m_next = m_requests.m_free;
            m_requests.m_free = node;
            m_requests.m_count--;
        }
    }
    CompartmentNode* cend = &m_compartments.m_head;
    for (CompartmentNode* c = m_compartments.m_head.m_next; c != cend; c = c->m_next) {
        bool lacks = true;
        const int* p = set.m_indices;
        for (int j = 0; j < 4 && lacks; p++, j++)
            lacks = *p != c->m_data.m_index;
        c->m_data.m_loaded = lacks;
    }
    for (int i = 0; i < 4; i++) {
        int index = set.m_indices[i];
        CCompartment* compartment;
        CompartmentNode* c2;
        for (c2 = m_compartments.m_head.m_next; c2 != cend; c2 = c2->m_next) {
            if (index == c2->m_data.m_index) {
                compartment = &c2->m_data;
                goto found;
            }
        }
        compartment = 0;
    found:
        if (compartment)
            continue;
        RequestNode* rn;
        bool requested = false;
        for (rn = m_requests.m_head.m_next; rn != end; rn = rn->m_next) {
            if (index == rn->m_data.m_index) {
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
// GetCompartment
CCompartment* CWorldObject::GetCompartment(int index) {
    CompartmentNode* node = m_compartments.m_head.m_next;
    CompartmentNode* end = &m_compartments.m_head;
    for (; node != end; node = node->m_next) {
        if (index == node->m_data.m_index)
            return &node->m_data;
    }
    return 0;
}

// HandleExplosion
void CWorldObject::HandleExplosion(const CExplosion& explosion) {
    CompartmentNode* node = m_compartments.m_head.m_next;
    CompartmentNode* end = &m_compartments.m_head;
    for (; node != end; node = node->m_next)
        node->m_data.HandleExplosion(explosion);
}

// ServiceDataStreamer
void CWorldObject::ServiceDataStreamer(float dt) {
    RequestNode* request;
    CompartmentNode* node;
    CompartmentNode* end;
    int chunk;
    bool done;
    char cptName[128];
    char cdbName[128];

    if (dt < 1.0f)
        dt = 1.0f;
    end = &m_compartments.m_head;
    done = false;
    chunk = (int)(524288.0f * dt);
    while (!done && m_requests.m_count != 0) {
        request = m_requests.m_head.m_next;
        switch (request->m_data.m_state) {
        case 0:
            if (m_factory.m_used < m_factory.m_count) {
                char** buffer = (char**)m_factory.m_free;
                m_factory.m_free = (int)*buffer;
                m_factory.m_used++;
                request->m_data.m_buffer = (char*)buffer;
            } else {
                CompartmentNode* node;
                for (node = m_compartments.m_head.m_next; node != end; node = node->m_next) {
                    if (node->m_data.m_loaded)
                        break;
                }
                CCompartment* compartment = &node->m_data;
                request->m_data.m_buffer = compartment->m_buffer;
                request->m_data.m_reuse[0] = true;
                compartment->m_active = false;
                CCDBObject* object = (CCDBObject*)compartment->GetLocalBoundingVolume((EVolumeType)0);
                m_volume.RemoveSubVolume(*object);
                done = true;
            }
            request->m_data.m_cursor = request->m_data.m_buffer;
            request->m_data.m_state = 1;
            break;
        case 1:
            if (request->m_data.m_reuse[0]) {
                for (node = m_compartments.m_head.m_next; node != end; node = node->m_next) {
                    if (node->m_data.m_buffer == request->m_data.m_buffer) {
                        CompartmentNode* next;
                        CompartmentNode* prev = node->m_prev;
                        next = node->m_next;
                        prev->m_next = next;
                        next->m_prev = prev;
                        node->~CompartmentNode();
                        node->m_prev = 0;
                        node->m_next = m_compartments.m_free;
                        m_compartments.m_free = node;
                        m_compartments.m_count--;
                        break;
                    }
                }
            }
            request->m_data.m_state = 2;
            break;
        case 2: {
            request->m_data.m_cursor = (char*)(((int)request->m_data.m_cursor + 127) & ~127);
            request->m_data.m_offset = 0;
            request->m_data.m_cpt = (CPTHeader*)request->m_data.m_cursor;
            request->m_data.m_state = 3;
            GetCptFilename(request->m_data.m_index, cptName, 128);
            FILESYS_opensync(cptName, 33, 100, &request->m_data.m_file);
            break;
        }
        case 3:
            request->m_data.m_async = ASYNCFILE_read(request->m_data.m_file, request->m_data.m_offset, request->m_data.m_cursor, chunk);
            request->m_data.m_state = 4;
            done = true;
            break;
        case 4:
            if (ASYNCFILE_getstatus(request->m_data.m_async) == 1) {
                int got = 0;
                ASYNCFILE_release(request->m_data.m_async, 0, &got);
                request->m_data.m_cursor += got;
                request->m_data.m_offset += got;
                m_progress += (float)got / (float)((LoadedHeaderView*)request->m_data.m_cpt)->size;
                if (request->m_data.m_cursor - (char*)request->m_data.m_cpt < ((LoadedHeaderView*)request->m_data.m_cpt)->size) {
                    request->m_data.m_state = 3;
                } else {
                    FILESYS_closesync(request->m_data.m_file, 100);
                    request->m_data.m_state = 5;
                }
            } else {
                done = true;
            }
            break;
        case 5: {
            request->m_data.m_cursor = (char*)(((int)request->m_data.m_cursor + 15) & ~15);
            request->m_data.m_offset = 0;
            request->m_data.m_cdb = (CDB*)request->m_data.m_cursor;
            request->m_data.m_state = 6;
            GetCdbFilename(request->m_data.m_index, cdbName, 128);
            FILESYS_opensync(cdbName, 33, 100, &request->m_data.m_file);
            break;
        }
        case 6:
            request->m_data.m_async = ASYNCFILE_read(request->m_data.m_file, request->m_data.m_offset, request->m_data.m_cursor, chunk);
            request->m_data.m_state = 7;
            done = true;
            break;
        case 7:
            if (ASYNCFILE_getstatus(request->m_data.m_async) == 1) {
                int got = 0;
                int status = 0;
                ASYNCFILE_release(request->m_data.m_async, &status, &got);
                request->m_data.m_cursor += got;
                request->m_data.m_offset += got;
                m_progress += (float)got / (float)((LoadedHeaderView*)request->m_data.m_cdb)->size;
                if (request->m_data.m_cursor - (char*)request->m_data.m_cdb < ((LoadedHeaderView*)request->m_data.m_cdb)->size) {
                    request->m_data.m_state = 6;
                } else {
                    FILESYS_closesync(request->m_data.m_file, 100);
                    request->m_data.m_state = 8;
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
            CCompartment* added = &m_compartments.m_head.m_prev->m_data;
            added->Init(request->m_data.m_index, request->m_data.m_buffer, request->m_data.m_cpt, request->m_data.m_cdb);
            CCDBObject* object = (CCDBObject*)added->GetLocalBoundingVolume((EVolumeType)0);
            m_volume.AddSubVolume(*object);
            done = true;
            RequestNode* next;
            RequestNode* prev;
            RequestNode* first = m_requests.m_head.m_next;
            prev = first->m_prev;
            next = first->m_next;
            prev->m_next = next;
            next->m_prev = prev;
            first->m_prev = 0;
            first->m_next = m_requests.m_free;
            m_requests.m_free = first;
            m_requests.m_count--;
            break;
        }
        }
    }
}

// HandleBulletCollision
void CWorldObject::HandleBulletCollision(CBullet* bullet, const CCollision& collision) {
    ContactView* contact;
    CompartmentNode* node;
    CompartmentNode* end;
    bool enabled;
    SurfaceView* surface;
    enabled = true;
    contact = collision.m_contacts;
    surface = contact->m_surface;
    if (surface->m_texSwap >= 0) {
        end = &m_compartments.m_head;
        for (node = m_compartments.m_head.m_next; node != end; node = node->m_next) {
            if (contact->m_object == node->m_data.GetCDBObject()) {
                enabled = !node->m_data.TextureSwapCollisionDisabled(surface->m_texSwap);
                if (enabled)
                    node->m_data.TextureSwap(surface->m_texSwap);
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

// BeginUpdate
void CWorldObject::BeginUpdate(float dt) {
    CVector3 low;
    CVector3 high;

    if (m_requests.m_count)
        ServiceDataStreamer(dt);
    m_volume.GetExtents(low, high);
    ((WorldBoundView*)data0c)->key = low.x;
    ((WorldBoundView*)data14)->key = low.y;
    ((WorldBoundView*)data10)->key = high.x;
    ((WorldBoundView*)data18)->key = high.y;
}

// Draw
void CWorldObject::Draw(CDrawContext& context) {
    CompartmentNode* node = m_compartments.m_head.m_next;
    CompartmentNode* end = &m_compartments.m_head;
    for (; node != end; node = node->m_next) {
        if (node->m_data.IsDrawEnabled() && node->m_data.IsVisible(context))
            node->m_data.Draw(context);
    }
}

// DrawShadows
void CWorldObject::DrawShadows(CDrawContext& context) {
    CompartmentNode* node = m_compartments.m_head.m_next;
    CompartmentNode* end = &m_compartments.m_head;
    for (; node != end; node = node->m_next) {
        if (node->m_data.IsDrawEnabled() && node->m_data.IsVisible(context))
            node->m_data.DrawShadows(context);
    }
}

// GetLocalBoundingVolume
IVolume* CWorldObject::GetLocalBoundingVolume(EVolumeType) const {
    return (IVolume*)&m_volume;
}

// GetWorldBoundingVolume
IVolume* CWorldObject::GetWorldBoundingVolume(EVolumeType) const {
    return (IVolume*)&m_volume;
}

// IsVisible
bool CWorldObject::IsVisible(CDrawContext&) const {
    return true;
}

// IsDrawEnabled
bool CWorldObject::IsDrawEnabled() const {
    return m_drawEnabled;
}

// GetCollisionId
EClsnId CWorldObject::GetCollisionId() const {
    return (EClsnId)0;
}

// globals
int CWorldObject::m_sCompartmentSize;
void (*CWorldObject::GetCptFilename)(int, char*, int) = GetCptFilenameFromBigFile;
void (*CWorldObject::GetCdbFilename)(int, char*, int) = GetCdbFilenameFromBigFile;
CFilenameTable g_CptFnameTable;
CFilenameTable g_CdbFnameTable;

