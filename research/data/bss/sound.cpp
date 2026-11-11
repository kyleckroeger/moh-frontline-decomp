// sound.cpp: its zero-initialised globals (0xaf071 bytes), written by research/tools/bss_decls.py
// and checked by check_data.py. Views marked "view" hold the size only; real layouts come with the shared headers.

struct SoundPlayOptsView { unsigned char data[0x18]; }; // view: size from the original
struct CSoldierObject;

int gAEMSbHandle[8]; // file-local in the original // address: gAEMSbHandle 0x80229470
void* gAEMSBank[8]; // file-local in the original // address: gAEMSBank 0x802294b0
char gAEMSStreamBuf[0x32000]; // file-local in the original // address: gAEMSStreamBuf 0x802294d0
char g_AEMSOneShotStreamBuf[0xc800]; // file-local in the original // address: g_AEMSOneShotStreamBuf 0x8025b4d0
char g_AEMSOneShotMem[0x70800]; // file-local in the original // address: g_AEMSOneShotMem 0x80267cd0
SoundPlayOptsView gMusicOpts;
CSoldierObject* g_objSound; // file-local in the original // address: g_objSound 0x8034ecbc
int gAEMSBankCount; // file-local in the original // address: gAEMSBankCount 0x8034ecc0
void* gAEMSsystem; // file-local in the original // address: gAEMSsystem 0x8034ecc4
bool gAEMSStreamUsed; // file-local in the original // address: gAEMSStreamUsed 0x8034ecc8
int g_AEMSOneShotMemOffset;
int g_AEMSOneShotLoadState;
void* gMusicBuf;
