// bsbifunc.cpp: its initialised globals (.sdata): the script prompt's state and the
// current mission, level, sound bank and weapon set. The prompt globals are file statics
// in the original, defined global here because the file's fragments reach them with extern.
// Checked by research/tools/check_data.py.

extern char g_promptBuffer[];

int g_promptstart = 19; // file-local in the original
char* g_promptCursor = g_promptBuffer; // file-local in the original
int g_promptLineWidth = 40; // file-local in the original
int g_promptMaxSize = 48; // file-local in the original
int g_CurrentMission = -1;
int g_CurrentLevel = -1;
int g_CurrentSoundBank = -1;
int g_levelWeaponSetID = 1;
