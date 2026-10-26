// A fragment of scene.cpp (0x800dcdc8): the weak
// dwi::list<ISceneNode::SExtent>::reserve, which grows the node pool to the
// requested capacity: it builds a new list of that capacity from the current
// elements (the range constructor, inline here: allocate the pool, chain the
// free nodes, then copy each extent into a popped node and link it at the
// tail), swaps it in and lets the old storage go with the temporary's inline
// destructor. The dwi templates are named by the mangled symbols; the list
// layout, the inline members and the 16-byte element view are inferred (as
// in world_volume_list_reserve.cpp and scene_list_extent.cpp). The sentinel
// address for the final link is taken afresh after the copy loop's set-up
// (the original recomputes it there rather than reusing the constructor's
// value): end() is written with a byte offset from the list. reserve is a
// weak template copy emitted in this file, defined __declspec(weak) and
// instantiated explicitly; the destructor's own copy is placed by
// scene_list_extent. The rest of the file is not part of this unit.
extern "C" void MEM_free(void*);
void* DWI_allocalign(const char*, int, int, int);
inline void* operator new(unsigned long, void* p) { return p; }

class ISceneNode {
public:
    struct SExtent {
        SExtent() {}

        int data[4];
    };
};

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
        head = end().m_node;
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
    list_iter<T> end() { return list_iter<T>((list_node<T>*)((char*)this + 16)); }
    ~list() {
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

    void reserve(int, char*);

    int m_count;
    int m_capacity;
    list_node<T>* m_pool;
    list_node<T>* m_free;
    list_node<T> m_head;
    bool m_owned;
};

template <class T> __declspec(weak) void list<T>::reserve(int capacity, char* name) {
    if (capacity > m_capacity) {
        list<T> grown(capacity, begin(), end(), name);
        swap(grown);
    }
}
}

template class dwi::list<ISceneNode::SExtent>;
