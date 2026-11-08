extern "C" void MEM_free(void*);
void* DWI_allocalign(const char*, int, int, int);

class CCollision {
public:
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

namespace dwi {
static const unsigned long dwi_prime_list[27] = {
    53,        97,        193,       389,       769,       1543,      3079,      6151,      12289,
    24593,     49157,     98317,     196613,    393241,    786433,    1572869,   3145739,   6291469,
    12582917,  25165843,  50331653,  100663319, 201326611, 402653189, 805306457, 1610612741, 3221225473u,
};

template <class T>
class vector {
public:
    vector() : m_size(0), m_capacity(0), m_data(0), m_owned(true) {}
    void reserve(int, char*);

    int m_size;
    int m_capacity;
    T* m_data;
    bool m_owned;
};

template <class T>
struct list_node {
    list_node(const T& data) : m_prev(this), m_next(this), m_data(data) {}

    list_node* m_prev;
    list_node* m_next;
    T m_data;
};

template <class T>
class list {
public:
    list() : m_count(0), m_unknown4(0), m_pool(0), m_free(0), m_head(T()), m_owned(true) {}
    void reserve(int, char*);

    int m_count;
    int m_unknown4;
    list_node<T>* m_pool;
    list_node<T>* m_free;
    list_node<T> m_head;
    bool m_owned;
};

template <class T>
class fast_vec {
public:
    fast_vec(int growth) : m_data(0), m_unknown4(0), m_size(0), m_capacity(0), m_owned(true), m_growth(growth) {}
    void reserve(int count, char* name) {
        if (m_owned && m_data)
            MEM_free(m_data);
        m_data = (T*)DWI_allocalign(name, count * sizeof(T), 16, 1024);
        m_capacity = count;
        m_owned = true;
    }

    T* m_data;
    int m_unknown4;
    int m_size;
    int m_capacity;
    bool m_owned;
    int m_growth;
};
}

struct SPair {
    unsigned char data[16];
};

struct SExclusion {
    unsigned char data[12];
};

struct SModCommand {
    unsigned char data[16];
};

class CScene {
public:
    CScene();

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
    dwi::fast_vec<void*> m_lights;
    dwi::fast_vec<void*> m_shadows;
    dwi::list<ISceneNode::SExtent> m_extentsX;
    dwi::list<ISceneNode::SExtent> m_extentsY;
    dwi::fast_vec<SPair> m_pairsX;
    dwi::fast_vec<SPair> m_pairsY;
    dwi::vector<CCollision> m_collisions;
    dwi::fast_vec<SExclusion> m_exclusions;
    dwi::fast_vec<SModCommand> m_commands;
};

CScene::CScene()
    : m_node0(0), m_value24(0), m_value28(0), m_value32(0), m_value36(0), m_value40(0), m_value44(0), m_value48(0), m_value52(0), m_count56(0), m_value60(0), m_value64(0), m_value68(0), m_updating(false), m_value76(0), m_lights(2), m_shadows(0xDEADBEEF), m_pairsX(5), m_pairsY(6),
      m_exclusions(9), m_commands(10) {
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
