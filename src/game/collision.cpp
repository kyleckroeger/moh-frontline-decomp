// collision.cpp: CCollision. ClearContacts returns every contact to the
// file's contact pool and counts them off. Test asks the first node's volume
// (of the kind the collision selects) to test the second's: in single mode
// once; otherwise it bisects the time step (at most MAX_NUM_LOOPS halvings
// down to TIME_STEP_EPSILON) until a contact is found, reporting the fraction
// of the step reached, and when none converges it resets both nodes to the
// start of the step, excludes the pair from further collision and clears the
// contacts. Then SwapOrder, AddContact, the assignment operator, the
// destructor, the two constructors and the file's static initialisation.
// Contacts are taken from the contact pool (g_contactPool over the static
// g_aClsnContacts array, both file-local) and linked through the word at +44.
// The pool's constructor runs its inline Init, which chains every element onto
// the free list from the last one down; the free-list address and the element
// slot are indexed by two counters (the slot by a pre-decremented one), as the
// original keeps two offsets. The pool's destructor is the weak template copy
// that ends the file; the contact's empty constructor, passed to the array
// construction, is a weak copy whose original sits in csg_volume.cpp.
// CCollision, SClsnContact, dwi::CFactory, the statics and the called
// functions are named by the symbols; the members, the pool's inline helpers,
// the scene node's virtual slots and the result codes are inferred. The pool's
// Allocate constructs the element with an empty placement new, which leaves
// the null compare without a branch seen in the original. The assignment
// copies a contact field by field and leaves the link alone (inferred), and
// the destructor repeats the ClearContacts loop rather than calling it inline
// (calling it changes register assignment).
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

extern "C" int sprintf(char*, const char*, ...);

class CCollision;

class IVolume {
public:
    virtual void vf08();
    virtual void vf0c();
    virtual void vf10();
    virtual int TestCollision(const IVolume&, CCollision&, bool) const;
};

class CAnimController {
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
    virtual void vf40();
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
    virtual void vf9c();
    virtual void vfa0();
    virtual void vfa4();
    virtual void vfa8();
    virtual void vfac();
    virtual void vfb0();
    virtual void vfb4();
    virtual void vfb8();
    virtual void vfbc();
    virtual void vfc0();
    virtual void vfc4();
    virtual void vfc8();
    virtual void vfcc();
    virtual void vfd0();
    virtual void vfd4();
    virtual void vfd8();
    virtual void vfdc();
    virtual void Reset();
};

class CPlayerObject {
public:
    unsigned char unknown000[916];
    unsigned char flag394a : 1;
    unsigned char flag394b : 1;
    unsigned char flag394c : 1;
    unsigned char m_collided : 1;
};

class ISceneNode {
public:
    virtual void MarkForDestruction(int);
    virtual ~ISceneNode();
    virtual void Destroy();
    virtual void BeginUpdate(float);
    virtual void UpdateAI(float);
    virtual void CommitAI();
    virtual void ConstrainVelocity();
    virtual void AttemptUpdate(float);
    virtual void OnCollision(const void*);
    virtual void CommitUpdate();
    virtual void Draw(void*);
    virtual IVolume* GetLocalBoundingVolume(int) const;
    virtual IVolume* GetWorldBoundingVolume(int) const;
    virtual void vf3c();
    virtual void vf40();
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
    virtual CAnimController* GetAnimController();
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
    virtual CPlayerObject* GetPlayerObject();
};

#include "game/CScene.h"


extern CScene g_scene;
extern const float TIME_STEP_EPSILON;

class CCollision {
public:
    CCollision();
    CCollision(const CCollision& other);
    ~CCollision();
    CCollision& operator=(const CCollision& other);
    void ClearContacts();
    int Test(float&);
    void SwapOrder();
    void AddContact(const SClsnContact& contact);

