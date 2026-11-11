// compartment.cpp: its zero-initialised globals (0x7a bytes), written by research/tools/bss_decls.py
// and checked by check_data.py. Views marked "view" hold the size only; real layouts come with the shared headers.

struct CVector3 { unsigned char data[0x10]; }; // view: size from the original
struct CRenderList;
struct CScreen;
struct FogView;
struct CCharacterShadow;
struct CMSHMatBin;

float g_WorldToCameraGCMatrix[3][4]; // file-local in the original // address: g_WorldToCameraGCMatrix 0x801d4fa0
static CVector3 g_CamPos; // address: g_CamPos 0x801d4fd0
static CRenderList* g_pRenderList; // address: g_pRenderList 0x8034e980
static CScreen* g_pScreen; // address: g_pScreen 0x8034e984
float g_NearClip; // file-local in the original // address: g_NearClip 0x8034e988
static void* g_pFrustum; // address: g_pFrustum 0x8034e98c
static FogView* g_pFog; // address: g_pFog 0x8034e990
float g_MipMapBias; // file-local in the original // address: g_MipMapBias 0x8034e994
bool g_bMipMapScale; // file-local in the original // address: g_bMipMapScale 0x8034e998
bool g_bShadowSet; // file-local in the original // address: g_bShadowSet 0x8034e999
CCharacterShadow* g_pCurrentShadow; // file-local in the original // address: g_pCurrentShadow 0x8034e99c
int g_SharedCompartmentSize; // file-local in the original // address: g_SharedCompartmentSize 0x8034e9a0
bool g_bCPTFogDisabled;
bool g_bCPTForceClipped;
bool g_bCPTDisableLighting;
bool g_bDisableMipMapBias;
int g_TexturesCached;
static int g_CurrState; // address: g_CurrState 0x8034e9ac
static CMSHMatBin* g_CurrMaterial; // address: g_CurrMaterial 0x8034e9b0
static float g_NearPlane; // address: g_NearPlane 0x8034e9b4
static float g_FarPlane; // address: g_FarPlane 0x8034e9b8
