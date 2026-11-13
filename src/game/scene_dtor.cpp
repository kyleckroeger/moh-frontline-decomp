// A fragment of scene.cpp (0x800dc620): the CScene destructor shuts the
// scene down while it still has nodes (the count at +56), then destroys its
// containers: the fast vectors (freeing owned storage), the collision vector
// (destroying each collision, then freeing) and the two extent lists (nodes
// back to the free list, owned node storage freed). Then the CScene
// constructor (0x800dc8c4): every member cleared, the containers constructed
// inline (fast vector growth values 2, 0xDEADBEEF, 5, 6, 9, 10; the extent
// lists' sentinels point at themselves around an uninitialised extent copy)
// and reserved under their names (the fast vectors inline, the lists and the
// collision vector through their weak reserve functions). The containers'
// inline destructors matter to the constructor: with exceptions on, each
// constructed member gets a cleanup record, and those records are what keep
// the constants 0/1/2 in callee-saved registers as in the original. CScene,
// CCollision, ISceneNode::SExtent, the fast vectors' element types and the
// dwi containers are named by the mangled symbols; the scene's members and
// the container layouts are inferred (as in scene_reset.cpp). The unit
// carries the dwi header's prime list, which precedes the names in .rodata.
// The weak destructor copies this compiles are duplicates: the collision
// iterator's (scene_veciter_dtor.cpp), CLight*'s (another file) and the
// scene.cpp copies placed by the scene_fastvec_*, scene_vec_dtor and
// scene_list_extent units. The rest of the file is not part of this unit.
extern "C" void MEM_free(void*);
void* DWI_allocalign(const char*, int, int, int);

class CCollision {
public:
    ~CCollision();

    unsigned char data[32];
};

class ISceneNode {
public:
    struct SExtent {
        SExtent() {}

        unsigned char data[16];
    };
};

class CPlayerObject;
class CLight;
class CCharacterShadow;

// Header-defined hash-table primes; every unit that includes the header carries a copy.
#include "game/DwiHashMap.h"
#include "game/DwiContainers.h"

class CScene {
public:
    struct UNodePair {
        unsigned char data[16];
    };
    struct SExclusionEntry {
        unsigned char data[12];
    };
    struct SModCommand {
        unsigned char data[16];
    };

    CScene();
    ~CScene();
    void Shutdown();

    void* m_node0;
    void* m_node4;
    CPlayerObject* m_players[4];
    void* m_value24;
    void* m_value28;
    void* m_value32;
    void* m_value36;
    void* m_value40;
    void* m_value44;
    void* m_value48;
    void* m_value52;
    int m_count56;
    void* m_value60;
    void* m_value64;
    void* m_value68;
    bool m_updating;
    void* m_value76;
    dwi::fast_vec<CLight*> m_lights;
    dwi::fast_vec<CCharacterShadow*> m_shadows;
    dwi::list<ISceneNode::SExtent> m_extentsX;
    dwi::list<ISceneNode::SExtent> m_extentsY;
    dwi::fast_vec<UNodePair> m_pairsX;
    dwi::fast_vec<UNodePair> m_pairsY;
    dwi::vector<CCollision> m_collisions;
    dwi::fast_vec<SExclusionEntry> m_exclusions;
    dwi::fast_vec<SModCommand> m_commands;
};

CScene::CScene()
    : m_node0(0), m_value24(0), m_value28(0), m_value32(0), m_value36(0), m_value40(0), m_value44(0), m_value48(0),
      m_value52(0), m_count56(0), m_value60(0), m_value64(0), m_value68(0), m_updating(false), m_value76(0),
      m_lights(2), m_shadows(0xDEADBEEF), m_pairsX(5), m_pairsY(6), m_exclusions(9), m_commands(10) {
    m_lights.reserve(20, "Scene lights");
    m_shadows.reserve(20, "Scene shadows");
    m_extentsX.reserve(500, "Scene extents X");
    m_extentsY.reserve(500, "Scene extents Y");
    m_pairsX.reserve(2048, "Scene node pairs X");
    m_pairsY.reserve(2048, "Scene node pairs Y");
    m_collisions.reserve(2048, "Scene collisions");
    m_exclusions.reserve(2048, "Scene exlusions");
    m_commands.reserve(40, "Scene mod commands");
}

CScene::~CScene() {
    if (m_count56 > 0)
        Shutdown();
}
