// world_volume.cpp (0x800cbb80..0x800ccb7c): CWorldVolume, the collision volume that holds the
// world's CDB objects in a dwi::list of sub-volumes and tests other volumes against each of them.
// The weak TestCollision defaults (no collision), GetExtents, the TestCollision overloads (CSG
// and generic volumes swap the order and let the other volume test; the rest go through the
// TestCollisionWithSubVolumes template), both RemoveSubVolume overloads, AddSubVolume, the
// destructor, the weak dwi::list<const CCDBObject*> destructor, the constructor, the weak
// reserve and the six weak TestCollisionWithSubVolumes instances. Merged from the fragments
// world_volume, world_volume_edit, world_volume_remove, world_volume_dtor, world_volume_list,
// world_volume_ctor, world_volume_list_reserve and world_volume_subvol; it defines
// __vt__12CWorldVolume and the file's RTTI records.
//
// Names from the mangled symbols and the vtables (__vt__12CWorldVolume, __vt__7IVolume).
// Inferred: the member layout (the list at +4, the extents at +40 and +56, the word at +72),
// the dwi::list layout and inline members, the collision record view and the int results. The
// CVector3 view is file-private (four floats overlaid with two doubles), as plan rule 10 allows
// a merged unit where the shared header's copy doesn't keep the match.
//
// The file compiles with RTTI on and deferred inlining, so the definitions below are written
// last to first (the compiler emits them in reverse). The list's reserve is defined in the class
// and the TestCollisionWithSubVolumes instances are implicit (from the overloads that call them),
// so the weak copies come out after the constructor in the original's order: ~list, the
// constructor, reserve, then the six instances.
//
// RemoveAllSubVolumes is a gap function (accepted by the maintainer 2026-11-13, docs/Remaining.md):
// the original's extab labels leave room for functions the linker stripped between the
// destructor and the weak ~list, and the weak ~list is emitted right after ~CWorldVolume, which
// needs a function there that destroys a list out of line. Its name and body are this project's
// (empty the list by swapping it with an empty one); it is stripped at link, as in the original.
extern "C" void MEM_free(void*);
void* DWI_allocalign(const char*, int, int, int);
inline void* operator new(unsigned long, void* p) { return p; }

class CCollision;
class CTriangle;
class CPlane;
class CCDBObject;
class CMatrix;
class CLine3;
class CVolBox;
class CVolSphere;
class CVolCapsule;
class CVolFiber;
class CAnimatedVolume;
class CWorldVolume;

class CDrawContext;

/* inferred: four floats, 8-byte aligned, overlaid with two doubles: an
   implicit copy moves it as two lfd/stfd pairs, as GetExtents does */
class CVector3 {
public:
    union {
        double pair[2];
        float v[4];
    };
} __attribute__((aligned(8)));

/* inferred: the collision record (32 bytes; the depth at +12, the line flag at +16) */
class CCollision {
public:
    CCollision(const CCollision&);
    ~CCollision();
    CCollision& operator=(const CCollision&);
    void SwapOrder();

    unsigned char unknown00[12];
    float m_depth;
    unsigned char m_line : 1;
    unsigned char unknown10 : 7;
    unsigned char unknown11[15];
};

/* IVolume's virtuals in the order of __vt__7IVolume; the results are int */
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

class CCSGVolume : public IVolume {};
class CCDBObject : public IVolume {};

namespace dwi {
template <class T>
struct list_node {
    list_node() {}
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
    list() : m_count(0), m_capacity(0), m_pool(0), m_free(0) {
        list* self = this;
        m_head.m_prev = &self->m_head;
        m_head.m_next = &self->m_head;
        m_head.m_data = 0;
        m_owned = true;
    }
    template <class I>
    list(int capacity, I first, I last, char* name)
        : m_count(0), m_capacity(capacity),
          m_pool((list_node<T>*)DWI_allocalign(name, capacity * sizeof(list_node<T>), 16, 1024)), m_free(0),
          m_head(&m_head, &m_head, T()) {
        m_owned = true;
        list_node<T>* head;
        list_node<T>* next = 0;
        list_node<T>* node;
        for (node = m_pool + (m_capacity - 1); node != m_pool; node--) {
            node->m_next = next;
            next = node;
        }
        node->m_next = next;
        m_free = node;
        list_node<T>* tail = m_head.m_prev;
        head = (list_node<T>*)((char*)this + 16);
        for (; first != last; ++first) {
            const T& value = *first;
            list_node<T>* added = new (alloc_node()) list_node<T>(0, 0, value);
            tail->m_next = added;
            added->m_prev = tail;
            tail = added;
            m_count++;
        }
        tail->m_next = head;
        head->m_prev = tail;
    }
    list_node<T>* alloc_node() {
        list_node<T>* node = m_free;
        m_free = node->m_next;
        return node;
    }
    list_iter<T> begin() { return list_iter<T>(m_head.m_next); }
    list_iter<T> end() { return list_iter<T>(&m_head); }
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

