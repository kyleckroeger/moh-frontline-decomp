// player.cpp: its zero-initialised globals (0x7c bytes), written by research/tools/bss_decls.py
// and checked by check_data.py. Views marked "view" hold the size only; real layouts come with the shared headers.

struct CSprite;
struct CScreen;
struct ISceneNode;

unsigned char g_pMGCrossHairMem[100]; // file-local in the original // address: g_pMGCrossHairMem 0x801ef020
CSprite* g_pMGCrossHair; // file-local in the original // address: g_pMGCrossHair 0x8034eae0
CSprite* g_pBazookaOverlay; // file-local in the original // address: g_pBazookaOverlay 0x8034eae4
static CScreen* g_pScreen; // address: g_pScreen 0x8034eae8
int g_cScreenWidth; // file-local in the original // address: g_cScreenWidth 0x8034eaec
int g_cScreenHeight; // file-local in the original // address: g_cScreenHeight 0x8034eaf0
ISceneNode* g_pCamera; // file-local in the original // address: g_pCamera 0x8034eaf4
