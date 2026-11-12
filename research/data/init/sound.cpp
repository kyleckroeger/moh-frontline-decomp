// sound.cpp: its initialised globals (.data, .sdata). gMusicHandle is global in the
// original; the rest are file statics, defined global here because the file's fragments
// reach them with extern. gAEMSAmbientTrackEvent is the ambient track's AEMS event
// record (first word 12, the rest zero); its layout is a view.
// Checked by research/tools/check_data.py.
// settings: config/GMFE69/sound.json

int gAEMSAmbientTrackEvent[5] = {12, 0, 0, 0, 0}; // file-local in the original; view
float g_sfxVolume = 1.0f; // file-local in the original
float g_musicVolume = 1.0f; // file-local in the original
bool g_ambientSourceOnPlayer = true; // file-local in the original
int g_hAEMSAmbientTrack = -1; // file-local in the original
int gMusicHandle = -1;

// File inferred: no file map range covers these two, but the original places them in
// .sdata beside sound.cpp's globals (SoundBankHeapSize just before g_sfxVolume,
// gMusicProjectID just after gMusicHandle) and only sound.cpp uses them.
int SoundBankHeapSize = 0x32000;
int gMusicProjectID = -1;
