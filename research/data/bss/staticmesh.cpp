// staticmesh.cpp: its zero-initialised globals (0x2e bytes), written by research/tools/bss_decls.py
// and checked by check_data.py. Views marked "view" hold the size only; real layouts come with the shared headers.

struct CRenderList;
struct CScreen;
struct BPDLightVolume;
struct CDrawContext;
struct FogView;
struct CMatrix;
struct CMSHMatBin;

static CRenderList* g_pRenderList; // address: g_pRenderList 0x8034ed00
static CScreen* g_pScreen; // address: g_pScreen 0x8034ed04
BPDLightVolume* g_pLightVolume; // file-local in the original // address: g_pLightVolume 0x8034ed08
bool g_bAllowLighting; // file-local in the original // address: g_bAllowLighting 0x8034ed0c
static CDrawContext* g_pContext; // address: g_pContext 0x8034ed10
static void* g_pFrustum; // address: g_pFrustum 0x8034ed14
static FogView* g_pFog; // address: g_pFog 0x8034ed18
CMatrix* g_pModelToWorld; // file-local in the original // address: g_pModelToWorld 0x8034ed1c
bool g_bCacheLightParam;
static int g_CurrState; // address: g_CurrState 0x8034ed24
static CMSHMatBin* g_CurrMaterial; // address: g_CurrMaterial 0x8034ed28
static float g_NearPlane; // address: g_NearPlane 0x8034ed2c
static float g_FarPlane; // address: g_FarPlane 0x8034ed30
