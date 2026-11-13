// The dwi container templates (list, vector, fast_vec) and min/max, shared by every file that
// uses them (PortReadiness step 3.3). The hash map is in game/DwiHashMap.h.
//
// Evidence (nm of the original; weak copies are the header's inline bodies):
// - Names: dwi::list<T>, list_node<T>, list_iter<T>, vector<T>, vec_iter<T>, vec_const_iter<T>,
//   fast_vec<T> (destructors, reserve, begin/end/size, push_back and the list range
//   constructor in the mangled names).
// - list<T>::reserve(int, char*) and vector<T>::reserve(int, char*) (the name is the
//   allocation tag), the list range constructor list(int, list_iter, list_iter, char*),
//   fast_vec<T>::reserve(T*, int) (adopts caller storage; 0x800770a4), fast_vec(int) (0x80076f68),
//   fast_vec<T>::push_back(T) by value (0x80070c5c), vector<T>::erase(vec_iter<T>),
//   vec_iter<T>::operator+(int) const, and a destructor for list_node and vec_iter (weak).
// - Sizes: list 0x2c for a 16-byte element (count, capacity, pool, free list, head node, owned),
//   fast_vec 0x18, vector 0x10 (CScene's members).
//
// Inferred, not in any mangled name: the member names (the matched views'), the bodies of the
// inline members no weak copy shows, fast_vec's (T*, int, int) constructor and its default
// growth, list::alloc_node/push_back/push_front/erase/clear/swap, and min/max's bodies
// (reference-returning, as AISplinePath_segment.cpp's dwi::min).
#ifndef GAME_DWICONTAINERS_H
#define GAME_DWICONTAINERS_H

extern "C" void MEM_free(void*);
void* DWI_allocalign(const char*, int, int, int);

inline void* operator new(unsigned long, void* p) { return p; }

namespace dwi {

template <class T>
inline const T& min(const T& a, const T& b) {
    return a < b ? a : b;
}

template <class T>
inline const T& max(const T& a, const T& b) {
    return a < b ? b : a;
}

template <class T>
struct list_node {
    list_node() {}
    list_node(const T& value) : m_prev(0), m_next(0), m_data(value) {}
    list_node(list_node* prev, list_node* next, const T& data) : m_prev(prev), m_next(next), m_data(data) {}

    list_node* m_prev;
    list_node* m_next;
    T m_data;
};

template <class T>
class list_iter {
public:
    list_iter() {}
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
    typedef list_iter<T> iterator;

    list() : m_count(0), m_capacity(0), m_pool(0), m_free(0), m_head(&m_head, &m_head, T()), m_owned(true) {}
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

    iterator begin() { return iterator(m_head.m_next); }
    iterator end() { return iterator(&m_head); }

    list_node<T>* alloc_node() {
        list_node<T>* node = m_free;
        m_free = node->m_next;
        return node;
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
    void push_front(const T& value) {
        list_node<T>* pos = m_head.m_next;
        list_node<T>* prev = pos->m_prev;
        list_node<T>* node = alloc_node();
        new (node) list_node<T>(0, 0, value);
        m_count++;
        prev->m_next = node;
        node->m_prev = prev;
        node->m_next = pos;
        pos->m_prev = node;
    }
    list_node<T>* erase(list_node<T>* node) {
        list_node<T>* prev = node->m_prev;
        list_node<T>* next = node->m_next;
        prev->m_next = next;
        next->m_prev = prev;
        node->m_prev = 0;
        node->m_next = m_free;
        m_free = node;
        m_count--;
        return next;
    }
    void clear() {
        list_node<T>* node = m_head.m_next;
        while (node != &m_head) {
            list_node<T>* next = node->m_next;
            node->m_prev = 0;
            node->m_next = m_free;
            m_free = node;
            node = next;
        }
        m_head.m_next = &m_head;
        m_head.m_prev = &m_head;
        m_count = 0;
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

template <class T>
class vec_iter {
public:
    vec_iter() : m_p(0) {}
    vec_iter(T* p) : m_p(p) {}
    vec_iter(const vec_iter& other) : m_p(other.m_p) {}
    ~vec_iter() { m_p = 0; }
    bool operator!=(const vec_iter& other) const { return m_p != other.m_p; }
    vec_iter& operator++() {
        m_p++;
        return *this;
    }
    vec_iter operator+(int n) const { return vec_iter(m_p + n); }
    T* operator->() const { return m_p; }
    T& operator*() const { return *m_p; }

    T* m_p;
};

template <class T>
class vec_const_iter {
public:
    vec_const_iter(const T* p) : m_p(p) {}
    vec_const_iter(const vec_iter<T>& it) : m_p(it.m_p) {}
    ~vec_const_iter() { m_p = 0; }
    bool operator!=(const vec_iter<T>& other) const { return m_p != other.m_p; }
    vec_const_iter& operator++() {
        m_p++;
        return *this;
    }
    const T* operator->() const { return m_p; }
    const T& operator*() const { return *m_p; }

    const T* m_p;
};

template <class T>
class vector {
public:
    vector() : m_size(0), m_capacity(0), m_data(0), m_owned(true) {}
    ~vector() {
        clear();
        if (m_owned)
            MEM_free(m_data);
    }

    vec_iter<T> begin() { return vec_iter<T>(m_data); }
    vec_iter<T> end() { return vec_iter<T>(m_data + m_size); }
    int size() const { return m_size; }
    T& operator[](int i) { return m_data[i]; }
    void clear() {
        for (vec_iter<T> it = begin(); it != end(); ++it)
            it.m_p->~T();
        m_size = 0;
    }
    void push_back(const T& value) {
        new (end().m_p) T(value);
        m_size++;
    }
    void reserve(int, char*);
    vec_iter<T> erase(vec_iter<T>);

    int m_size;
    int m_capacity;
    T* m_data;
    bool m_owned;
};

template <class T>
class fast_vec {
public:
    fast_vec(int growth = -1) : m_data(0), m_unknown4(0), m_size(0), m_capacity(0), m_owned(true), m_growth(growth) {}
    fast_vec(T* storage, int capacity, int growth = 12)
        : m_data(storage), m_unknown4(0), m_size(0), m_capacity(capacity), m_owned(false), m_growth(growth) {}
    ~fast_vec() {
        if (m_owned && m_data)
            MEM_free(m_data);
    }

    void reserve(T* storage, int capacity) {
        if (m_owned && m_data)
            MEM_free(m_data);
        m_data = storage;
        m_capacity = capacity;
        m_owned = false;
    }
    void reserve(int count, char* name) {
        if (m_owned && m_data)
            MEM_free(m_data);
        m_data = (T*)DWI_allocalign(name, count * sizeof(T), 16, 1024);
        m_capacity = count;
        m_owned = true;
    }
    T* begin() const { return m_data; }
    T* end() const { return m_data + m_size; }
    int size() const { return m_size; }
    int capacity() const { return m_capacity; }
    T& back() { return m_data[m_size - 1]; }
    void push_back(T value) { m_data[m_size++] = value; }
    void pop_back() { m_size--; }
    void clear() { m_size = 0; }
    T* find(const T& value) {
        T* it = m_data;
        T* last = m_data + m_size;
        for (; it != last; it++) {
            if (*it == value)
                break;
        }
        return it;
    }
    void erase(T* it) {
        T* last = m_data + m_size;
        for (T* src = it + 1; src != last;)
            *it++ = *src++;
        m_size--;
    }

    T* m_data;
    int m_unknown4;
    int m_size;
    int m_capacity;
    bool m_owned;
    int m_growth;
};

} // namespace dwi

#endif
