// A fragment of scene.cpp (0x800d89bc): the weak dwi::vector<CCollision>::erase,
// which shifts the collisions after the erased one down by one (the file's
// dwi::copy from it + 1 to the end onto it), destroys the now-unused last
// collision, decrements the size and returns the iterator. The containers and
// CCollision are named by the mangled symbols; the members are inferred (as in
// scene_vec_reserve.cpp) and CCollision is a 32-byte view. erase is a weak
// template copy emitted in this file, defined __declspec(weak) and
// instantiated explicitly; copy is file-local in the original
// (scene_copy.cpp) and, like the out-of-line end and operator+, only declared
// here. The rest of the file is not part of this unit.
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
    vec_iter operator+(int) const;
    T* operator->() const { return m_p; }

    T* m_p;
};

template <class T>
class vector {
public:
    vec_iter<T> erase(vec_iter<T>);
    vec_iter<T> end();

    int m_size;
    int m_capacity;
    T* m_data;
    bool m_owned;
};

template <class I, class O>
O copy(I first, I last, O result);

template <class T> __declspec(weak) vec_iter<T> vector<T>::erase(vec_iter<T> it) {
    it = copy(it + 1, end(), it);
    it->~T();
    m_size--;
    return it;
}
}

template class dwi::vector<CCollision>;
