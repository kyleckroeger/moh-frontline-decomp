// A fragment of bsutil.cpp (0x8003b54c): BSUtilPreLoadScriptCallback (the
// resource's name without its four-character extension, copied into a
// 64-byte buffer whose size is inferred from the frame, loads the script
// class), BSUtilComputeClassMemoryRequirements (32 bytes per state plus a
// word per state entry), BSUtilFreeObjectInstanceMemory and
// BSUtilGetObjectInstanceMemory (an instance block returned to, or taken
// from, the first of the three object allocators that owns it or whose
// elements are large enough; the allocator members are inlined here, as
// copies of the out-of-line ones in bsutil_pools.cpp, and the class size is
// read from +84 of the class record), BSUtilGetMemory (a block taken from
// the script memory arena at the running offset, first rounded up to the given
// alignment when there is one; as written, an already aligned offset still
// moves up by a whole alignment), BSUtilUpdate (one script frame: events,
// messages, built-ins, timers with the frame time capped at 6, proximity
// triggers and object creation) and BSUtilShutdownScriptingSystem (each
// script subsystem ended, then the arena freed and its globals cleared), and
// BSUtilInitScriptingSystem (the arena sized from every script subsystem's
// requirements, each rounded up to 16, plus the three object allocators'
// needs, allocated on a first init; the allocators set up in it, the
// subsystems initialised (the built-ins reset instead on a reset, which also
// keeps the class list), the script resource type registered on a first
// init, and the level's property data and BSP tree loaded). The
// functions and globals are named by the symbols; the globals' types and the
// allocator layout (as in bsutil_pools.cpp) are inferred, and g_pMemAllocators
// is file-local in the original. The file is compiled with deferred inlining (as bsutil_pools.cpp
// notes), so the functions are listed in reverse image order. The rest of the
// file is not part of this unit.
extern "C" void MEM_free(void*);

namespace dwi {
static const unsigned long dwi_prime_list[27] = {
    53,        97,        193,       389,       769,       1543,      3079,      6151,      12289,
    24593,     49157,     98317,     196613,    393241,    786433,    1572869,   3145739,   6291469,
    12582917,  25165843,  50331653,  100663319, 201326611, 402653189, 805306457, 1610612741, 3221225473u,
};
}

void BSEventProcessEvents(int);
void BSProcessMessages(int);
void BSUpdateBIFuncs();
void BSUpdateTimer(float);
void UpdateProximityTriggerStatus();
void UpdateObjectCreation();
void ShutdownObjectCreators();
void BSCleanUpBIFuncs();
void BSEndTimer();
void BSEndSchedule();
void BSEndMessage();
void BSEndEvent();
void BSEndMachine();
void FreePropertyMemory();
void DebugMsg(const char*, ...);
void* DWI_allocalign(const char*, int, int, int);
int BSEventGetMemoryRequirements();
int BSFileGetMemoryRequirements();
int BSMachineGetMemoryRequirements();
int BSMessageGetMemoryRequirements();
int BSObjectGetMemoryRequirements();
int BSScheduleGetMemoryRequirements();
int BSTimerGetMemoryRequirements();
int CreateObjectGetMemoryRequirements();
void BSInitFile();
void BSInitMachine();
void BSInitEvent();
void BSInitMessage();
void BSInitBIFuncs();
void BSResetBIFuncs();
void BSInitBSObject();
void BSInitTimer();
void BSInitSchedule();
void InitObjectCreators();
void LoadPropBSPTree(char*, bool);
void LoadPropertyData(char*, bool);

struct BSStateView {
    unsigned char unknown00[20];
    unsigned short m_count;
    unsigned char unknown16[6];
};

struct BSClass_struct {
    BSStateView* m_states;
    unsigned char unknown04[80];
    int m_instanceSize;
    unsigned short m_stateCount;
};

struct LevelFileResourceView {
    unsigned char unknown00[4];
    char* m_name;
    unsigned char unknown08[588];
    int m_messageListSize;
};

struct LevelFileContentsStruct_ {
    unsigned char unknown00[8];
    LevelFileResourceView* m_resource;
};

