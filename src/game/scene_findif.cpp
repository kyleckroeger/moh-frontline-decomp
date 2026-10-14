// A fragment of scene.cpp (0x800d8b38): the file-local instance of
// dwi::find_if over a vector of collisions with the ClsnInvolvesThisBullet
// predicate (a collision involves the bullet when either of its two nodes is
// the bullet). The dwi containers, CCollision and the predicate are named by
// the mangled symbols; the iterator's pointer, the collision view and the
// predicate's member are inferred, and the template is file-local (static)
// in the original, as its local symbol shows, so it is static here and
// instantiated explicitly. The rest of the file is not part of this unit.
class ISceneNode;

class CCollision {
public:
    CCollision& operator=(const CCollision&);

    ISceneNode* m_node0;
    ISceneNode* m_node1;
    unsigned char data08[24];
};

/* inferred: the predicate matching collisions that involve a bullet */
struct ClsnInvolvesThisBullet {
    bool operator()(const CCollision& c) const { return m_bullet == c.m_node0 || m_bullet == c.m_node1; }

    ISceneNode* m_bullet;
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
    T& operator*() const { return *m_p; }

    T* m_p;
};

template <class T>
class vector {
public:
    vec_iter<T> end();
    vec_iter<T> begin();

    int m_size;
    int m_capacity;
    T* m_data;
};


template <class I, class P>
static I find_if(I first, I last, P pred) {
    I it = first;
    for (; it != last; ++it) {
        if (pred(*it))
            break;
    }
    return it;
}

}

template dwi::vec_iter<CCollision> dwi::find_if(dwi::vec_iter<CCollision>, dwi::vec_iter<CCollision>,
                                                ClsnInvolvesThisBullet);
