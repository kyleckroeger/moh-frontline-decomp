// A fragment of particlesystem.cpp (0x80080ec4): CParticleSystem::Reset (the
// manager's scene-node links cleared, the manager added to the scene again,
// both particle pools reset to all-free) and InitClass (the ready flag, render
// list, screen and scene kept, the width and height scales as half the
// screen's width and height, the manager registered with the render list and
// added to the scene, and two pools of 128 nodes allocated and reset). The
// file name is this project's; the original record is particlesystem.cpp.
// The classes, functions and globals are named by the mangled symbols; the
// manager is a render bin with a scene node at +32 (as its @32@ thunks show);
// its members, its pools, their 272- and 304-byte nodes and the pool's Init
// and Reset inlines are inferred (the pool view is light_statics.cpp's); the
// statics are the file's and the 0.5 constant is an entry of the file's
// .sdata2 pool. CScreen declares its virtual functions up to GetWidth, in
// table order; CRenderBin and the manager declare a virtual function defined
// elsewhere first so their tables stay elsewhere.
class CDmaTag;
class CDmaPacket;

class CRenderBin;

struct CRenderBinData {
    CRenderBin* m_prev;
    CRenderBin* m_next;
    CRenderBin* m_children;
    int data0c;
    int data10;
    int m_priority;
    int data18;
};

class CRenderBin : public CRenderBinData {
public:
    virtual int Render(CDmaPacket&, void*);
};

void* DWI_alloc(const char*, int, int);

struct CColor;
struct CRect;

#include "game/CScreen.h"


class ISceneNode {
public:
    virtual void MarkForDestruction(int);

    void* m_scenePrev;
    void* m_sceneNext;
};

class CScene {
public:
    void Add(ISceneNode&);
};

class CRenderList {
public:
    void Register(CRenderBin&);
};

/* Inferred: a pool node, a particle record with the next link after it. */
template <int N>
struct ParticleNodeView {
    unsigned char data[N];
    ParticleNodeView* next;
    unsigned char unknown[12];
};

/* Inferred: a fixed pool of nodes with in-use and free lists. */
template <class T>
struct ParticlePoolView {
    void Reset() {
        m_used = 0;
        m_free = m_data;
        m_count = 0;
        m_data[m_capacity - 1].next = 0;
        for (int i = 0; i < m_capacity - 1; i++)
            m_free[i].next = &m_free[i + 1];
    }
    void Init(int capacity) {
        m_capacity = capacity;
        m_data = (T*)DWI_alloc(0, m_capacity * sizeof(T), 1024);
        Reset();
    }

    T* m_data;
    T* m_used;
    T* m_free;
    int m_capacity;
    int m_count;
};

/* Inferred: the texture-bin list and the pools of the manager. */
class CParticleSystemManager : public CRenderBin, public ISceneNode {
public:
    virtual void BeginUpdate(float);

    unsigned char unknown2c[36];
    void* m_bins;
    ParticlePoolView<ParticleNodeView<288> > m_pool54;
    ParticlePoolView<ParticleNodeView<256> > m_pool68;
};

extern CParticleSystemManager g_ParticleSystemManager;
extern bool g_bReady;
extern CRenderList* g_pRenderList;
extern CScreen* g_pScreen;
extern CScene* g_pScene;
extern float g_WidthScale;
extern float g_HeightScale;

class CParticleSystem {
public:
    static void Reset();
    static void InitClass(CRenderList*, CScreen*, CScene*);
};

void CParticleSystem::Reset() {
    g_ParticleSystemManager.m_sceneNext = 0;
    g_ParticleSystemManager.m_scenePrev = 0;
    g_pScene->Add(g_ParticleSystemManager);
    g_ParticleSystemManager.m_pool68.Reset();
    g_ParticleSystemManager.m_pool54.Reset();
}

void CParticleSystem::InitClass(CRenderList* list, CScreen* screen, CScene* scene) {
    g_bReady = true;
    g_pRenderList = list;
    g_pScreen = screen;
    g_pScene = scene;
    g_WidthScale = 0.5f * screen->GetWidth();
    g_HeightScale = 0.5f * g_pScreen->GetHeight();
    list->Register(g_ParticleSystemManager);
    scene->Add(g_ParticleSystemManager);
    g_ParticleSystemManager.m_pool68.Init(128);
    g_ParticleSystemManager.m_pool54.Init(128);
}
