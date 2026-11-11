// font.cpp: its zero-initialised globals (0x15 bytes), written by research/tools/bss_decls.py
// and checked by check_data.py. Views marked "view" hold the size only; real layouts come with the shared headers.

struct CRenderList;
struct CScreen;
struct CFont;

static CRenderList* g_pRenderList; // address: g_pRenderList 0x8034ea00
static CScreen* g_pScreen; // address: g_pScreen 0x8034ea04
static bool g_bReady; // address: g_bReady 0x8034ea08
CFont* g_pCurrentFont; // file-local in the original // address: g_pCurrentFont 0x8034ea0c
static float g_ScreenXOffset; // address: g_ScreenXOffset 0x8034ea10
static float g_ScreenYOffset; // address: g_ScreenYOffset 0x8034ea14
