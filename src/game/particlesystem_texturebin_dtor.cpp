// A fragment of particlesystem.cpp (0x8007edb0): the CParticleSystemTextureBin
// destructor (its table pointer, the shape cleared, then the inline CRenderBin
// destructor and the object freed when asked). The file name is this
// project's; the original record is particlesystem.cpp. The classes are named
// by the mangled symbols; the members follow particlesystem_register.cpp. The
// bin declares another of its own virtual functions (defined elsewhere) before
// its destructor, so its global table is not emitted here; CRenderBin's table
// is weak in the original, so the compiler's copy is a weak duplicate.
class CDmaTag;
class CDmaPacket;
struct ShapeFile;

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
    virtual ~CRenderBin() {}
    virtual int Render(CDmaPacket&, void*);
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();
};

class CTexture {
public:
    unsigned char unknown00[64];
};

class CParticleSystemTextureBin : public CRenderBin {
public:
    virtual int Render(CDmaPacket&, void*);
    virtual ~CParticleSystemTextureBin();

    CTexture m_texture;
    ShapeFile* m_shape;
};

CParticleSystemTextureBin::~CParticleSystemTextureBin() {
    m_shape = 0;
}
