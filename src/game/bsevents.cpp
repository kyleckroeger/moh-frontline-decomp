// The tail of bsevents.cpp (0x80035734): BSInitEvent (the event memory is
// taken in one block and split into a cleared 16-bucket hash table, 128
// event records linked into the free list, and the allocator's blocks; the
// queues and the process state are cleared; the record and memory views are
// inferred), BSEventGetMemoryRequirements (the
// allocator's size for 128 blocks of 36 bytes plus a fixed 4224 bytes), then
// the static initialisation (0x80035a34). The static initialisation registers the
// destructor of the global event-memory allocator, whose constructor is
// empty; the weak empty destructor follows (emitted here for the
// registration). The rest of the file is in other units. g_pMemBlockAllocator and
// BSUtilObjectInstanceMemoryAllocator are named by the symbols; the object
// size comes from the symbol.
class BSUtilObjectInstanceMemoryAllocator {
public:
    BSUtilObjectInstanceMemoryAllocator() {}
    ~BSUtilObjectInstanceMemoryAllocator() {}
    int ComputeNeededSize(int, int, int);
    void Init(void*, int, int, int);

    unsigned char unknown00[28];
};

BSUtilObjectInstanceMemoryAllocator g_pMemBlockAllocator;

/* inferred: a 32-byte event record, its link at +8 */
struct EventRecordView {
    unsigned char unknown00[8];
    EventRecordView* m_next;
    unsigned char unknown0c[20];
};

char* BSUtilGetMemory(int, int);

// file-local in the original; declared extern here
extern char* g_pBSEventMemory;
extern int g_pBSEventMemoryOffset;
struct EventBucketView {
    EventRecordView* m_head;
    EventRecordView* m_tail;
};

extern EventBucketView* g_pEventHashTable;
extern EventRecordView* g_pEventRecordArray;
extern EventRecordView* g_pFreeEventRecordHead;
extern EventRecordView* g_pEventRecordExecutionListHead;
extern EventRecordView* g_pEventRecordExecutionListTail;
extern EventRecordView* g_pImmediateEventQueue;
extern int g_eProcessState;

int BSInitEvent() {
    g_pBSEventMemory = BSUtilGetMemory(g_pMemBlockAllocator.ComputeNeededSize(36, 128, 0) + 4224, 0);
    g_pBSEventMemoryOffset = 0;
    g_pEventHashTable = (EventBucketView*)(g_pBSEventMemory + g_pBSEventMemoryOffset);
    g_pBSEventMemoryOffset += 128;
    int i;
    for (i = 0; i < 16; i++) {
        g_pEventHashTable[i].m_head = 0;
        g_pEventHashTable[i].m_tail = 0;
    }
    g_pEventRecordArray = (EventRecordView*)(g_pBSEventMemory + g_pBSEventMemoryOffset);
    g_pBSEventMemoryOffset += 4096;
    g_pFreeEventRecordHead = g_pEventRecordArray;
    for (i = 0; i < 127; i++)
        g_pEventRecordArray[i].m_next = &g_pEventRecordArray[i + 1];
    g_pEventRecordArray[127].m_next = 0;
    g_pEventRecordExecutionListHead = 0;
    g_pEventRecordExecutionListTail = 0;
    g_pMemBlockAllocator.Init(g_pBSEventMemory + g_pBSEventMemoryOffset, 36, 128, 0);
    g_pBSEventMemoryOffset += g_pMemBlockAllocator.ComputeNeededSize(36, 128, 0);
    g_pImmediateEventQueue = 0;
    g_eProcessState = 0;
    return 0;
}

int BSEventGetMemoryRequirements() {
    return g_pMemBlockAllocator.ComputeNeededSize(36, 128, 0) + 4224;
}
