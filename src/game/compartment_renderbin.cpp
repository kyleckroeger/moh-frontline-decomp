// A fragment of compartment.cpp (0x80073b2c): the weak copies of
// CRenderBin's inline Init (clears +12 and +16, then re-inits each child) and
// IsUsed (+16 non-zero) emitted in this file, and the CTextureSwapCache
// destructor (the file-local g_TextureSwapCache's class; nothing to destroy).
// The file name is this project's; the original record is compartment.cpp.
// The classes and functions are named by the mangled symbols, CRenderBinData
// by CRenderBin's RTTI record; member names and the result type are inferred.
// CRenderBin's destructor (weak, emitted elsewhere in the file) is declared
// without a body, so CRenderBin's table is not emitted here.
class CDmaTag;
class CDmaPacket;
class CRenderBin;

/* CRenderBin's non-polymorphic base (see dmesh_cremapbin_ctor.cpp). */
struct CRenderBinData {
    int m_prev;
    CRenderBin* m_next;
    CRenderBin* m_children;
    int m_lastTag;
    int m_firstTag;
    int m_priority;
    int data18;
};

/* CRenderBin's virtuals in table order (Init is slot 16). */
class CRenderBin : public CRenderBinData {
public:
    virtual ~CRenderBin();
    virtual int Render(CDmaPacket&, void*);
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();
};

/* Only the destructor is part of this view (0x8020 bytes of members). */
class CTextureSwapCache {
public:
    ~CTextureSwapCache();
};

__declspec(weak) void CRenderBin::Init() {
    m_lastTag = 0;
    m_firstTag = 0;
    for (CRenderBin* child = m_children; child; child = child->m_next)
        child->Init();
}

__declspec(weak) bool CRenderBin::IsUsed() {
    return m_firstTag != 0;
}

CTextureSwapCache::~CTextureSwapCache() {
}
