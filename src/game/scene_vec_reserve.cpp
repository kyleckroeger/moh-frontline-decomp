// A fragment of scene.cpp (0x800dcc48): the weak dwi::vector<CCollision>::
// reserve, which grows the storage to the requested capacity: it allocates
// the new block (16-byte aligned), copies the collisions across with the
// file's dwi::copy, clears the old vector (destroying each collision), frees
// its storage when owned and takes the new block. The containers and
// CCollision are named by the mangled symbols; the members, the inline
// iterator operators and the inline clear are inferred (as in
// buddy_vec_info_reserve.cpp) and CCollision is a 32-byte view. reserve is a weak template copy
// emitted in this file, defined __declspec(weak) and instantiated explicitly;
// copy is file-local in the original and only declared here. The rest of the
// file is not part of this unit.
extern "C" void MEM_free(void*);
void* DWI_allocalign(const char*, int, int, int);

class CCollision {
public:
    ~CCollision();

    unsigned char data[32];
};

namespace dwi {
template <class T>
class vec_iter {
public:
    vec_iter(T* p) : m_p(p) {}
    ~vec_iter() { m_p = 0; }
    bool operator!=(const vec_iter& other) const { return m_p != other.m_p; }
    vec_iter& operator++() {
        m_p++;
        return *this;
    }

    T* m_p;
};

template <class T>
class vector {
public:
    void reserve(int, char*);
    void clear() {
        for (vec_iter<T> it = begin(); it != end(); ++it)
            it.m_p->~T();
        m_size = 0;
    }
    vec_iter<T> begin();
    vec_iter<T> end();

    int m_size;
    int m_capacity;
    T* m_data;
    bool m_owned;
};

// File-local in the original (scene_copy.cpp); declared here only.
template <class I, class O>
O copy(I first, I last, O result);

template <class T> __declspec(weak) void vector<T>::reserve(int capacity, char* name) {
    if (capacity > m_capacity) {
        T* data = (T*)DWI_allocalign(name, capacity * sizeof(T), 16, 1024);
        copy(begin(), end(), vec_iter<T>(data));
        clear();
        if (m_owned)
            MEM_free(m_data);
        m_data = data;
        m_capacity = capacity;
        m_owned = true;
    }
}
}

template class dwi::vector<CCollision>;