    void push_back(const T& value) {
        list_node<T>* node = m_free;
        if (node) {
            m_free = node->m_next;
            node->m_prev = 0;
            node->m_next = 0;
            new (&node->m_data) T(value);
        }
        node->m_next = &m_head;
        node->m_prev = m_head.m_prev;
        m_head.m_prev->m_next = node;
        m_head.m_prev = node;
        m_count++;
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

    void erase(list_node<T>* node) {
        list_node<T>* prev = node->m_prev;
        list_node<T>* next = node->m_next;
        prev->m_next = next;
        next->m_prev = prev;
        node->m_prev = 0;
        node->m_next = m_free;
        m_free = node;
        m_count--;
    }
    void push_back(const T& data, list_node<T>* head) {
        list_node<T>* node = m_free;
        list_node<T>* last = head->m_prev;
        m_free = node->m_next;
        node = new (node) list_node<T>(0, 0, data);
        m_count++;
        last->m_next = node;
        node->m_prev = last;
        node->m_next = head;
        head->m_prev = node;
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

}

class CWorldVolume : public IVolume {
public:
    CWorldVolume(int);
    virtual ~CWorldVolume();
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
    virtual void GetExtents(CVector3&, CVector3&) const;
    virtual int TestCollision(const CCSGVolume&, CCollision&, bool) const;
    void RemoveSubVolume(dwi::list_iter<const CCDBObject*>);
    void RemoveSubVolume(const CCDBObject&);
    void AddSubVolume(const CCDBObject&);
    void RemoveAllSubVolumes();
    template <class T> int TestCollisionWithSubVolumes(const T&, CCollision&, bool) const;

    void UpdateExtents() {
        if (m_subVolumes.m_count != 0) {
            m_min.v[0] = 3.4028235e38f;
            m_min.v[1] = 3.4028235e38f;
            m_min.v[2] = 3.4028235e38f;
            m_max.v[0] = -m_min.v[0];
            m_max.v[1] = -m_min.v[1];
            m_max.v[2] = -m_min.v[2];
            dwi::list_node<const CCDBObject*>* end = &m_subVolumes.m_head;
            for (dwi::list_node<const CCDBObject*>* node = m_subVolumes.m_head.m_next; node != end;
                 node = node->m_next) {
                CVector3 min;
                CVector3 max;
                node->m_data->GetExtents(min, max);
                m_min.v[0] = m_min.v[0] < min.v[0] ? m_min.v[0] : min.v[0];
                m_min.v[1] = m_min.v[1] < min.v[1] ? m_min.v[1] : min.v[1];
                m_min.v[2] = m_min.v[2] < min.v[2] ? m_min.v[2] : min.v[2];
                m_max.v[0] = m_max.v[0] > max.v[0] ? m_max.v[0] : max.v[0];
                m_max.v[1] = m_max.v[1] > max.v[1] ? m_max.v[1] : max.v[1];
                m_max.v[2] = m_max.v[2] > max.v[2] ? m_max.v[2] : max.v[2];
            }
        } else {
            m_min.v[2] = 0.0f;
            m_min.v[1] = 0.0f;
            m_min.v[0] = 0.0f;
            m_max.v[2] = 0.0f;
            m_max.v[1] = 0.0f;
            m_max.v[0] = 0.0f;
        }
    }

    void UpdateExtents(dwi::list_node<const CCDBObject*>* end) {
        if (m_subVolumes.m_count != 0) {
            m_min.v[0] = 3.4028235e38f;
            m_min.v[1] = 3.4028235e38f;
            m_min.v[2] = 3.4028235e38f;
            m_max.v[0] = -m_min.v[0];
            m_max.v[1] = -m_min.v[1];
            m_max.v[2] = -m_min.v[2];
            for (dwi::list_node<const CCDBObject*>* node = m_subVolumes.m_head.m_next; node != end;
                 node = node->m_next) {
                CVector3 min;
                CVector3 max;
                node->m_data->GetExtents(min, max);
                m_min.v[0] = m_min.v[0] < min.v[0] ? m_min.v[0] : min.v[0];
                m_min.v[1] = m_min.v[1] < min.v[1] ? m_min.v[1] : min.v[1];
                m_min.v[2] = m_min.v[2] < min.v[2] ? m_min.v[2] : min.v[2];
                m_max.v[0] = m_max.v[0] > max.v[0] ? m_max.v[0] : max.v[0];
                m_max.v[1] = m_max.v[1] > max.v[1] ? m_max.v[1] : max.v[1];
                m_max.v[2] = m_max.v[2] > max.v[2] ? m_max.v[2] : max.v[2];
            }
        } else {
            m_min.v[2] = 0.0f;
            m_min.v[1] = 0.0f;
            m_min.v[0] = 0.0f;
            m_max.v[2] = 0.0f;
            m_max.v[1] = 0.0f;
            m_max.v[0] = 0.0f;
        }
    }

    dwi::list<const CCDBObject*> m_subVolumes;
    CVector3 m_min;
    CVector3 m_max;
    int data48;
};

#pragma auto_inline off
template <class T>
__declspec(weak) int CWorldVolume::TestCollisionWithSubVolumes(const T& other, CCollision& collision, bool flag) const {
    const dwi::list_node<const CCDBObject*>* node = m_subVolumes.m_head.m_next;
    const dwi::list_node<const CCDBObject*>* end = &m_subVolumes.m_head;
    int result = 1;
    for (; node != end; node = node->m_next) {
        const CCDBObject* volume = node->m_data;
        CCollision local(collision);
        int test = volume->TestCollision(other, local, flag);
        if (collision.m_line) {
            if (test != 1 && local.m_depth <= collision.m_depth) {
                collision = local;
                result = test;
            }
        } else {
            if (test >= result) {
                result = test;
                collision = local;
            }
            if (result == 3)
                break;
        }
    }
    return result;
}
#pragma auto_inline reset


CWorldVolume::CWorldVolume(int count) {
    data48 = 0;
    m_subVolumes.reserve(count, "World volume sub volumes");
}

void CWorldVolume::RemoveAllSubVolumes() {
    dwi::list<const CCDBObject*> empty;
    m_subVolumes.swap(empty);
}

CWorldVolume::~CWorldVolume() {
}

void CWorldVolume::AddSubVolume(const CCDBObject& object) {
    dwi::list_node<const CCDBObject*>* head = &m_subVolumes.m_head;
    m_subVolumes.push_back(&object, head);
    UpdateExtents(head);
}

void CWorldVolume::RemoveSubVolume(const CCDBObject& object) {
    dwi::list_node<const CCDBObject*>* node = m_subVolumes.m_head.m_next;
    dwi::list_node<const CCDBObject*>* end = &m_subVolumes.m_head;
    for (; node != end; node = node->m_next) {
        if (node->m_data == &object) {
            RemoveSubVolume(dwi::list_iter<const CCDBObject*>(node));
            return;
        }
    }
}

#pragma auto_inline off
void CWorldVolume::RemoveSubVolume(dwi::list_iter<const CCDBObject*> it) {
    m_subVolumes.erase(it.m_node);
    UpdateExtents();
}
#pragma auto_inline reset

int CWorldVolume::TestCollision(const IVolume& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    return other.TestCollision(*this, collision, flag);
}

int CWorldVolume::TestCollision(const CVolBox& other, CCollision& collision, bool flag) const {
    return TestCollisionWithSubVolumes(other, collision, flag);
}

int CWorldVolume::TestCollision(const CVolSphere& other, CCollision& collision, bool flag) const {
    return TestCollisionWithSubVolumes(other, collision, flag);
}

int CWorldVolume::TestCollision(const CVolCapsule& other, CCollision& collision, bool flag) const {
    return TestCollisionWithSubVolumes(other, collision, flag);
}

int CWorldVolume::TestCollision(const CLine3& other, CCollision& collision, bool flag) const {
    return TestCollisionWithSubVolumes(other, collision, flag);
}

int CWorldVolume::TestCollision(const CVolFiber& other, CCollision& collision, bool flag) const {
    return TestCollisionWithSubVolumes(other, collision, flag);
}

int CWorldVolume::TestCollision(const CAnimatedVolume& other, CCollision& collision, bool flag) const {
    return TestCollisionWithSubVolumes(other, collision, flag);
}

int CWorldVolume::TestCollision(const CCSGVolume& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    return other.TestCollision(*this, collision, flag);
}

void CWorldVolume::GetExtents(CVector3& min, CVector3& max) const {
    min = m_min;
    max = m_max;
}

__declspec(weak) int CWorldVolume::TestCollision(const CCDBObject&, CCollision&, bool) const {
    return false;
}

__declspec(weak) int CWorldVolume::TestCollision(const CWorldVolume&, CCollision&, bool) const {
    return false;
}

__declspec(weak) int CWorldVolume::TestCollision(CVector3, CCollision&, bool) const {
    return false;
}

__declspec(weak) int CWorldVolume::TestCollision(const CPlane&, CCollision&, bool) const {
    return false;
}

__declspec(weak) int CWorldVolume::TestCollision(const CTriangle&, CCollision&, bool) const {
    return false;
}

