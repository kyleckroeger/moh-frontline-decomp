// A fragment of staticmesh.cpp (0x800f2478): CStaticMesh::InitClass, which
// on its first call records the screen and the render list, registers the
// two simple-mesh render bins, clears the light volume and zeroes the light
// parameters. The bins and g_LightParam belong to the file's .bss block and
// are reached from its base register, so the block is defined here with
// size views (no constructors) and pooled; the staticmesh.cpp unit places it.
// CRenderList, CRenderBin and CStaticMesh are named by the symbols; the
// sizes come from the original's symbol table.
extern "C" void* memset(void*, int, unsigned long);

class CRenderBin;

class CRenderList {
public:
    void Register(CRenderBin&);
};

class CScreen;
struct BPDLightVolume;

class CStaticMesh {
public:
    static void InitClass(CRenderList*, CScreen*);
};

extern CRenderList* g_pRenderList;
extern CScreen* g_pScreen;
extern BPDLightVolume* g_pLightVolume;

/* view: the file's .bss block in original order; the matrices are CMatrix,
   g_CamPos a CVector3 and the bins CMshSimpleBin objects with their
   destructor-chain records (@465, @466) before them */
struct MatrixView {
    float m[16];
};
struct CamPosView {
    float v[4];
};
struct SLightParam {
    unsigned char unknown00[96];
};
struct DestructorChainView {
    unsigned char unknown00[12];
};
class CRenderBin {
    unsigned char unknown00[36];
};

static MatrixView g_ActualWorldToCameraMatrix;
static MatrixView g_WorldToCameraMatrix;
static MatrixView g_ModelToScreenMatrix;
static MatrixView g_ModelToClipMatrix;
static MatrixView g_ClipToScreenMatrix;
static CamPosView g_CamPos;
SLightParam g_LightParam;
static DestructorChainView s_simpleBinChain;
static CRenderBin g_MshSimpleBin;
static DestructorChainView s_simpleAlphaBinChain;
static CRenderBin g_MshSimpleAlphaBin;

void CStaticMesh::InitClass(CRenderList* list, CScreen* screen) {
    static bool bReady = false;
    if (!bReady) {
        g_pScreen = screen;
        g_pRenderList = list;
        list->Register(g_MshSimpleBin);
        list->Register(g_MshSimpleAlphaBin);
        g_pLightVolume = 0;
        memset(&g_LightParam, 0, sizeof(g_LightParam));
    }
}
