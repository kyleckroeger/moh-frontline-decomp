// mainloop.cpp: its zero-initialised globals (0x108 bytes), written by research/tools/bss_decls.py
// and checked by check_data.py. Views marked "view" hold the size only; real layouts come with the shared headers.

struct AnimFileFormat_t;
struct CWorldObject;

char g_soundStreamFilename[256];
AnimFileFormat_t* g_pLevelEMT; // file-local in the original // address: g_pLevelEMT 0x8034e9d8
CWorldObject* g_pWorld;
