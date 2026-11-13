// A fragment of scene.cpp (0x800dc44c): CScene::Reset clears the scene's node
// and player pointers and other members, empties the two extent lists (nodes
// back to each list's free list), the four fast vectors and the collision
// vector (destroying each collision). CScene, CCollision, ISceneNode::SExtent
// and the dwi containers are named by the mangled symbols; the scene's
// members (offsets from the stores), the container layouts (as in the weak
// container fragments) and their inline clear functions (names inferred) are
// inferred, and the fast vectors' element types are not known (void* here).
// The collision iterator's destructor, which this compiles as a weak copy, is
// discarded as a duplicate of the one in scene_veciter_dtor.cpp. The rest of
// the file is not part of this unit.
extern "C" void MEM_free(void*);

class CCollision {
public:
    ~CCollision();

    unsigned char data[32];
};

class ISceneNode {
public:
    struct SExtent {
        unsigned char data[16];
    };
};

class CPlayerObject;

#include "game/DwiContainers.h"

class CScene {
public:
    void Reset();

    void* m_node0;
    void* m_node4;
    CPlayerObject* m_players[4];
    void* m_value24[12];
    unsigned char unknown072[4];
    void* m_value76;
    unsigned char unknown080[8];
    void* m_value88;
    unsigned char unknown092[20];
    void* m_value112;
    unsigned char unknown116[12];
    dwi::list<ISceneNode::SExtent> m_extents128;
    dwi::list<ISceneNode::SExtent> m_extents172;
    dwi::fast_vec<void*> m_vec216;
    dwi::fast_vec<void*> m_vec240;
    dwi::vector<CCollision> m_collisions;
    dwi::fast_vec<void*> m_vec280;
    dwi::fast_vec<void*> m_vec304;
};

void CScene::Reset() {
    int i;

    m_value76 = 0;
    m_players[0] = 0;
    m_players[1] = 0;
    m_players[2] = 0;
    m_players[3] = 0;
    m_node0 = 0;
    m_node4 = 0;
    for (i = 0; i < 12; i++)
        m_value24[i] = 0;
    m_value88 = 0;
    m_value112 = 0;
    m_extents128.clear();
    m_extents172.clear();
    m_vec216.clear();
    m_vec240.clear();
    m_collisions.clear();
    m_vec280.clear();
    m_vec304.clear();
}
