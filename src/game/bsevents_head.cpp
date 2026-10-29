// A fragment of bsevents.cpp; the file name is this project's. Behaviour-script events: the functions at the start of the file, the
// process-state flag, the event memory blocks taken from g_pMemBlockAllocator, the declared-state search and the event
// processing (hash buckets moved to the execution list, the immediate queue). The structure names come from the mangled
// symbols; the members and the record views are inferred from offsets.
class BSUtilObjectInstanceMemoryAllocator {
public:
    bool DoWeOwnThisMemory(void*);
    void FreeElement(void*);
    void* GetFreeElement(bool);

private:
    unsigned char unknown00[28];
};

struct BSEventMemoryBlockView {
    unsigned char unknown00[32];
    int referenceCount;
};

struct BSCode_EventHandlerEntry_struct {
    unsigned int code;
    unsigned short eventNumber;
    unsigned char unknown06;
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

extern BSUtilObjectInstanceMemoryAllocator g_pMemBlockAllocator;
extern int g_eProcessState;

void BSEventCurrentlyProcessingMessages(bool processing) {
    int state = 0;
    if (processing)
        state = 3;
    g_eProcessState = state;
}

bool BSEventIsEventMemoryBlock(void* block) {
    return g_pMemBlockAllocator.DoWeOwnThisMemory(block);
}

void BSEventCheckMemBlockReferenceCount(void* block) {
    BSEventMemoryBlockView* view = (BSEventMemoryBlockView*)block;

    if (view->referenceCount <= 0 && view->referenceCount != (int)0xFFFF0000) {
        view->referenceCount = 0xFFFF0000;
        g_pMemBlockAllocator.FreeElement(block);
    }
}

void* BSEventGetEventMemoryBlock(unsigned int) {
    BSEventMemoryBlockView* block = (BSEventMemoryBlockView*)g_pMemBlockAllocator.GetFreeElement(true);
    block->referenceCount = 0;
    return block;
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


inline void BSEventDecrimentReferenceCount(void* memory) {
    BSEventMemoryBlockView* block = (BSEventMemoryBlockView*)memory;
    block->referenceCount--;
    if (block->referenceCount <= 0 && block->referenceCount != (int)0xffff0000) {
        block->referenceCount = 0xffff0000;
        g_pMemBlockAllocator.FreeElement(block);
    }
}

extern EventRecordView* g_pEventRecordExecutionListHead;
extern EventRecordView* g_pEventRecordExecutionListTail;
extern EventRecordView* g_pCurrentProcessingEvent;
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
