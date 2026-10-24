// A fragment of world_volume.cpp (0x800cbf1c): CWorldVolume::RemoveSubVolume
// (const CCDBObject&) walks the sub-volume list and removes the first node
// holding the object through the iterator overload (defined in
// world_volume_edit.cpp; kept in its own unit because the original calls it
// rather than inlining it); CWorldVolume::AddSubVolume takes a node from the
// free list (placement-constructed, so null-checked), links it before the
// list head, counts it and recomputes the bounds like the iterator overload
// does (the head address is shared by the link and the bounds loop).
// CWorldVolume, CCDBObject, IVolume and the dwi list templates are named by
// the mangled symbols; the list view is world_volume_list.cpp's; the
// iterator's user copy constructor lets the by-value argument be built in
// place, as in the original. The constants are items of the file's .sdata2
// pool.
class CCDBObject;
class CCollision;
class CMatrix;
class CVolBox;
class CVolSphere;
class CVolCapsule;
class CVolFiber;
class CAnimatedVolume;
class CWorldVolume;
class CLine3;
class CPlane;
class CTriangle;
class CDrawContext;

class CVector3 {
public:
    float x, y, z, w;
} __attribute__((aligned(8)));

/* IVolume's virtuals in table order up to GetExtents. */
class IVolume {
public:
    virtual ~IVolume();
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

class CCDBObject : public IVolume {};

inline void* operator new(unsigned long, void* p) { return p; }

namespace dwi {
template <class T>
struct list_node {
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
    list_node<T>* m_node;
};

template <class T>
class list {
public:
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
    void RemoveSubVolume(dwi::list_iter<const CCDBObject*>);
    void RemoveSubVolume(const CCDBObject&);
    void AddSubVolume(const CCDBObject&);

    void UpdateExtents(dwi::list_node<const CCDBObject*>* end) {
        if (m_subVolumes.m_count != 0) {
            m_min.x = 3.4028235e38f;
            m_min.y = 3.4028235e38f;
            m_min.z = 3.4028235e38f;
            m_max.x = -m_min.x;
            m_max.y = -m_min.y;
            m_max.z = -m_min.z;
            for (dwi::list_node<const CCDBObject*>* node = m_subVolumes.m_head.m_next; node != end;
                 node = node->m_next) {
                CVector3 min;
                CVector3 max;
                node->m_data->GetExtents(min, max);
                m_min.x = m_min.x < min.x ? m_min.x : min.x;
                m_min.y = m_min.y < min.y ? m_min.y : min.y;
                m_min.z = m_min.z < min.z ? m_min.z : min.z;
                m_max.x = m_max.x > max.x ? m_max.x : max.x;
                m_max.y = m_max.y > max.y ? m_max.y : max.y;
                m_max.z = m_max.z > max.z ? m_max.z : max.z;
            }
        } else {
            m_min.z = 0.0f;
            m_min.y = 0.0f;
            m_min.x = 0.0f;
            m_max.z = 0.0f;
            m_max.y = 0.0f;
            m_max.x = 0.0f;
        }
    }

    dwi::list<const CCDBObject*> m_subVolumes;
    CVector3 m_min;
    CVector3 m_max;
};

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

void CWorldVolume::AddSubVolume(const CCDBObject& object) {
    dwi::list_node<const CCDBObject*>* head = &m_subVolumes.m_head;
    m_subVolumes.push_back(&object, head);
    UpdateExtents(head);
}
