// A fragment of buddy.cpp (0x80058c68): the file-local instance of dwi::copy for
// a vector of BuddyInfos (each element assigned member by member). The dwi
// containers and BuddyInfo are named by the mangled symbols; the iterator's
// pointer and the element view (four words and a byte, from the copy) are inferred. The
// template is file-local (static) in the original, as its local symbol shows,
// so it is static here and instantiated explicitly. The rest of the file is
// not part of this unit.
struct BuddyInfo {
    int word00;
    int word04;
    int word08;
    int word0c;
    unsigned char byte10;
};

namespace dwi {
template <class T>
class vec_iter {
public:
    vec_iter(T* p) : m_p(p) {}
    bool operator!=(const vec_iter& other) const { return m_p != other.m_p; }
    vec_iter& operator++() {
        m_p++;
        return *this;
    }
    T& operator*() const { return *m_p; }

    T* m_p;
};

template <class I, class O>
static O copy(I first, I last, O result) {
    while (first != last) {
        *result = *first;
        ++result;
        ++first;
    }
    return result;
}
}

template dwi::vec_iter<BuddyInfo> dwi::copy(dwi::vec_iter<BuddyInfo>, dwi::vec_iter<BuddyInfo>, dwi::vec_iter<BuddyInfo>);
