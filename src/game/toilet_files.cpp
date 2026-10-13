// A fragment of toilet.cpp; the file name is this project's. The level-file layer of toilet.cpp, the functions from TLT_CloseFile to
// TLT_LoadShellBriefingLogBigFile: closing a file frees it unless it lies
// inside the level or shell-briefing big file (files loaded from a big file
// point into it), the level-contents table is searched for the next resource
// of a type (optionally with a value in its data;
// TLT_FindNextResourceByTypeAndVal ignores its start argument, searches from
// the beginning and inlines TLT_FindNextResourceByType), the shell-briefing
// big file is unloaded, and loaded (its name built for the mission and stage,
// anything up to a device colon dropped, through an inferred inline load
// helper that fetches the size first; the formats are pool entries). The file is compiled with -inline
// deferred,auto: functions are emitted in reverse source order, which is why
// the search that is inlined comes first in the source and last but one in
// the image. The function, type and global names come from the symbols; the
// 12-byte contents entry and the resource data view (a count, then 8-byte
// entries) are inferred. The file's globals are defined elsewhere (the
// file-local ones are referenced as local externals).
// GetCompartmentCDBNameFromIndex, GetCompartmentCPTNameFromIndex and
// GetCompartmentSizeFromIndex (the two entries' sizes in the compartment big
// file added, -1 when anything is missing) come last in the source (first in
// the image): they inline the value search for type 0
// with the doubled index (plus one for the CPT name) in the second data word and prints the first data word, a name, into the buffer
// (empty when there is none; its third argument is unused).
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
}

/* inferred: the load helper (the optional size is fetched first). */
inline char* TLT_LoadWholeFile(const char* name, int* size) {
    if (size)
        *size = FILE_sizez(name);
    return FILE_loadz(name, 1024);
}

void TLT_LoadShellBriefingLogBigFile(int mission, int stage) {
    char name[128];
    char path[264];
    sprintf(name, "data/shell/%d_%d_blog.viv", mission, stage);
    char* file = strchr(name, ':');
    if (file == 0)
        file = name;
    else
        file = file + 1;
    sprintf(path, "%s", file);
    g_pShellBriefingLogBigFile = TLT_LoadWholeFile(path, &g_ShellBriefingLogBigFileSize);
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