enum TLTResourceID {};

LevelFileContentsStruct_* TLT_FindNextResourceByType(TLTResourceID, LevelFileContentsStruct_*);
void TLT_InitResourceType(void (*)(LevelFileContentsStruct_*), TLTResourceID);
void BSUtilPreLoadScriptCallback(LevelFileContentsStruct_*);
void* BSUtilGetMemory(int, int);

extern "C" char* strcpy(char*, const char*);
extern "C" unsigned long strlen(const char*);
void BSFileGetClass(const char*, BSClass_struct**);

class BSUtilObjectInstanceMemoryAllocator {
public:
    bool DoWeOwnThisMemory(void* element) {
        int offset = (char*)element - (char*)m_base;
        bool owned = false;
        if ((unsigned long*)element >= m_base && (unsigned long*)element < m_base + m_totalWords &&
            offset % (m_strideWords * 4) == 0)
            owned = true;
        return owned;
    }
    void* GetFreeElement(bool) {
        if (m_count < m_maxCount) {
            unsigned long* element = m_free;

            m_free = (unsigned long*)element[m_elementWords];
            m_count++;
            return element;
        }
        return 0;
    }
    int ComputeNeededSize(int size, int count, int align) {
        int stride = size + 4;

        if (align && (stride & (align - 1)))
            stride += align - (stride & (align - 1));
        int total = count * stride;
        return total + (16 - (total & 15));
    }
    void Init(void* memory, int size, int count, int align) {
        m_base = (unsigned long*)memory;
        int stride = size + 4;
        m_elementWords = size >> 2;
        m_maxCount = count;
        if (align && (stride & (align - 1)))
            stride += align - (stride & (align - 1));
        m_strideWords = stride >> 2;
        m_count = 0;
        m_totalWords = ComputeNeededSize(size, count, align) >> 2;
        LinkElements(m_elementWords, m_strideWords);
        m_free = m_base;
    }
    void LinkElements(int elementWords, int strideWords) {
        unsigned long* link = m_base + elementWords;
        for (int i = 0; i < m_maxCount - 1; i++) {
            *link = (unsigned long)(link + (strideWords - elementWords));
            link += m_strideWords;
        }
        *link = 0;
    }
    void FreeElement(void* element) {
        ((unsigned long*)element)[m_elementWords] = (unsigned long)m_free;
        m_free = (unsigned long*)element;
        m_count--;
    }

    int m_elementWords;
    int m_strideWords;
    int m_maxCount;
    int m_count;
    int m_totalWords;
    unsigned long* m_base;
    unsigned long* m_free;
};

extern BSUtilObjectInstanceMemoryAllocator g_pMemAllocators[3];

extern char* g_pScriptMemory;
extern int g_memOffset;
extern int g_memSize;
extern int g_iBSMessageRegistrationListSize;
extern char* g_pcClassList;

inline int Align16(int size) {
    return size + 16 - (size & 15);
}

