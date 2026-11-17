// A fragment of compartment.cpp (0x800742d8): the shadow bins' small
// overrides (CCptShadowCleanUpBin::IsUsed is the shadow bin's inline IsUsed),
// CUcodeRenderBin's weak destructor and the CCptSimpleBin overrides (Init
// resets the file's light pool for priority-2 bins, then runs CRenderBin's
// inline Init). The file name is this project's; the original record is
// compartment.cpp. The classes and functions are named by the mangled
// symbols, CRenderBinData by CRenderBin's RTTI record; member names, the
// layout of the pool (g_LightPool, a local of this file) and the result types
// are inferred. Each bin declares its destructor (weak in the original,
// emitted elsewhere in the file) first, so the bins' global tables are not
// emitted here; CUcodeRenderBin declares its Link override first for the same
// reason. The compiler's out-of-line copy of CRenderBin's inline Init is a
// weak duplicate.
class CDmaTag;
class CDmaPacket;
class CRenderBin;

/* CRenderBin's non-polymorphic base (see dmesh_cremapbin_ctor.cpp): Init
   clears +12 and +16 and walks the children from +8 through the siblings at
   +4; IsUsed tests +16; +20 is the render priority. */
struct CRenderBinData {
    int data00;
    CRenderBin* m_next;
    CRenderBin* m_children;
    int m_lastTag;
    int m_firstTag;
    int m_priority;
    int data18;
};

/* CRenderBin's virtuals in table order (Init is slot 16); all are inline in
   the original. Render and Link (weak copies in compartment_bins.cpp) are
   declared without a body, which keeps CRenderBin's table out of this unit. */
class CRenderBin : public CRenderBinData {
public:
    virtual ~CRenderBin() {}
    virtual int Render(CDmaPacket&, void*);
    virtual void Init() {
        m_lastTag = 0;
        m_firstTag = 0;
        for (CRenderBin* child = m_children; child; child = child->m_next)
            child->Init();
    }
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed() { return m_firstTag != 0; }
};

class CPTMaterial : public CRenderBin {
public:
    virtual ~CPTMaterial();
};

class CUcodeRenderBin : public CRenderBin {
public:
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual ~CUcodeRenderBin();
};

class CCptShadowBin : public CRenderBin {
public:
    virtual ~CCptShadowBin();
    virtual int Render(CDmaPacket&, void*);
    virtual void Link(CDmaTag*, CDmaPacket&);
};

class CCptShadowCleanUpBin : public CRenderBin {
public:
    virtual ~CCptShadowCleanUpBin();
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();
};

class CCptSimpleBin : public CRenderBin {
public:
    virtual ~CCptSimpleBin();
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();

    bool m_used;
};

/* The file's local light pool (g_LightPool, 0xc14 bytes): 256 entries of 12
   bytes linked through +8, then the pointers and counts Init resets. */
struct CLightPoolEntry {
    int data00;
    int data04;
    CLightPoolEntry* m_next;
};

struct CLightPool {
    void Reset() {
        m_used = 0;
        m_free = m_data;
        m_count = 0;
        m_data[m_capacity - 1].m_next = 0;
        for (int i = 0; i < m_capacity - 1; i++)
            m_free[i].m_next = &m_free[i + 1];
    }

    CLightPoolEntry m_storage[256];
    CLightPoolEntry* m_data;
    CLightPoolEntry* m_used;
    CLightPoolEntry* m_free;
    int m_capacity;
    int m_count;
};

extern CCptShadowBin g_CptShadowBin;
extern CLightPool g_LightPool;

bool CCptShadowCleanUpBin::IsUsed() {
    return g_CptShadowBin.IsUsed();
}

void CCptShadowCleanUpBin::Link(CDmaTag*, CDmaPacket&) {
}

void CCptShadowBin::Link(CDmaTag*, CDmaPacket&) {
}

int CCptShadowBin::Render(CDmaPacket&, void*) {
    return 0;
}

__declspec(weak) CUcodeRenderBin::~CUcodeRenderBin() {}

void CCptSimpleBin::Init() {
    if (m_priority == 2)
        g_LightPool.Reset();
    m_used = false;
    CRenderBin::Init();
}

bool CCptSimpleBin::IsUsed() {
    return m_used;
}

void CCptSimpleBin::Link(CDmaTag*, CDmaPacket&) {
}