    ISceneNode* m_first;
    ISceneNode* m_second;
    float m_time;
    float m_scale;
    unsigned char m_scaleTime : 1;
    unsigned char m_single : 1;
    unsigned char m_volume : 2;
    int data14;
    SClsnContact* m_contacts;
    int m_numContacts;
};

void CCollision::ClearContacts() {
    while (m_contacts) {
        SClsnContact* next = m_contacts->m_next;
        g_contactPool.Free(m_contacts);
        m_contacts = next;
        m_numContacts--;
    }
}

int CCollision::Test(float& fraction) {
    static bool isCSFWarningEnabled = true;
    int volumes[3] = {0, 1, 2};
    int volume = volumes[m_volume];
    IVolume* first = m_first->GetWorldBoundingVolume(volume);
    IVolume* second = m_second->GetWorldBoundingVolume(volume);
    int result = 0;
    if (m_single) {
        result = first->TestCollision(*second, *this, false);
    } else {
        float start = m_time;
        float lo = 0.0f;
        float hi = start;
        static const int MAX_NUM_LOOPS = (int)(1.0f / TIME_STEP_EPSILON) + 1;
        int i;
        for (i = 1; i <= MAX_NUM_LOOPS; i++) {
            if (m_time < TIME_STEP_EPSILON)
                break;
            int hit = first->TestCollision(*second, *this, i == 1);
            if (hit > result)
                result = hit;
            switch (hit) {
            case 2:
                fraction = m_time / start;
                if (m_scaleTime)
                    m_time *= m_scale;
                goto found;
            case 1:
                lo = m_time;
                m_scaleTime = 0;
                if (i == 1)
                    goto found;
                break;
            case 3:
                hi = m_time;
                ClearContacts();
                break;
            }
            m_time = 0.5f * (lo + hi);
            m_first->AttemptUpdate(m_time);
            first = m_first->GetWorldBoundingVolume(volume);
            m_second->AttemptUpdate(m_time);
            second = m_second->GetWorldBoundingVolume(volume);
        }
        if (isCSFWarningEnabled) {
            char message[260];
            sprintf(message, "\n\tContact search failed (inner loop): %d of %d iterations", i, MAX_NUM_LOOPS);
            isCSFWarningEnabled = false;
        }
        CAnimController* controller = m_first->GetAnimController();
        if (controller)
            controller->Reset();
        m_first->AttemptUpdate(0.0f);
        m_first->CommitUpdate();
        controller = m_second->GetAnimController();
        if (controller)
            controller->Reset();
        m_second->AttemptUpdate(0.0f);
        m_second->CommitUpdate();
        g_scene.ExcludeCollision(*m_first, *m_second, 10);
        CPlayerObject* player1 = m_first->GetPlayerObject();
        CPlayerObject* player2 = m_second->GetPlayerObject();
        if (player1)
            player1->m_collided = 0;
        if (player2)
            player2->m_collided = 0;
        data14 = 0;
        m_scale = 1.0f;
        ClearContacts();
        return 1;
    found:
        isCSFWarningEnabled = true;
    }
    return result;
}

void CCollision::SwapOrder() {
    ISceneNode* first = m_first;
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
        m_time = other.m_time;
        m_scale = other.m_scale;
        m_scaleTime = other.m_scaleTime;
        m_single = other.m_single;
        m_volume = other.m_volume;
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
    : m_first(other.m_first), m_second(other.m_second), m_time(other.m_time), m_scale(other.m_scale),
      m_scaleTime(other.m_scaleTime), m_single(other.m_single), m_volume(other.m_volume), data14(other.data14),
      m_contacts(0), m_numContacts(0) {
    for (SClsnContact* c = other.m_contacts; c; c = c->m_next)
        AddContact(*c);
}

CCollision::CCollision()
    : m_first(0), m_second(0), m_time(-1.0f), m_scale(1.0f), m_scaleTime(0), m_single(0), m_volume(0),
      data14(0), m_contacts(0), m_numContacts(0) {
}
