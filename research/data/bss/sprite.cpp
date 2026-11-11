// sprite.cpp: its zero-initialised globals (0x10 bytes), written by research/tools/bss_decls.py
// and checked by check_data.py. Views marked "view" hold the size only; real layouts come with the shared headers.

struct CRenderList;
struct CScreen;

static CRenderList* g_pRenderList; // address: g_pRenderList 0x8034ea60
static CScreen* g_pScreen; // address: g_pScreen 0x8034ea64
static float g_ScreenXOffset; // address: g_ScreenXOffset 0x8034ea68
static float g_ScreenYOffset; // address: g_ScreenYOffset 0x8034ea6c
