// A fragment of toilet.cpp (0x8001dde8): TLT_Shutdown frees the level big
// file and the compartment big-file header, closes the compartment big file
// (waiting up to 100) and clears the level-contents globals. The function and
// globals are named by the symbols; the globals' types are inferred, and the
// file-local ones are declared extern here. The rest of the file is not part
// of this unit.
extern "C" void MEM_free(void*);
extern "C" void FILESYS_delbigsync(int, int);

struct LevelFileContentsStruct_;

extern void* g_pLevelBigFile;
extern void* g_pCompartmentBigFileHeader;
extern int g_CompartmentBigFile;
extern LevelFileContentsStruct_* g_pLevelFileContentsArray;
extern LevelFileContentsStruct_* g_pLevelFileContentsEndMarker;
extern int g_pNumLevelFileContents;
extern int g_LevelBigFileSize;

void TLT_Shutdown() {
    if (g_pLevelBigFile) {
        MEM_free(g_pLevelBigFile);
        g_pLevelBigFile = 0;
    }
    if (g_pCompartmentBigFileHeader) {
        MEM_free(g_pCompartmentBigFileHeader);
        g_pCompartmentBigFileHeader = 0;
    }
    if (g_CompartmentBigFile) {
        FILESYS_delbigsync(g_CompartmentBigFile, 100);
        g_CompartmentBigFile = 0;
    }
    g_pLevelFileContentsArray = 0;
    g_pLevelFileContentsEndMarker = 0;
    g_pNumLevelFileContents = 0;
    g_LevelBigFileSize = 0;
}
