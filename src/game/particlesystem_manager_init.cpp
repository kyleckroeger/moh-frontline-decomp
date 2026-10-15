// A fragment of particlesystem.cpp (0x8007ed30): CParticleSystemManager's
// IsUsed (whether it holds any texture bins) and Init (clears two counters,
// re-initialises each texture bin through its table and drops them). The
// classes and functions are named by the mangled symbols; the render-bin
// data members are as in particlesystem_reset.cpp (the bins are the
// children list), and the manager is declared only as far as its
// render-bin part with its destructor (defined elsewhere) first, so no table
// is emitted here. The rest of the file is not part of this unit.
class CDmaPacket;
class CDmaTag;

class CRenderBinData {
public:
    CRenderBinData* m_prev;
    CRenderBinData* m_next;
    CRenderBinData* m_children;
    int data0c;
    int data10;
    int m_priority;
    int data18;
};

class CRenderBin : public CRenderBinData {
public:
    virtual ~CRenderBin();
    virtual void Render(CDmaPacket&, void*);
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();
};

/* inferred: only the render-bin part of the manager is used here */
class CParticleSystemManager : public CRenderBin {
public:
    virtual ~CParticleSystemManager();
    virtual void Init();
    virtual bool IsUsed();
};

bool CParticleSystemManager::IsUsed() {
    return m_children != 0;
}

void CParticleSystemManager::Init() {
    data0c = 0;
    data10 = 0;
    for (CRenderBinData* bin = m_children; bin; bin = bin->m_next)
        ((CRenderBin*)bin)->Init();
    m_children = 0;
}
