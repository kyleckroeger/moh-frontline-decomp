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



#include "game/CRenderBin.h"


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
    m_lastTag = 0;
    m_firstTag = 0;
    for (CRenderBinData* bin = m_children; bin; bin = bin->m_next)
        ((CRenderBin*)bin)->Init();
    m_children = 0;
}
