// A fragment of IStudioFuncs.cpp (0x8008bbe4): ResUnloadScreen closes a
// loaded screen's file data when there is any, and ResLoadScreen loads one
// resolution's file of a shell screen from the shell big file (the screen
// table's records are 8 bytes, their file-name lists at +4), returning the
// loaded data or none. The function and ScreenInfo are named by the symbols;
// the record view is inferred. The rest of the file is in other units.
void* TLT_LoadFileFromShellBigFile(const char*, int*);
void TLT_CloseFile(void*);

/* inferred: a shell screen record, its file names per resolution at +4 */
struct ScreenInfoView {
    int unknown00;
    char** files;
};

extern ScreenInfoView* ScreenInfo;

void ResUnloadScreen(unsigned short, unsigned short, void* data) {
    if (data)
        TLT_CloseFile(data);
}

void* ResLoadScreen(unsigned short screen, unsigned short resolution) {
    void* data = TLT_LoadFileFromShellBigFile(ScreenInfo[screen].files[resolution], 0);
    if (data)
        return data;
    return 0;
}
