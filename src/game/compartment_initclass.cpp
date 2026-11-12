// A fragment of compartment.cpp (0x80075db4): CCompartment::InitClass, which
// on its first call records the render list and the screen, registers the
// simple, alpha and shadow render bins and links the 256-entry light pool
// into its free list. The bins belong to the file's .bss block and are
// reached from its base register, so the block up to them is defined here
// with size views (no constructors) and pooled; the compartment.cpp unit
// places it. CRenderList, CRenderBin and CCompartment are named by the
// symbols; the sizes come from the original's symbol table and the light
// pool's layout is inferred (as matched in compartment_shadowbins.cpp).
class CRenderBin;

class CRenderList {
public:
    void Register(CRenderBin&);
};

class CScreen;

class CCompartment {
public:
    static void InitClass(CRenderList*, CScreen*);
};

extern CRenderList* g_pRenderList;
extern CScreen* g_pScreen;

/* The file's local light pool (g_LightPool, 0xc14 bytes): 256 entries of 12
   bytes linked through +8, then the pointers and counts (as matched in
   compartment_shadowbins.cpp; Init is inferred). */
struct CLightPoolEntry {
    int data00;
    int data04;
    CLightPoolEntry* m_next;
};

struct CLightPool {
    void Init(int capacity) {
        m_capacity = capacity;
        m_data = m_storage;
        Reset();
    }
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

extern CLightPool g_LightPool;

/* view: the file's .bss block in original order up to the bins; the
   matrices are CMatrix (the GameCube one 3x4), g_CamPos a CVector3,
   g_GlobalCpt a CCompartment and the bins render bins, each object with a
   destructor after its destructor-chain record (@537..@540) */
struct MatrixView {
    float m[16];
};
struct GCMatrixView {
    float m[12];
};
struct CamPosView {
    float v[4];
};
struct DestructorChainView {
    unsigned char unknown00[12];
};
struct CompartmentView {
    double unknown00[13];
};
class CRenderBin {
    unsigned char unknown00[36];
};

static MatrixView g_ModelToScreenMatrix;
static MatrixView g_ModelToClipMatrix;
static MatrixView g_ClipToScreenMatrix;
static GCMatrixView g_WorldToCameraGCMatrix;
static CamPosView g_CamPos;
static DestructorChainView s_globalCptChain;
static CompartmentView g_GlobalCpt;
static DestructorChainView s_simpleBinChain;
static CRenderBin g_CptSimpleBin;
static DestructorChainView s_simpleAlphaBinChain;
static CRenderBin g_CptSimpleAlphaBin;
static DestructorChainView s_shadowBinChain;
static CRenderBin g_CptShadowBin;

void CCompartment::InitClass(CRenderList* list, CScreen* screen) {
    static bool bReady = false;
    if (!bReady) {
        g_pRenderList = list;
        g_pScreen = screen;
        list->Register(g_CptSimpleBin);
        list->Register(g_CptSimpleAlphaBin);
        list->Register(g_CptShadowBin);
        g_LightPool.Init(256);
    }
}
