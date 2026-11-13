// Behaviour-script events, bsevents.cpp: the process-state flag, the event
// memory blocks taken from g_pMemBlockAllocator, the declared-state search and
// the event processing (hash buckets moved to the execution list, the
// immediate queue); BSEventQueueImmediateEvent takes a reference on the event
// memory when the event-memory allocator owns it, and, unless the object is not
// running (its value at +16 is -1: the memory is released again and 14
// returned), fills a record from the free list (object, thread, handler,
// memory, sender, the object's value, whether the memory is owned, the event)
// and appends it to the immediate event queue, returning 0;
// BSEventTriggerEvent does the same but appends the record to the event hash
// table's bucket for the handler's byte at +6 (a head and tail pair).
// BSEventDecrimentReferenceCount drops an event memory block's reference count
// (at +32) and, when it falls to zero or below and the block is not already
// marked free (0xffff0000), marks it free and returns it to the allocator (the
// earlier functions inline it); BSEventIncrimentReferenceCount adds one to the
// count, and BSEndEvent clears the event hash table, record array and free
// list pointers. BSInitEvent takes the event memory in one block and splits it
// into a cleared 16-bucket hash table, 128 event records linked into the free
// list, and the allocator's blocks, clearing the queues and the process state;
// BSEventGetMemoryRequirements gives the allocator's size for 128 blocks of 36
// bytes plus a fixed 4224 bytes; then the static initialisation registers the
// destructor of the global event-memory allocator (its constructor is empty)
// and the weak empty destructor follows. The structure names come from the
// mangled symbols; the members and the record, object, memory and bucket views
// are inferred from offsets. The record list heads are file-local in the
// original and declared extern here. The unit is compiled with deferred
// inlining (the reference-count release is inlined into earlier functions), so
// the functions are written last to first.
class BSUtilObjectInstanceMemoryAllocator {
public:
    BSUtilObjectInstanceMemoryAllocator() {}
    ~BSUtilObjectInstanceMemoryAllocator() {}
    int ComputeNeededSize(int, int, int);
    void Init(void*, int, int, int);
    bool DoWeOwnThisMemory(void*);
    void FreeElement(void*);
    void* GetFreeElement(bool);

    unsigned char unknown00[28];
};

BSUtilObjectInstanceMemoryAllocator g_pMemBlockAllocator;

/* inferred: an event memory block, its reference count at +32 */
struct BSEventMemoryBlockView {
    unsigned char unknown00[32];
    int referenceCount;
};

struct EventMemoryBlockView {
    unsigned char unknown00[32];
    int m_references;
};

struct BSCode_EventHandlerEntry_struct {
    unsigned int code;
    unsigned short eventNumber;
    union {
        unsigned char unknown06;
        unsigned char m_bucket;
    };
    unsigned char flags;
};
struct BSStateView {
    unsigned int code;
    BSCode_EventHandlerEntry_struct* events;
    void* messages;
    unsigned short parent;
    unsigned char depth;
    unsigned char messageCount;
    unsigned char eventCount;
};

struct BSCode_struct {
    int** codeBases;
    unsigned char unknown04[4];
    BSStateView** states;
};

struct BSMachineThread_struct {
    int* frame;
    int* stackTop;
    void* context;
    int* instruction;
    BSStateView* state;
    BSCode_struct* level;
    void* messages;
    unsigned short stateId;
    unsigned short flags;
};

extern int g_eProcessState;






/* inferred: the object's value at +16 (-1 when not running) and a flag at
   +20 set by event 44 */
struct BSObject {
    unsigned char unknown00[16];
    int m_value10;
    bool m_flag14;
};

extern int* g_piStackTop;

int BSExecuteThread(BSObject*, BSMachineThread_struct*);




/* inferred: a queued event record */
struct EventRecordView {
    BSObject* m_object;
    BSMachineThread_struct* m_thread;
    EventRecordView* m_next;
    BSCode_EventHandlerEntry_struct* m_handler;
    void* m_memory;
    BSObject* m_sender;
    int m_value;
    bool m_owned;
    unsigned char unknown1d;
    unsigned short m_event;
};

extern EventRecordView* g_pFreeEventRecordHead;     // file-local in the original
extern EventRecordView* g_pImmediateEventQueue;     // file-local in the original
extern EventRecordView* g_pImmediateEventQueueTail; // file-local in the original

/* inferred: a bucket of the event hash table */
struct EventBucketView {
    EventRecordView* m_head;
    EventRecordView* m_tail;
};

extern EventBucketView* g_pEventHashTable;



extern EventRecordView* g_pEventRecordExecutionListHead;
extern EventRecordView* g_pEventRecordExecutionListTail;
extern EventRecordView* g_pCurrentProcessingEvent;
int BSEventProcessImmediateEvents();

char* BSUtilGetMemory(int, int);

