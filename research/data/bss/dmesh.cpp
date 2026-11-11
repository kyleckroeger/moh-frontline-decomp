// dmesh.cpp: its zero-initialised globals (0x14 bytes), written by research/tools/bss_decls.py
// and checked by check_data.py. Views marked "view" hold the size only; real layouts come with the shared headers.

struct CRenderList;
struct FogView;

CRenderList* _renderList; // file-local in the original // address: _renderList 0x8034ed88
int _remap_next; // file-local in the original // address: _remap_next 0x8034ed8c
static FogView* g_pFog; // address: g_pFog 0x8034ed90
static float g_NearPlane; // address: g_NearPlane 0x8034ed94
static float g_FarPlane; // address: g_FarPlane 0x8034ed98
