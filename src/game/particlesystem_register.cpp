// A fragment of particlesystem.cpp (0x80080db0): CParticleSystem::Register
// for a shape file. When the manager already holds a texture bin for the
// shape, the duplicate is reported (a .rodata pool string); otherwise a new
// texture bin is built for it (inline CRenderBinData and CRenderBin
// constructors; its texture set from the shape) and linked into the manager's
// bin list after its first entry, or as the first entry. The file name is
// this project's; the original record is particlesystem.cpp. The classes,
// functions and globals are named by the mangled symbols; the members, the
// manager view (its bin list at +80) and the search and insertion helpers
// (inline here, their names are this project's) are inferred. CParticleSystemTextureBin declares
// its destructor (defined elsewhere) first so its table stays elsewhere.
class CDmaTag;
class CDmaPacket;
struct ShapeFile;

void DebugMsg(const char*, ...);

class CRenderBin;

struct CRenderBinData {
    CRenderBinData() : m_prev(0), m_next(0), m_children(0), m_lastTag(0), m_firstTag(0), m_priority(3) {}

    CRenderBin* m_prev;
    CRenderBin* m_next;
    CRenderBin* m_children;
    int m_lastTag;
    int m_firstTag;
    int m_priority;
    int data18;
};

class CRenderBin : public CRenderBinData {
public:
    virtual ~CRenderBin() {}
    virtual int Render(CDmaPacket&, void*);
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();

    void InsertAfter(CRenderBin* bin) {
        bin->m_next = m_next;
        if (m_next)
            m_next->m_prev = bin;
        bin->m_prev = this;
        m_next = bin;
    }
};

class CTexture {
public:
    void Set(ShapeFile*, bool);

    unsigned char unknown00[64];
};

class CParticleSystemTextureBin : public CRenderBin {
public:
    virtual ~CParticleSystemTextureBin();
    CParticleSystemTextureBin(ShapeFile* shape) : m_shape(shape) { m_texture.Set(m_shape, false); }

    CTexture m_texture;
    ShapeFile* m_shape;
};

/* Inferred: only the texture-bin list of the manager is part of this view. */
class CParticleSystemManager {
public:
    unsigned char unknown00[80];
    CParticleSystemTextureBin* m_bins;
};

extern CParticleSystemManager g_ParticleSystemManager;

static inline CParticleSystemTextureBin* FindBin(ShapeFile* shape) {
    for (CParticleSystemTextureBin* bin = g_ParticleSystemManager.m_bins; bin;
         bin = (CParticleSystemTextureBin*)bin->m_next) {
        if (bin->m_shape == shape)
            return bin;
    }
    return 0;
}

class CParticleSystem {
public:
    static void Register(ShapeFile*);
};

void CParticleSystem::Register(ShapeFile* shape) {
    if (FindBin(shape)) {
        DebugMsg("CParticleSystemManager::Register():  ALREADY REGISTERED");
        return;
    }
    CParticleSystemTextureBin* bin = new CParticleSystemTextureBin(shape);
    if (g_ParticleSystemManager.m_bins)
        g_ParticleSystemManager.m_bins->InsertAfter(bin);
    else
        g_ParticleSystemManager.m_bins = bin;
}