// file-local in the original; declared extern here
extern char* g_pBSEventMemory;
extern int g_pBSEventMemoryOffset;
extern EventRecordView* g_pEventRecordArray;
int BSEventQueueImmediateEvent(BSObject*, BSMachineThread_struct*, BSCode_EventHandlerEntry_struct*, unsigned short, void*, BSObject*);
int BSEventTriggerEvent(BSObject*, BSMachineThread_struct*, BSCode_EventHandlerEntry_struct*, unsigned short, void*, BSObject*);
void BSEventDecrimentReferenceCount(void* memory);
void BSEventIncrimentReferenceCount(void* memory);
unsigned short BSEventFindDeclaredState(unsigned short, BSMachineThread_struct*, BSCode_EventHandlerEntry_struct**);
int BSEventProcessImmediateEvents();

inline void BSEventRun(EventRecordView* record, BSObject* object, BSMachineThread_struct* thread) {
    bool run = false;
    if (object->m_value10 == record->m_value) {
        BSCode_EventHandlerEntry_struct* entry = record->m_handler;
        unsigned short stateId = BSEventFindDeclaredState(record->m_handler->eventNumber, thread, &entry);
        if (entry) {
            if (stateId != record->m_event)
                run = (entry->flags & 2) != 0;
            else
                run = true;
        }
        if (record->m_handler->eventNumber == 44)
            object->m_flag14 = true;
        if (run) {
            unsigned int code = entry->code;
            thread->stackTop = g_piStackTop + 3;
            thread->frame = g_piStackTop + 2;
            thread->stackTop[-1] = (int)record->m_memory;
            thread->stackTop[0] = (int)record->m_sender;
            thread->instruction = thread->level->codeBases[code >> 24] + (code & 0xFFFFFF);
            BSExecuteThread(record->m_object, record->m_thread);
        }
    }
}

int BSEventGetMemoryRequirements() {
    return g_pMemBlockAllocator.ComputeNeededSize(36, 128, 0) + 4224;
}

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

int BSEndEvent() {
    g_pEventHashTable = 0;
    g_pEventRecordArray = 0;
    g_pFreeEventRecordHead = 0;
    return 0;
}

void BSEventIncrimentReferenceCount(void* memory) {
    ((EventMemoryBlockView*)memory)->m_references++;
}

void BSEventDecrimentReferenceCount(void* memory) {
    EventMemoryBlockView* block = (EventMemoryBlockView*)memory;
    block->m_references--;
    if (block->m_references <= 0 && block->m_references != (int)0xffff0000) {
        block->m_references = 0xffff0000;
        g_pMemBlockAllocator.FreeElement(block);
    }
}

int BSEventTriggerEvent(BSObject* object, BSMachineThread_struct* thread, BSCode_EventHandlerEntry_struct* handler,
                        unsigned short event, void* memory, BSObject* sender) {
    bool owned = false;
    if (g_pMemBlockAllocator.DoWeOwnThisMemory(memory))
        owned = true;
    if (owned)
        ((EventMemoryBlockView*)memory)->m_references++;
    if (object->m_value10 == -1) {
        if (owned)
            BSEventDecrimentReferenceCount(memory);
        return 14;
    }
    EventRecordView* record = g_pFreeEventRecordHead;
    g_pFreeEventRecordHead = record->m_next;
    record->m_object = object;
    record->m_thread = thread;
    record->m_next = 0;
    record->m_event = event;
    record->m_handler = handler;
    record->m_memory = memory;
    record->m_sender = sender;
    record->m_value = object->m_value10;
    record->m_owned = owned;
    int bucket = handler->m_bucket;
    if (g_pEventHashTable[bucket].m_tail)
        g_pEventHashTable[bucket].m_tail->m_next = record;
    g_pEventHashTable[bucket].m_tail = record;
    if (!g_pEventHashTable[bucket].m_head)
        g_pEventHashTable[bucket].m_head = record;
    return 0;
}

int BSEventQueueImmediateEvent(BSObject* object, BSMachineThread_struct* thread,
                               BSCode_EventHandlerEntry_struct* handler, unsigned short event, void* memory,
                               BSObject* sender) {
    bool owned = false;
    if (g_pMemBlockAllocator.DoWeOwnThisMemory(memory))
        owned = true;
    if (owned)
        ((EventMemoryBlockView*)memory)->m_references++;
    if (object->m_value10 == -1) {
        if (owned)
            BSEventDecrimentReferenceCount(memory);
        return 14;
    }
    EventRecordView* record = g_pFreeEventRecordHead;
    g_pFreeEventRecordHead = record->m_next;
    record->m_object = object;
    record->m_thread = thread;
    record->m_next = 0;
    record->m_event = event;
    record->m_handler = handler;
    record->m_memory = memory;
    record->m_sender = sender;
    record->m_value = object->m_value10;
    record->m_owned = owned;
    if (g_pImmediateEventQueue) {
        g_pImmediateEventQueueTail->m_next = record;
        g_pImmediateEventQueueTail = record;
    } else {
        g_pImmediateEventQueue = record;
        g_pImmediateEventQueueTail = record;
    }
    return 0;
}

