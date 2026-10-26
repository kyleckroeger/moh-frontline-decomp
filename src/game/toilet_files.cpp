// A fragment of toilet.cpp; the file name is this project's. The level-file
// layer of toilet.cpp, the functions from TLT_LoadFileNormal to
// TLT_LoadShellBriefingLogBigFile. TLT_LoadFileNormal drops anything up to a
// device colon, fetches the size when asked and loads the file with the
// given flags; TLT_InitResourceType calls a function for every level-contents
// entry of a type. GetCompartmentSizeFromIndex, GetCompartmentCPTNameFromIndex
// and GetCompartmentCDBNameFromIndex inline the value search for type 0 with
// the doubled index (plus one for the CPT entry) in the second data word; the
// first data word is the entry's name: the name functions print it into the
// buffer (empty when there is none; their third argument is unused), the size
// function adds the two entries' sizes in the compartment big file (-1 when
// anything is missing). Closing a file frees it unless it lies inside the
// level or shell-briefing big file (files loaded from a big file point into
// it); the level-contents table is searched for the next resource of a type
// (optionally with a value in its data; TLT_FindNextResourceByTypeAndVal
// ignores its start argument, searches from the beginning and inlines
// TLT_FindNextResourceByType); the shell-briefing big file is unloaded, and
// loaded (its name built for the mission and stage; TLT_LoadFileNormal is
// inlined). The big-file loaders that follow in the image (shell-briefing
// log, slides, shell) drop anything up to a device colon with a DEBUG
// warning, then look the name (without its directories) up in their big
// file, or (slides) mount the slides big file around a sized load. Their
// colon test is written with an else branch that re-assigns the name and a
// separate increment ('n = p; n++;'): the original keeps that empty block
// ('bne; b') and computes the stripped name before the warning call. The
// formats are pool entries. The file is compiled with -inline
// deferred,auto: functions are emitted in reverse source order, which is why
// functions inlined into earlier-emitted ones come later in the source. The
// function, type and global names come from the symbols; the 12-byte contents
// entry and the resource data view (a count, then 8-byte entries) are
// inferred. The file's globals are defined elsewhere (the file-local ones are
// referenced as local externals).
extern "C" void MEM_free(void*);

enum TLTResourceID {};

struct TLTResourceDataView {
    int count;
    struct {
        int value;
        int unknown04;
    } entries[1];
};

struct LevelFileContentsStruct_ {
    TLTResourceID type;
    int unknown04;
    TLTResourceDataView* data;
};

extern char* g_pLevelBigFile;
extern char* g_pShellBriefingLogBigFile;
extern int g_LevelBigFileSize;
extern int g_ShellBriefingLogBigFileSize;
extern void* g_pCompartmentBigFileHeader;
extern void* g_CompartmentBigFile;
extern LevelFileContentsStruct_* g_pLevelFileContentsArray;
extern int* g_pNumLevelFileContents;
extern LevelFileContentsStruct_* g_pLevelFileContentsEndMarker;

extern "C" {
int sprintf(char*, const char*, ...);
char* strchr(const char*, int);
int FILE_sizez(const char*);
char* FILE_loadz(const char*, int);
void* BIG_locateentryz(void*, const char*, int, int*, int*);
char* BIG_locate(void*, const char*);
void FILESYS_addbigsync(const char*, int, int, int*);
void FILESYS_delbigsync(int, int);
void* FILE_loadbigheader(const char*, int);
char* FILE_loadsize(const char*, int*, int);
}

void DebugMsg(const char*, ...);

inline char* LoadFromBig(char* big, const char* name, int* size) {
    const char* q = strchr(name, '\\');
    while (q) {
        name = q + 1;
        q = strchr(name, '\\');
    }
    int offset;
    if (big) {
        if (BIG_locateentryz(big, name, 0, &offset, size))
            return BIG_locate(big, name);
    }
    return 0;
}
extern char* szTLT_SlidesBigFileName;
char* TLT_LoadFileNormal(const char*, int*, int);

char* TLT_LoadFileFromShellBigFile(const char* name, int* size) {
    const char* p = strchr(name, ':');
    {
        const char* n = name;
        if (!p) {
            n = name;
        } else {
            n = p;
            n++;
            DebugMsg("WARNING: When loading from a big file, strip off the %s part.\nSince it's a DEBUG build, I'll do it for you\n", "");
        }
        name = n;
    }
    return LoadFromBig(g_pLevelBigFile, name, size);
}

char* TLT_LoadFileFromSlidesBigFile(const char* name, int* size) {
    char path[140];
    int handle;
    const char* p = strchr(name, ':');
    {
        const char* n = name;
        if (!p) {
            n = name;
        } else {
            n = p;
            n++;
            DebugMsg("WARNING: When loading from a big file, strip off the %s part.\nSince it's a DEBUG build, I'll do it for you\n", "");
        }
        name = n;
    }
    sprintf(path, "data/shell/%s", szTLT_SlidesBigFileName);
    FILESYS_addbigsync(path, 1024, 100, &handle);
    g_pCompartmentBigFileHeader = FILE_loadbigheader(path, 1024);
    char* file = FILE_loadsize(name, size, 1024);
    FILESYS_delbigsync(handle, 100);
    MEM_free(g_pCompartmentBigFileHeader);
    g_pCompartmentBigFileHeader = 0;
    return file;
}

