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
// log, slides, shell, level) drop anything up to a device colon with a DEBUG
// warning, then look the name (without its directories) up in their big
// file, or (slides) mount the slides big file around a sized load; the level
// loader falls back to TLT_LoadFileNormal (inlined; its path buffer is 260
// bytes, which the level loader's frame shows). Their
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

// Header-defined hash-table primes; every unit that includes the header carries a copy.
namespace dwi {
static const unsigned long dwi_prime_list[27] = {
    53,        97,        193,       389,       769,       1543,      3079,      6151,      12289,
    24593,     49157,     98317,     196613,    393241,    786433,    1572869,   3145739,   6291469,
    12582917,  25165843,  50331653,  100663319, 201326611, 402653189, 805306457, 1610612741, 3221225473u,
};
}

static char* szTLT_LevelContentsFileName = "levelfile.lfc";
static char* szTLT_LevelBigFileName = "level.viv";
static char* szTLT_CompartmentBigFileName = "comp.viv";
static char* szTLT_SlidesBigFileName = "slides.viv";

extern char* g_pLevelBigFile;
extern char* g_pShellBriefingLogBigFile;
extern int g_LevelBigFileSize;
extern int g_ShellBriefingLogBigFileSize;
extern void* g_pCompartmentBigFileHeader;
extern int g_CompartmentBigFile;
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
char* TLT_LoadFileNormal(const char*, int*, int);

void* DWI_allocalign(const char*, int, int, int);
void TLT_LoadLevelContentsFile(int, int);
char* TLT_LoadFileNormal(const char* name, int* size, int flags);
extern "C" int BIG_count(void*);
namespace dwi {
template <class K, class V>
class hash_map {
public:
    struct value_type {
        K first;
        V second;
    };

    hash_map() : m_data(0), m_size(0), m_count(0) {}
    ~hash_map();

    static int NextPrime(int n) {
        const unsigned long* p = dwi_prime_list;
        const unsigned long* end = dwi_prime_list + 27;
        for (; p != end; p++) {
            if ((int)*p >= n)
                return *p;
        }
        return *(end - 1);
    }

    void clear() {
        for (value_type* it = m_data; it != m_data + m_size; ++it)
            it->first = (K)-1;
        m_data[-1].first = (K)-2;
        m_data[m_size].first = (K)-2;
        m_count = 0;
    }

    void Resize(int count) {
        int size = NextPrime(count + 1);
        m_size = size;
        m_data = (value_type*)DWI_allocalign(0, (size + 2) * sizeof(value_type), 16, 1024) + 1;
        clear();
    }

    value_type* m_data;
    int m_size;
    int m_count;
};
}

static dwi::hash_map<char*, char*> g_Repository;



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

char* TLT_LoadFileFromLevelBigFile(const char* name, int* size) {
    const char* n;
    const char* p = strchr(name, ':');
    if (!p) {
        n = name;
    } else {
        n = p;
        n++;
        DebugMsg("WARNING: When loading from a big file, strip off the %s part.\nSince it's a DEBUG build, I'll do it for you\n", "");
    }
    char* file = LoadFromBig(g_pLevelBigFile, n, size);
    if (!file)
        file = TLT_LoadFileNormal(n, size, 0);
    return file;
}

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
    char path[256];
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

extern "C" char* strcpy(char*, const char*);

inline void ChangeEndian(int& value) {
    unsigned char bytes[4];
    *reinterpret_cast<int*>(bytes) = value;
    unsigned char c = bytes[0];
    bytes[0] = bytes[3];
    bytes[3] = c;
    c = bytes[1];
    bytes[1] = bytes[2];
    bytes[2] = c;
    value = *reinterpret_cast<int*>(bytes);
}

struct TLTPairView {
    int type;
    unsigned int value;
};

void TLT_LoadLevelContentsFile(int, int) {
    char name[256];
    int size;
    strcpy(name, szTLT_LevelContentsFileName);
    int* header = (int*)TLT_LoadFileFromLevelBigFile(name, &size);
    ChangeEndian(header[0]);
    ChangeEndian(header[1]);
    ChangeEndian(header[2]);
    ChangeEndian(header[3]);
    if (header[0] != 1) {
        DebugMsg("Error: version check failed on level contents file\n");
        DebugMsg("       Engine is version %d.  File is version %d.\n", 1, header[0]);
    }
    if (header[1] != 0x858cd) {
        DebugMsg("Error: checksum failed on level contents file\n");
        DebugMsg("       Engine checksum = %d.  File checksum = %d.\n", 0x858cd, header[1]);
        DebugMsg("Use Toilet to generate a new tltheader.h for the engine or get a new definition.crd and reprocess your level\n");
    }
    header[3] = (int)header + header[3];
    g_pLevelFileContentsArray = (LevelFileContentsStruct_*)header[3];
    g_pNumLevelFileContents = (int*)header[2];
    for (int i = 0; i < header[2]; i++) {
        LevelFileContentsStruct_* entry = &g_pLevelFileContentsArray[i];
        ChangeEndian(*(int*)&entry->type);
        ChangeEndian(entry->unknown04);
        ChangeEndian(*(int*)&entry->data);
        entry->data = (TLTResourceDataView*)((int)header + (int)entry->data);
        for (int j = 0; j < entry->unknown04; j++) {
            ChangeEndian(((TLTPairView*)entry->data)[j].type);
            ChangeEndian(*(int*)&((TLTPairView*)entry->data)[j].value);
            TLTPairView* pair = &((TLTPairView*)entry->data)[j];
            if ((pair->type == 0 || pair->type == 4) && pair->value)
                pair->value = (unsigned int)header + pair->value;
        }
    }
    g_pLevelFileContentsEndMarker = g_pLevelFileContentsArray + header[2];
}

void TLT_Init(int mission, int stage) {
    char name[128];
    char compartment[128];
    if (g_pLevelBigFile)
        MEM_free(g_pLevelBigFile);
    sprintf(name, "data/%d/%d_%d/%s", mission, mission, stage, szTLT_LevelBigFileName);
    g_pLevelBigFile = TLT_LoadFileNormal(name, &g_LevelBigFileSize, 0);
    TLT_LoadLevelContentsFile(mission, stage);
    if (g_CompartmentBigFile)
        FILESYS_delbigsync(g_CompartmentBigFile, 100);
    sprintf(compartment, "data/%d/%d_%d/%s", mission, mission, stage, szTLT_CompartmentBigFileName);
    FILESYS_addbigsync(compartment, 1024, 100, &g_CompartmentBigFile);
    g_pCompartmentBigFileHeader = FILE_loadbigheader(compartment, 1024);
    g_Repository.Resize(BIG_count(g_pLevelBigFile));
}
