// particlesystem.cpp: its zero-initialised globals (0x2e bytes), written by research/tools/bss_decls.py
// and checked by check_data.py. Views marked "view" hold the size only; real layouts come with the shared headers.

struct CRenderList;
struct CScreen;
struct CScene;
struct CDrawContext;
struct FogView;

static CRenderList* g_pRenderList; // address: g_pRenderList 0x8034ea20
static CScreen* g_pScreen; // address: g_pScreen 0x8034ea24
CScene* g_pScene; // file-local in the original // address: g_pScene 0x8034ea28
static CDrawContext* g_pContext; // address: g_pContext 0x8034ea2c
float g_WidthScale; // file-local in the original // address: g_WidthScale 0x8034ea30
float g_HeightScale; // file-local in the original // address: g_HeightScale 0x8034ea34
static bool g_bReady; // address: g_bReady 0x8034ea38
bool g_bPaused; // file-local in the original // address: g_bPaused 0x8034ea39
static FogView* g_pFog; // address: g_pFog 0x8034ea3c
int g_RenderTypeCache;
int g_ParticleCounter; // file-local in the original // address: g_ParticleCounter 0x8034ea44
int g_ParticleCounterMax; // file-local in the original // address: g_ParticleCounterMax 0x8034ea48
int g_CurrentSeed;
