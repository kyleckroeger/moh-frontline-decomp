// A fragment of buddy.cpp (0x8005872c): the weak dwi::list<BuddyInfo>::reserve,
// the list destructor and the file-local range constructor it uses, in the
// original's order (reserve, then the weak copies CW emits after their first
// user). reserve grows the node pool to the requested capacity: it builds a
// new list of that capacity from the current elements through the range
// constructor (pool allocation, free-list threading, the sentinel built from a
// BuddyInfo() temporary, the elements linked in at the end), swaps it in
// (counts, capacities, pools, free lists and ownership swapped, the sentinel
// links moved for each non-empty side) and lets the old storage go with the
// temporary's destructor (every node back to the free list through clear(),
// the pool freed when owned). The dwi templates and BuddyInfo are named by
// the mangled symbols; the list layout, the inline members (swap, clear,
// init_free, link_range, iterators, with a user-declared iterator copy
// constructor so the iterator arguments are built in place) and BuddyInfo's
// members are inferred, as in world_object_dtor.cpp and buddy_head.cpp. reserve
// is defined __declspec(weak) and instantiated explicitly. The rest of the
// file is not part of this unit.
extern "C" void MEM_free(void*);
void* DWI_allocalign(const char*, int, int, int);
class CBuddySoldier;
inline void* operator new(unsigned long, void* p) { return p; }

/* inferred: the state part of an attached buddy (path base, moving flag, in use). Its assignment
   copies the fields in layout order, as the original's element assignments do. */
struct BuddyState {
    int path;
    int moving;
    bool active;
    BuddyState& operator=(const BuddyState& o) {
        path = o.path;
        moving = o.moving;
        active = o.active;
        return *this;
    }
};

/* inferred: an empty check of a buddy state, taken by value (it reads like a compiled-out debug
   check). Called first in BuddyInfo's constructor: the whole-state read makes CW 1.3.2 keep only
   the state part of a BuddyInfo temporary or local in its stack slot and split soldier/progress
   into registers, as the original's list range constructor, push_back(BuddyInfo()) and
   BIFunc_AddBuddy show. */
inline void Check(BuddyState) {}

/* inferred: an attached buddy (soldier, checkpoint progress, state) */
struct BuddyInfo {
    BuddyInfo() : soldier(0), progress(0) {
        Check(state);
        state.moving = 0;
        state.active = false;
        state.path = 0;
    }
    BuddyInfo(const BuddyInfo& other) : soldier(other.soldier), progress(other.progress) {
        state.moving = other.state.moving;
        state.active = other.state.active;
        state.path = other.state.path;
    }

    CBuddySoldier* soldier;
    int progress;
    BuddyState state;
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
          m_pool((list_node<T>*)DWI_allocalign(name, m_capacity * sizeof(list_node<T>), 16, 1024)), m_free(0),
          m_head(&m_head, &m_head, T()), m_owned(true) {
        init_free();
        link_range(end(), first, last);
    }
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
    list_iter<T> end() { return list_iter<T>(&m_head); }
    void clear() {
        list_node<T>* node = m_head.m_next;
        list_node<T>* next;
        while (node != &m_head) {
            next = node->m_next;
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

template class dwi::list<BuddyInfo>;
