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
// script subsystem ended, then the arena freed and its globals cleared). The
// functions and globals are named by the symbols; the globals' types and the
// allocator layout (as in bsutil_pools.cpp) are inferred, and g_pMemAllocators
// is file-local in the original. The file is compiled with deferred inlining (as bsutil_pools.cpp
// notes), so the functions are listed in reverse image order. The rest of the
// file is not part of this unit.
extern "C" void MEM_free(void*);

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
    const char* m_name;
};

struct LevelFileContentsStruct_ {
    unsigned char unknown00[8];
    LevelFileResourceView* m_resource;
};

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
    g_memOffset = offset + size;
    return g_pScriptMemory + offset;
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