int BSEventProcessImmediateEvents() {
    if (g_eProcessState == 0) {
        g_eProcessState = 2;
        EventRecordView* record = g_pImmediateEventQueue;
        EventRecordView* last = 0;
        while (record) {
            BSMachineThread_struct* thread = record->m_thread;
            BSObject* object = record->m_object;
            BSEventRun(record, object, thread);
            if (record->m_owned)
                BSEventDecrimentReferenceCount(record->m_memory);
            last = record;
            record = record->m_next;
        }
        if (g_pImmediateEventQueue) {
            last->m_next = g_pFreeEventRecordHead;
            g_pFreeEventRecordHead = g_pImmediateEventQueue;
        }
        g_pImmediateEventQueue = 0;
        g_pImmediateEventQueueTail = 0;
        g_eProcessState = 0;
    }
    return 0;
}

int BSEventProcessEvents(int maxEvents) {
    int i;
    bool started = false;
    EventRecordView* tail = 0;
    g_eProcessState = 1;
    for (i = 0; i < 16; i++) {
        if (g_pEventHashTable[i].m_head) {
            if (started) {
                tail->m_next = g_pEventHashTable[i].m_head;
                g_pEventRecordExecutionListTail = g_pEventHashTable[i].m_tail;
                tail = g_pEventHashTable[i].m_tail;
            } else if (!started) {
                if (g_pEventRecordExecutionListTail)
                    g_pEventRecordExecutionListTail->m_next = g_pEventHashTable[i].m_head;
                else
                    g_pEventRecordExecutionListHead = g_pEventHashTable[i].m_head;
                started = true;
                tail = g_pEventHashTable[i].m_tail;
            }
        }
        g_pEventHashTable[i].m_head = 0;
        g_pEventHashTable[i].m_tail = 0;
    }
    int count = 0;
    EventRecordView* last = 0;
    EventRecordView* record = g_pEventRecordExecutionListHead;
    while (record && count < maxEvents) {
        g_pCurrentProcessingEvent = record;
        BSMachineThread_struct* thread = record->m_thread;
        BSObject* object = record->m_object;
        BSEventRun(record, object, thread);
        if (record->m_owned)
            BSEventDecrimentReferenceCount(record->m_memory);
        last = record;
        record = record->m_next;
        count++;
        g_eProcessState = 0;
        BSEventProcessImmediateEvents();
        g_eProcessState = 1;
    }
    if (last) {
        last->m_next = g_pFreeEventRecordHead;
        g_pFreeEventRecordHead = g_pEventRecordExecutionListHead;
    }
    g_pEventRecordExecutionListHead = record;
    if (!record)
        g_pEventRecordExecutionListTail = 0;
    g_eProcessState = 0;
    return 0;
}

unsigned short BSEventFindDeclaredState(unsigned short eventNumber, BSMachineThread_struct* thread,
                                        BSCode_EventHandlerEntry_struct** handler) {
    BSStateView* state;
    BSStateView** states = thread->level->states;
    unsigned short stateId = thread->stateId;

    while (stateId != 0xFFFF) {
        state = states[stateId];
        BSCode_EventHandlerEntry_struct* entry = state->events;

        for (int i = 0; i < state->eventCount; i++, entry++) {
            if (entry->flags & 4) {
                *handler = 0;
                return 0xFFFF;
            }
            if (entry->eventNumber == eventNumber) {
                if (entry->flags & 1) {
                    *handler = entry;
                    return stateId;
                }
                *handler = 0;
                return 0xFFFF;
            }
            if (entry->eventNumber > eventNumber)
                break;
        }
        stateId = states[stateId]->parent;
    }
    *handler = 0;
    return 0xFFFF;
}

void* BSEventGetEventMemoryBlock(unsigned int) {
    BSEventMemoryBlockView* block = (BSEventMemoryBlockView*)g_pMemBlockAllocator.GetFreeElement(true);
    block->referenceCount = 0;
    return block;
}

void BSEventCheckMemBlockReferenceCount(void* block) {
    BSEventMemoryBlockView* view = (BSEventMemoryBlockView*)block;

    if (view->referenceCount <= 0 && view->referenceCount != (int)0xFFFF0000) {
        view->referenceCount = 0xFFFF0000;
        g_pMemBlockAllocator.FreeElement(block);
    }
}

bool BSEventIsEventMemoryBlock(void* block) {
    return g_pMemBlockAllocator.DoWeOwnThisMemory(block);
}


void BSEventCurrentlyProcessingMessages(bool processing) {
    int state = 0;
    if (processing)
        state = 3;
    g_eProcessState = state;
}
