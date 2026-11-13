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

#include "game/DwiContainers.h"

class CCollision;
#include "game/ISceneNode.h"

/* the predicate of the find_if instance (named by its symbol) */
struct ClsnInvolvesThisBullet {
    ClsnInvolvesThisBullet(CBullet* bullet) : m_bullet(bullet) {}
    ClsnInvolvesThisBullet(const ClsnInvolvesThisBullet& other) : m_bullet(other.m_bullet) {}

    CBullet* m_bullet;
};

namespace dwi {
template <class I, class P> I find_if(I, I, P);
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
