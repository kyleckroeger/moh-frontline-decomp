// Pause.cpp: its zero-initialised globals (0x32392 bytes), written by research/tools/bss_decls.py
// and checked by check_data.py. Views marked "view" hold the size only; real layouts come with the shared headers.

struct ObjectiveListView { unsigned char data[0x324]; }; // view: size from the original
struct PathxTrackView;

char g_cheatSequence[9]; // file-local in the original // address: g_cheatSequence 0x801f70e0
ObjectiveListView theObjectiveList;
PathxTrackView* pathxinfo[24];
unsigned char SoundBankHeapStart[204800];
unsigned int g_cheatSequenceIndex; // file-local in the original // address: g_cheatSequenceIndex 0x8034ec98
bool g_unplugged;
