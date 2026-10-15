// CAnimDatabase: the loaded animation files in two object-type groups (up to
// 20 files each), with state lookups forwarded to the files, and the ARAM
// pool that holds the animation frames. The group record and the inline group
// search are inferred; CAnimDatabase and the parameter types are named by the
// mangled symbols.
extern "C" {
void* memset(void*, int, unsigned long);
int ARCheckInit(void);
unsigned long ARInit(unsigned long*, unsigned long);
void ARQInit(void);
void* ARAM_NEW_poolmanager(int, int, int, const char*);
void* ARAM_NEW_pool(void*, int, int, const char*);
}

struct AnimFileFormat_t {
    unsigned char field00[10];
    unsigned short filenum;
    unsigned char field0C[12];
    unsigned char** frames;
};

struct AnimTranTransListArray_t {
    unsigned short count;
    unsigned short max;
    unsigned short state;
};

struct AnimFileStateAnimList_t {
    unsigned short count;
    unsigned short max;
};

class CAnimDebugFile;

extern "C" {
void AnimFileGetStateTransList(AnimFileFormat_t*, unsigned long, AnimTranTransListArray_t*);
void AnimFileGetStateAnimList(AnimFileFormat_t*, unsigned long, AnimFileStateAnimList_t*);
void* AnimFileGetStateInfo(AnimFileFormat_t*, unsigned long);
int AnimFileGetStateCallbackID(AnimFileFormat_t*, unsigned long);
unsigned short AnimFileGetObjectType(AnimFileFormat_t*);
}
AnimFileFormat_t* CopyAnimFileFormatStructureToDynMem(AnimFileFormat_t*);
void UploadFramesToARAM(AnimFileFormat_t*, unsigned char*, unsigned char*);

void* g_ARAM_poolman;
void* g_ARAM_pool;
unsigned long g_ARAM_stack[2];

struct AnimDatabaseGroup {
    unsigned short objecttype;
    unsigned long count;
    AnimFileFormat_t* files[20];
};

class CAnimDatabase {
public:
    CAnimDatabase();
    ~CAnimDatabase();
    void Init();
    void AddFile(AnimFileFormat_t*, CAnimDebugFile*);
    int GetStateCallbackID(unsigned long, unsigned short);
    void* GetStateInfo(unsigned long, unsigned short);
    void GetStateAnimList(unsigned long, unsigned long, AnimFileStateAnimList_t*);
    void GetTransList(unsigned long, unsigned long, AnimTranTransListArray_t*);
    AnimFileFormat_t* GetFileFromFileNum(unsigned long, unsigned short);

private:
    AnimDatabaseGroup* FindGroup(unsigned long objecttype) {
        int i;
        AnimDatabaseGroup* group = 0;

        for (i = 0; i < 2; i++) {
            if (objecttype == m_groups[i].objecttype) {
                group = &m_groups[i];
                break;
            }
        }
        return group;
    }

    AnimDatabaseGroup m_groups[2];
};

AnimFileFormat_t* CAnimDatabase::GetFileFromFileNum(unsigned long objecttype, unsigned short filenum) {
    AnimDatabaseGroup* group = FindGroup(objecttype);
    if (!group)
        return 0;
    for (unsigned long i = 0; i < group->count; i++) {
        if (group->files[i] && group->files[i]->filenum == filenum)
            return group->files[i];
    }
    return 0;
}

void CAnimDatabase::GetTransList(unsigned long objecttype, unsigned long state, AnimTranTransListArray_t* list) {
    list->count = 0;
    list->max = 8;
    list->state = state;
    AnimDatabaseGroup* group = FindGroup(objecttype);

    if (group) {
        for (unsigned long i = 0; i < group->count; i++) {
            if (group->files[i])
                AnimFileGetStateTransList(group->files[i], state, list);
        }
    }
}

void CAnimDatabase::GetStateAnimList(unsigned long objecttype, unsigned long state, AnimFileStateAnimList_t* list) {
    list->count = 0;
    list->max = 250;
    AnimDatabaseGroup* group = FindGroup(objecttype);

    if (group) {
        for (unsigned long i = 0; i < group->count; i++) {
            if (group->files[i])
                AnimFileGetStateAnimList(group->files[i], state, list);
        }
    }
}

void* CAnimDatabase::GetStateInfo(unsigned long state, unsigned short objecttype) {
    void* info = 0;
    AnimDatabaseGroup* group = FindGroup(objecttype);

    if (!group)
        return 0;
    for (unsigned long i = 0; i < group->count; i++) {
        if (group->files[i]) {
            void* found = AnimFileGetStateInfo(group->files[i], state);
            if (found)
                info = found;
        }
    }
    return info;
}

int CAnimDatabase::GetStateCallbackID(unsigned long state, unsigned short objecttype) {
    unsigned long i;
    int id = -1;
    AnimDatabaseGroup* group = FindGroup(objecttype);

    if (!group)
        return 0;
    for (i = 0; i < group->count; i++) {
        if (group->files[i]) {
            int found = AnimFileGetStateCallbackID(group->files[i], state);
            if (found != -1)
                id = found;
        }
    }
    return id;
}

void CAnimDatabase::AddFile(AnimFileFormat_t* file, CAnimDebugFile*) {
    AnimFileFormat_t* copy = CopyAnimFileFormatStructureToDynMem(file);
    unsigned long i;
    AnimDatabaseGroup* group;
    unsigned long free;
    int g;

    UploadFramesToARAM(copy, *file->frames, (unsigned char*)file->frames);
    unsigned short type = AnimFileGetObjectType(copy);
    for (g = 0; g < 2; g++) {
        if (type == m_groups[g].objecttype)
            break;
        if (m_groups[g].objecttype == 0xFFFF) {
            m_groups[g].objecttype = type;
            break;
        }
    }
    group = &m_groups[g];
    free = group->count;
    for (i = 0; i < group->count; i++) {
        if (group->files[i] == copy)
            return;
        if (!group->files[i])
            free = i;
    }
    group->files[free] = copy;
    if (free == group->count)
        group->count++;
}

void CAnimDatabase::Init() {
    for (unsigned long i = 0; i < 2; i++) {
        m_groups[i].objecttype = 0xFFFF;
        m_groups[i].count = 0;
        memset(m_groups[i].files, 0, sizeof(m_groups[i].files));
    }
    if (!ARCheckInit()) {
        ARInit(g_ARAM_stack, 2);
        ARQInit();
    }
    g_ARAM_poolman = ARAM_NEW_poolmanager(32, 10, 32, "MOHFL ARAM POOL MANAGER");
    g_ARAM_pool = ARAM_NEW_pool(g_ARAM_poolman, 0x800000, 0x800000, "Animation Pool");
}

CAnimDatabase::~CAnimDatabase() {
}

CAnimDatabase::CAnimDatabase() {
}
