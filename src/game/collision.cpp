// A fragment of collision.cpp (0x800c79c4): CCollision::SwapOrder, AddContact,
// the assignment operator, the destructor, the two constructors and the file's
// static initialisation. Contacts are taken from the file's contact pool
// (g_contactPool over the static g_aClsnContacts array, both file-local) and
// linked through the word at +44. The pool's constructor runs its inline Init,
// which chains every element onto the free list from the last one down; the
// free-list address and the element slot are indexed by two counters (the
// slot by a pre-decremented one), as the original keeps two offsets. The
// pool's destructor is the weak template copy placed by
// collision_factory_dtor.cpp; the contact's empty constructor, passed to the
// array construction, is a weak copy whose original sits in csg_volume.cpp. CCollision, SClsnContact and dwi::CFactory are named
// by the mangled symbols; their members and the pool's inline helpers are
// inferred from the code. The pool's Allocate constructs the element with an
// empty placement new, which leaves the null compare without a branch seen in
// the original. The assignment copies a contact field by field and leaves the
// link alone (inferred), and the destructor repeats the ClearContacts loop
// rather than calling it inline (calling it changes register assignment).
// The rest of the file is not part of this unit.
extern "C" void MEM_free(void*);
inline void* operator new(unsigned long, void* p) { return p; }
void* DWI_allocalign(const char*, int, int, int);

union CVector3Pair {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(const CVector3& other) { d.pair[0] = other.d.pair[0]; d.pair[1] = other.d.pair[1]; }

    CVector3Pair d;
} __attribute__((aligned(8)));

class IVolume;

struct SClsnContact {
    SClsnContact() {}
    SClsnContact& operator=(const SClsnContact& o) {
        m_depth = o.m_depth;
        m_point = o.m_point;
        m_normal = o.m_normal;
        data24 = o.data24;
        data28 = o.data28;
        return *this;
    }

    CVector3 m_point;
    CVector3 m_normal;
    float m_depth;
    int data24;
    int data28;
    SClsnContact* m_next;
};

namespace dwi {
template <class T, class P>
class CFactory {
public:
    CFactory(const char* name, T* storage, int count) { Init(name, storage, count); }
    ~CFactory();

    void Init(const char* name, T* storage, int count) {
        if (m_owned)
            MEM_free(m_memory);
        T* memory = !storage ? (T*)DWI_allocalign(name, count * sizeof(T), 16, 1024) : storage;
        m_free = 0;
        m_used = 0;
        m_memory = memory;
        m_capacity = count;
        m_owned = storage == 0;
        Reset();
    }
    void Reset() {
        m_free = 0;
        m_used = 0;
        int j = m_capacity;
        for (int i = m_capacity; i > 0;) {
            --j;
            *(P*)&m_memory[--i] = m_free;
            m_free = &m_memory[j];
        }
    }
    P Allocate() {
        P p = m_free;
        P next = *(P*)p;
        T* t = new (p) T;
        m_free = next;
        m_used++;
        return t;
    }
    void Free(P p) {
        m_used--;
        *(P*)p = m_free;
        m_free = p;
    }

    T* m_memory;
    P m_free;
    int m_capacity;
    int m_used;
    bool m_owned;
};

template <class T, class P> __declspec(weak) CFactory<T, P>::~CFactory() {
    if (m_owned)
        MEM_free(m_memory);
    m_memory = 0;
    m_free = 0;
    m_capacity = 0;
    m_used = 0;
}
}

static SClsnContact g_aClsnContacts[300];
static dwi::CFactory<SClsnContact, SClsnContact*> g_contactPool("Collision Contact Pool", g_aClsnContacts, 300);

class CCollision {
public:
    CCollision();
    CCollision(const CCollision& other);
    ~CCollision();
    CCollision& operator=(const CCollision& other);
    void ClearContacts();
    void SwapOrder();
    void AddContact(const SClsnContact& contact);

    IVolume* m_first;
    IVolume* m_second;
    float data08;
    float data0c;
    unsigned char flag10a : 1;
    unsigned char flag10b : 1;
    unsigned char field10 : 2;
    int data14;
    SClsnContact* m_contacts;
    int m_numContacts;
};

void CCollision::SwapOrder() {
    IVolume* first = m_first;
    m_first = m_second;
    m_second = first;
}

void CCollision::AddContact(const SClsnContact& contact) {
    SClsnContact* p = g_contactPool.Allocate();
    *p = contact;
    p->m_next = m_contacts;
    m_contacts = p;
    m_numContacts++;
}

CCollision& CCollision::operator=(const CCollision& other) {
    if (&other != this) {
        while (m_contacts) {
            SClsnContact* next = m_contacts->m_next;
            g_contactPool.Free(m_contacts);
            m_contacts = next;
        }
        m_contacts = 0;
        m_numContacts = 0;
        m_first = other.m_first;
        m_second = other.m_second;
        data08 = other.data08;
        data0c = other.data0c;
        flag10a = other.flag10a;
        flag10b = other.flag10b;
        field10 = other.field10;
        data14 = other.data14;
        for (SClsnContact* c = other.m_contacts; c; c = c->m_next)
            AddContact(*c);
    }
    return *this;
}

CCollision::~CCollision() {
    while (m_contacts) {
        SClsnContact* c = m_contacts;
        SClsnContact* next = c->m_next;
        g_contactPool.Free(c);
        m_contacts = next;
        m_numContacts--;
    }
}

CCollision::CCollision(const CCollision& other)
    : m_first(other.m_first), m_second(other.m_second), data08(other.data08), data0c(other.data0c),
      flag10a(other.flag10a), flag10b(other.flag10b), field10(other.field10), data14(other.data14),
      m_contacts(0), m_numContacts(0) {
    for (SClsnContact* c = other.m_contacts; c; c = c->m_next)
        AddContact(*c);
}

CCollision::CCollision()
    : m_first(0), m_second(0), data08(-1.0f), data0c(1.0f), flag10a(0), flag10b(0), field10(0),
      data14(0), m_contacts(0), m_numContacts(0) {
}
