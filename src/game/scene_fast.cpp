// A fragment of scene.cpp (0x800d85f0): CScene::DetectFastCollisions. It
// destroys the previous collisions, sweeps the extents and walks the two
// sorted node-pair lists (+216, +240) in step as DetectSlowCollisions does
// (scene_query.cpp), but tests only the pairs that involve a bullet (with
// a time limit of 1); a hit (status 2) earlier than the bullet's earliest
// hit so far (+148) becomes that bullet's only collision: one found for the
// same bullet (find_if with ClsnInvolvesThisBullet) is erased and the new
// one is appended. It returns whether any collision was kept. The OSGetTick
// calls are the remains of timing code. The classes, the predicate and the
// functions are named by the symbols; the members and views are as in
// scene_query.cpp (the bullet's hit time is inferred), and the vector's
// begin, end and erase and find_if are the instances emitted elsewhere.
// The predicate's copy constructor is inferred (it makes CW build the
// by-value argument in place). The collision iterator's destructor, which
// this compiles as a weak copy, is discarded as a duplicate of the one in
// scene_veciter_dtor.cpp. The rest of the file is not part of this unit.
extern "C" long OSGetTick();

class ISceneNode;

enum EClsnId {};
class CDrawContext;
class CMatrix;
class CVector3;
class CBullet;
class CPlayerObject;

extern "C" long OSGetTick();

class ISceneNode;


class CBullet {
public:
    unsigned char unknown000[148];
    float m_hitTime;
};

class CCollision {
public:
    CCollision();
    CCollision(const CCollision&);
    ~CCollision();
    int Test(float&);

    ISceneNode* m_node0;
    ISceneNode* m_node1;
    float m_time;
    float m_hitTime;
    unsigned char m_line : 1;
    unsigned char unknown10 : 1;
    unsigned char m_order : 2;
    unsigned char unknown10b : 4;
    unsigned char unknown11[15];
};

class CCDBObject {
public:
    static void DisablePassThru(bool);
};

namespace dwi {
template <class T>
class vec_iter {
public:
    vec_iter() : m_p(0) {}
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
    vec_iter<T> begin();
    vec_iter<T> end();
    vec_iter<T> erase(vec_iter<T>);
    void clear() {
        for (vec_iter<T> it = begin(); it != end(); ++it)
            it.m_p->~T();
        m_size = 0;
    }
    void push_back(const T& value) {
        new (end().m_p) T(value);
        m_size++;
    }

    int m_size;
    int m_capacity;
    T* m_data;
    bool m_owned;
};

template <class T>
class fast_vec {
public:
    T* m_data;
    int m_unknown4;
    int m_size;
    int m_capacity;
    bool m_owned;
    int m_growth;
};
}

class CCollision;
class ISceneNode {
public:
    enum EVolumeType {};
    virtual void MarkForDestruction(int);
    virtual ~ISceneNode();
    virtual void Destroy();
    virtual void BeginUpdate(float);
    virtual void UpdateAI(float);
    virtual void CommitAI();
    virtual void ConstrainVelocity();
    virtual void AttemptUpdate(float);
    virtual void OnCollision(const CCollision&);
    virtual void CommitUpdate();
    virtual void Draw(CDrawContext&);
    virtual int GetLocalBoundingVolume(EVolumeType) const;
    virtual int GetWorldBoundingVolume(EVolumeType) const;
    virtual void GetTMLocalToWorld(CMatrix&) const;
    virtual void GetPosition(CVector3&) const;
    virtual void GetRightward(CVector3&) const;
    virtual void GetForward(CVector3&) const;
    virtual void GetUpward(CVector3&) const;
    virtual int IsVisible(CDrawContext&) const;
    virtual int IsDrawEnabled() const;
    virtual EClsnId GetCollisionId() const;
    virtual void SetCollisionId(EClsnId);
    virtual int GetScriptObject() const;
    virtual void TriggerScriptEvent(int, void*, bool);
    virtual void HandleBulletCollision(CBullet*, const CCollision&);
    virtual void* AsMovingNode();
    virtual const void* AsMovingNode() const;
    virtual void* AsStaticObject();
    virtual const void* AsStaticObject() const;
    virtual void* AsHierObject();
    virtual const void* AsHierObject() const;
    virtual void* AsWorldObject();
    virtual const void* AsWorldObject() const;
    virtual void* AsAnimObject();
    virtual const void* AsAnimObject() const;
    virtual void* AsSoldierObject();
    virtual const void* AsSoldierObject() const;
    virtual CPlayerObject* AsPlayerObject();
    virtual const void* AsPlayerObject() const;
    virtual void* AsPlayerWeaponObject();
    virtual const void* AsPlayerWeaponObject() const;
    virtual void* AsAnimatedPlayerObject();
    virtual const void* AsAnimatedPlayerObject() const;
    virtual void* AsLight();
    virtual const void* AsLight() const;
    virtual CBullet* AsBullet();
    virtual const CBullet* AsBullet() const;
    virtual void* AsCollisionVolume();
    virtual const void* AsCollisionVolume() const;