void BSUtilInitScriptingSystem(bool reset) {
    g_iBSMessageRegistrationListSize = TLT_FindNextResourceByType((TLTResourceID)13, 0)->m_resource->m_messageListSize;
    if (g_iBSMessageRegistrationListSize <= 0)
        g_iBSMessageRegistrationListSize = 3200;
    int size = Align16(BSEventGetMemoryRequirements());
    size += Align16(BSFileGetMemoryRequirements());
    size += Align16(BSMachineGetMemoryRequirements());
    size += Align16(BSMessageGetMemoryRequirements());
    size += Align16(BSObjectGetMemoryRequirements());
    size += Align16(BSScheduleGetMemoryRequirements());
    size += Align16(BSTimerGetMemoryRequirements());
    size += Align16(CreateObjectGetMemoryRequirements());
    size += g_pMemAllocators[0].ComputeNeededSize(144, 800, 0);
    size += g_pMemAllocators[1].ComputeNeededSize(276, 50, 0);
    size += g_pMemAllocators[2].ComputeNeededSize(760, 30, 0);
    g_memOffset = 0;
    g_memSize = size;
    if (!reset)
        g_pScriptMemory = (char*)DWI_allocalign("Script memory", size, 16, 1024);
    g_pMemAllocators[0].Init(BSUtilGetMemory(g_pMemAllocators[0].ComputeNeededSize(144, 800, 0), 0), 144, 800, 0);
    g_pMemAllocators[1].Init(BSUtilGetMemory(g_pMemAllocators[1].ComputeNeededSize(276, 50, 0), 0), 276, 50, 0);
    g_pMemAllocators[2].Init(BSUtilGetMemory(g_pMemAllocators[2].ComputeNeededSize(760, 30, 0), 0), 760, 30, 0);
    char* classList = g_pcClassList;
    DebugMsg("INIT:  BSFile\n");
    BSInitFile();
    DebugMsg("INIT:  BSMachine\n");
    BSInitMachine();
    DebugMsg("INIT:  BSEvent\n");
    BSInitEvent();
    DebugMsg("INIT:  BSMessage\n");
    BSInitMessage();
    DebugMsg("INIT:  BSBIFunc\n");
    if (reset)
        BSResetBIFuncs();
    else
        BSInitBIFuncs();
    DebugMsg("INIT:  BSObject\n");
    BSInitBSObject();
    DebugMsg("INIT:  BSTimer\n");
    BSInitTimer();
    DebugMsg("INIT:  BSSchedule\n");
    BSInitSchedule();
    DebugMsg("INIT:  BSObjCreate\n");
    InitObjectCreators();
    if (reset)
        g_pcClassList = classList;
    if (!reset)
        TLT_InitResourceType(BSUtilPreLoadScriptCallback, (TLTResourceID)7);
    char* properties = TLT_FindNextResourceByType((TLTResourceID)5, 0)->m_resource->m_name;
    LoadPropBSPTree(TLT_FindNextResourceByType((TLTResourceID)4, 0)->m_resource->m_name, reset);
    LoadPropertyData(properties, reset);
}

void BSUtilShutdownScriptingSystem() {
    ShutdownObjectCreators();
    BSCleanUpBIFuncs();
    BSEndTimer();
    BSEndSchedule();
    BSEndMessage();
    BSEndEvent();
    BSEndMachine();
    FreePropertyMemory();
    MEM_free(g_pScriptMemory);
    g_pScriptMemory = 0;
    g_memOffset = 0;
    g_memSize = 0;
}

void BSUtilUpdate(float time) {
    if (time > 6.0f)
        time = 6.0f;
    BSEventProcessEvents(128);
    BSProcessMessages(1024);
    BSUpdateBIFuncs();
    BSUpdateTimer(time);
    UpdateProximityTriggerStatus();
    UpdateObjectCreation();
}

void* BSUtilGetMemory(int size, int alignment) {
    int offset = g_memOffset;
    if (alignment != 0)
        offset += alignment - (offset & (alignment - 1));
    void* memory = g_pScriptMemory + offset;
    g_memOffset = offset + size;
    return memory;
}

void* BSUtilGetObjectInstanceMemory(BSClass_struct* cls) {
    int size = cls->m_instanceSize;
    for (int i = 0; i < 3; i++) {
        if (g_pMemAllocators[i].m_elementWords * 4 >= size) {
            void* element = g_pMemAllocators[i].GetFreeElement(false);
            if (element)
                return element;
        }
    }
    return 0;
}

void BSUtilFreeObjectInstanceMemory(void* element) {
    for (int i = 0; i < 3; i++) {
        if (g_pMemAllocators[i].DoWeOwnThisMemory(element)) {
            g_pMemAllocators[i].FreeElement(element);
            return;
        }
    }
}

int BSUtilComputeClassMemoryRequirements(BSClass_struct* cls) {
    int size = 0;
    for (int i = 0; i < cls->m_stateCount; i++) {
        size += 32;
        size += cls->m_states[i].m_count * 4;
    }
    return size;
}

void BSUtilPreLoadScriptCallback(LevelFileContentsStruct_* contents) {
    char name[64];
    BSClass_struct* cls;
    strcpy(name, contents->m_resource->m_name);
    name[strlen(name) - 4] = 0;
    BSFileGetClass(name, &cls);
}