char* TLT_LoadFileFromShellBriefingLogBigFile(const char* name, int* size) {
    const char* p = strchr(name, ':');
    {
        const char* n = name;
        if (!p) {
            n = name;
        } else {
            n = p;
            n++;
            DebugMsg("WARNING: When loading from a big file, strip off the %s part.\nSince it's a DEBUG build, I'll do it for you\n", "");
        }
        name = n;
    }
    return LoadFromBig(g_pShellBriefingLogBigFile, name, size);
}

void TLT_LoadShellBriefingLogBigFile(int mission, int stage) {
    char name[128];
    sprintf(name, "data/shell/%d_%d_blog.viv", mission, stage);
    g_pShellBriefingLogBigFile = TLT_LoadFileNormal(name, &g_ShellBriefingLogBigFileSize, 0);
}

void TLT_UnLoadShellBriefingLogBigFile() {
    if (g_pShellBriefingLogBigFile) {
        MEM_free(g_pShellBriefingLogBigFile);
        g_pShellBriefingLogBigFile = 0;
        g_ShellBriefingLogBigFileSize = 0;
    }
}

LevelFileContentsStruct_* TLT_FindNextResourceByType(TLTResourceID type, LevelFileContentsStruct_* previous) {
    if (previous >= g_pLevelFileContentsEndMarker)
        return 0;
    LevelFileContentsStruct_* entry = previous ? previous + 1 : g_pLevelFileContentsArray;
    for (; entry < g_pLevelFileContentsEndMarker; entry++) {
        if (entry->type == type)
            return entry;
    }
    return 0;
}

LevelFileContentsStruct_* TLT_FindNextResourceByTypeAndVal(TLTResourceID type, int index, int value, LevelFileContentsStruct_*) {
    LevelFileContentsStruct_* entry = 0;
    for (;;) {
        entry = TLT_FindNextResourceByType(type, entry);
        if (!entry)
            break;
        if (value == entry->data->entries[index].value)
            return entry;
    }
    return 0;
}

void TLT_CloseFile(void* file) {
    if ((char*)file >= g_pLevelBigFile && (char*)file < g_pLevelBigFile + g_LevelBigFileSize)
        return;
    if ((char*)file >= g_pShellBriefingLogBigFile && (char*)file < g_pShellBriefingLogBigFile + g_ShellBriefingLogBigFileSize)
        return;
    MEM_free(file);
}



void GetCompartmentCDBNameFromIndex(int index, char* name, int) {
    LevelFileContentsStruct_* entry = TLT_FindNextResourceByTypeAndVal((TLTResourceID)0, 1, index * 2, 0);
    if (!entry)
        name[0] = 0;
    else
        sprintf(name, "%s", (char*)entry->data->entries[0].value);
}

void GetCompartmentCPTNameFromIndex(int index, char* name, int) {
    LevelFileContentsStruct_* entry = TLT_FindNextResourceByTypeAndVal((TLTResourceID)0, 1, index * 2 + 1, 0);
    if (!entry)
        name[0] = 0;
    else
        sprintf(name, "%s", (char*)entry->data->entries[0].value);
}

int GetCompartmentSizeFromIndex(int index) {
    if (!g_pCompartmentBigFileHeader)
        return -1;
    LevelFileContentsStruct_* cdb = TLT_FindNextResourceByTypeAndVal((TLTResourceID)0, 1, index * 2, 0);
    if (!cdb)
        return -1;
    char* cdbName = (char*)cdb->data->entries[0].value;
    LevelFileContentsStruct_* cpt = TLT_FindNextResourceByTypeAndVal((TLTResourceID)0, 1, index * 2 + 1, 0);
    if (!cpt)
        return -1;
    char* cptName = (char*)cpt->data->entries[0].value;
    int offset;
    int cdbSize;
    int cptSize;
    if (!BIG_locateentryz(g_pCompartmentBigFileHeader, cdbName, 0, &offset, &cdbSize))
        return -1;
    if (!BIG_locateentryz(g_pCompartmentBigFileHeader, cptName, 0, &offset, &cptSize))
        return -1;
    return cdbSize + cptSize;
}

void TLT_InitResourceType(void (*init)(LevelFileContentsStruct_*), TLTResourceID type) {
    for (LevelFileContentsStruct_* entry = TLT_FindNextResourceByType(type, 0); entry;
         entry = TLT_FindNextResourceByType(type, entry))
        init(entry);
}

char* TLT_LoadFileNormal(const char* name, int* size, int flags) {
    char path[264];
    const char* file = strchr(name, ':');
    if (file == 0)
        file = name;
    else
        file = file + 1;
    sprintf(path, "%s", file);
    if (size)
        *size = FILE_sizez(path);
    return FILE_loadz(path, flags | 1024);
}
