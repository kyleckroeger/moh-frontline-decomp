// A fragment of particlesystem.cpp (0x8007e8d0):
// CParticleSystemManager::BeginUpdate sets the global pause flag (for a
// non-positive step), advances the manager's time and walks both particle
// system pools: an active system is updated by the step, an inactive one is
// returned to its pool's free list (unlinked from the in-use list, the count
// lowered) through an inferred inline pool helper. The classes are named by
// the mangled symbols; the render-bin, particle-system (virtual slots +36
// active and +192 update; the rest are placeholders), scene node and pool
// views are inferred. The rest of the file is in other units.
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
    virtual ~CRenderBin();
    virtual int Render(CDmaPacket&, void*);
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();
};

class CParticleSystem : public CRenderBin {
public:
    virtual void DeActivate();
    virtual void Terminate();
    virtual bool IsActive() const;
    virtual void vf028();
    virtual void vf02c();
    virtual void vf030();
    virtual void vf034();
    virtual void vf038();
    virtual void vf03c();
    virtual void vf040();
    virtual void vf044();
    virtual void vf048();
    virtual void vf04c();
    virtual void vf050();
    virtual void vf054();
    virtual void vf058();
    virtual void vf05c();
    virtual void vf060();
    virtual void vf064();
    virtual void vf068();
    virtual void vf06c();
    virtual void vf070();
    virtual void vf074();
    virtual void vf078();
    virtual void vf07c();
    virtual void vf080();
    virtual void vf084();
    virtual void vf088();
    virtual void vf08c();
    virtual void vf090();
    virtual void vf094();
    virtual void vf098();
    virtual void vf09c();
    virtual void vf0a0();
    virtual void vf0a4();
    virtual void vf0a8();
    virtual void vf0ac();
    virtual void vf0b0();
    virtual void vf0b4();
    virtual void vf0b8();
    virtual void vf0bc();
    virtual void UpdateSystem(float);
};

void* DWI_alloc(const char*, int, int);

struct CColor;
struct CRect;

class CScreen {
public:
    virtual ~CScreen();
    virtual void SetCurrent();
    virtual void Flip();
    virtual unsigned long Wait();
    virtual void SetClear(bool);
    virtual void SetClearColor(const CColor&);
    virtual void SetClearRect(const CRect&);
    virtual int GetHeight();
    virtual int GetWidth();
};

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
    T* Free(T* node) {
        T* next = node->next;
        // A no-op self-assignment (user-approved, like '(void)0;'): the
        // compiler drops the store, but it keeps the unlink loop from reusing
        // 'next', so node->next is reloaded there as in the original.
        node->next = next;
        if (node == m_used) {
            m_used = next;
        } else {
            for (T* q = m_used; q; q = q->next) {
                if (q->next == node) {
                    q->next = node->next;
                    break;
                }
            }
        }
        m_count--;
        node->next = m_free;
        m_free = node;
        return next;
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
    virtual ~CParticleSystemManager();
    virtual void BeginUpdate(float);

    unsigned char unknown2c[24];
    float m_time;
    unsigned char unknown48[8];
    void* m_bins;
    ParticlePoolView<ParticleNodeView<288> > m_pool54;
    ParticlePoolView<ParticleNodeView<256> > m_pool68;
};

extern bool g_bPaused;

void CParticleSystemManager::BeginUpdate(float dt) {
    g_bPaused = dt > 0.0f;
    m_time += dt;
    ParticleNodeView<288>* node = m_pool54.m_used;
    while (node) {
        CParticleSystem* system = (CParticleSystem*)node;
        if (system->IsActive()) {
            system->UpdateSystem(dt);
            node = node->next;
        } else {
            node = m_pool54.Free(node);
        }
    }
    ParticleNodeView<256>* node2 = m_pool68.m_used;
    while (node2) {
        CParticleSystem* system = (CParticleSystem*)node2;
        if (system->IsActive()) {
            system->UpdateSystem(dt);
            node2 = node2->next;
        } else {
            node2 = m_pool68.Free(node2);
        }
    }
}