    ISceneNode* m_link4;
    ISceneNode* m_link8;
};

/* the predicate of the find_if instance (named by its symbol) */
struct ClsnInvolvesThisBullet {
    ClsnInvolvesThisBullet(CBullet* bullet) : m_bullet(bullet) {}
    ClsnInvolvesThisBullet(const ClsnInvolvesThisBullet& other) : m_bullet(other.m_bullet) {}

    CBullet* m_bullet;
};

namespace dwi {
template <class I, class P> I find_if(I, I, P);
}

inline void* operator new(unsigned long, void* p) {
    return p;
}


class CScene {
public:
    union UNodePair {
        struct {
            ISceneNode* first;
            ISceneNode* second;
            signed char order;
        } nodes;
        unsigned long long key;
        unsigned char data[16];
    };

    /* inferred inline helper */
    static bool InvolvesBullet(const UNodePair& p) { return p.nodes.first->AsBullet() || p.nodes.second->AsBullet(); }
    bool IsNodeInScene(const ISceneNode*) const;
    CPlayerObject* GetPlayer(int) const;
    bool DetectFastCollisions(float);
    void SweepAllExtents();

    ISceneNode* m_firstNode;
    ISceneNode* m_node4;
    CPlayerObject* m_players[4];
    unsigned char unknown018[192];
    dwi::fast_vec<UNodePair> m_pairs216;
    dwi::fast_vec<UNodePair> m_pairs240;
    dwi::vector<CCollision> m_collisions;
};

bool CScene::DetectFastCollisions(float dt) {
    UNodePair* aEnd;
    UNodePair* bEnd;
    long ticks = 0;
    UNodePair* a;
    UNodePair* b;

    OSGetTick();
    m_collisions.clear();
    SweepAllExtents();
    OSGetTick();
    a = m_pairs216.m_data;
    aEnd = m_pairs216.m_data + m_pairs216.m_size;
    b = m_pairs240.m_data;
    bEnd = m_pairs240.m_data + m_pairs240.m_size;
    while (a != aEnd && b != bEnd) {
        if (a->key == b->key) {
            if (InvolvesBullet(*a)) {
                CCollision collision;
                collision.m_node0 = a->nodes.first;
                collision.m_node1 = a->nodes.second;
                collision.m_time = dt;
                collision.m_order = a->nodes.order > 1 ? a->nodes.order - 1 : 0;
                ticks -= OSGetTick();
                float limit = 1.0f;
                int status = collision.Test(limit);
                ticks += OSGetTick();
                switch (status) {
                case 1:
                    break;
                case 2: {
                    CBullet* bullet = collision.m_node0->AsBullet();
                    if (!bullet)
                        bullet = collision.m_node1->AsBullet();
                    if (collision.m_hitTime < bullet->m_hitTime) {
                        bullet->m_hitTime = collision.m_hitTime;
                        dwi::vec_iter<CCollision> it;
                        it = dwi::find_if(m_collisions.begin(), m_collisions.end(), ClsnInvolvesThisBullet(bullet));
                        if (it != m_collisions.end())
                            m_collisions.erase(it);
                        m_collisions.push_back(collision);
                    }
                    break;
                }
                case 3:
                    break;
                }
            }
            a++;
            b++;
        } else {
            if (a->key < b->key)
                a++;
            if (b->key < a->key)
                b++;
        }
    }
    OSGetTick();
    OSGetTick();
    return m_collisions.m_size != 0;
}
