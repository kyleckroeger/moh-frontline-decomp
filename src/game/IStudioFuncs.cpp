// The shell's big file: unloading frees it, loading frees any previous copy
// and loads the shell big file through the level loader (alignment 256). The
// globals are named by the symbols; their types are inferred. The empty
// DummyPluginProcessFnc follows (its parameter meanings are not known). The
// rest of the file is not part of this unit.
extern "C" void MEM_free(void*);
void* TLT_LoadFileNormal(const char*, int*, int);

extern void* g_pLevelBigFile;
extern int g_LevelBigFileSize;
extern const char* szShellBigFileName;

void UnloadShellBigFile() {
    if (g_pLevelBigFile) {
        MEM_free(g_pLevelBigFile);
        g_pLevelBigFile = 0;
    }
}

void LoadShellBigFile() {
    if (g_pLevelBigFile)
        MEM_free(g_pLevelBigFile);
    g_pLevelBigFile = TLT_LoadFileNormal(szShellBigFileName, &g_LevelBigFileSize, 256);
}

struct UISParam_t;

void DummyPluginProcessFnc(void*, unsigned long, unsigned long, UISParam_t*, UISParam_t*) {
}
