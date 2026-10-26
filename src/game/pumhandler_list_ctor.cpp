// A fragment of pumhandler.cpp (0x800b8f00): the file-local range
// constructor of dwi::list<QueuedSubtitle> (a member-template instance for
// list iterators, emitted for reserve): it allocates the node pool, sets up
// the sentinel from a default QueuedSubtitle (only its colour is set; the
// times and text are copied uninitialised), chains the free nodes from the
// last one down, then copies each element of the range into a popped node
// and links it at the tail, finally closing the ring at the sentinel.
// reserve is compiled here only to emit the constructor; its own copy is
// placed by pumhandler_list_reserve (a file duplicate). The dwi templates
// and QueuedSubtitle are named by the mangled symbols; the list layout, the
// inline members and the QueuedSubtitle fields are inferred, as in
// alarm_list_ctor.cpp. The members are set in the constructor's
// initialiser list, the free-list set-up sits in an inline helper (its
// 'next' shares the zero register with the cleared members) and the copy
// loop in a second one that takes the sentinel as an end() iterator. The
// rest of the file is not part of this unit.
extern "C" void MEM_free(void*);
void* DWI_allocalign(const char*, int, int, int);
inline void* operator new(unsigned long, void* p) { return p; }

struct CColor {
    CColor() {}
    CColor(unsigned char ar, unsigned char ag, unsigned char ab, unsigned char aa) : r(ar), g(ag), b(ab), a(aa) {}
    CColor(const CColor& c) : r(c.r), g(c.g), b(c.b), a(c.a) {}
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};
struct QueuedSubtitle {
    QueuedSubtitle() : m_color(0, 0, 0, 128) {}
    float m_delay;
    float m_time;
    CColor m_color;
    char m_text[48];
};

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
    template <class I>
    list(int, I, I, char*);
    template <class I2> void link_range(I2 end, I2 first, I2 last) {
        list_node<T>* head = end.m_node;
        list_node<T>* stop = last.m_node;
        list_node<T>* tail = m_head.m_prev;
        for (I2 it = first; it.m_node != stop; ++it) {
            list_node<T>* added = new (alloc_node()) list_node<T>(0, 0, *it);
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

template <class T> template <class I> list<T>::list(int capacity, I first, I last, char* name)
    : m_count(0), m_capacity(capacity), m_pool((list_node<T>*)DWI_allocalign(name, m_capacity * sizeof(list_node<T>), 16, 1024)),
      m_free(0), m_head(&m_head, &m_head, T()), m_owned(true) {
    init_free();
    link_range(end(), first, last);
}

template <class T> __declspec(weak) void list<T>::reserve(int capacity, char* name) {
    if (capacity > m_capacity) {
        list<T> grown(capacity, begin(), end(), name);
        swap(grown);
    }
}
}

template class dwi::list<QueuedSubtitle>;
