// A fragment of bsevents.cpp (0x80035440): BSEventQueueImmediateEvent takes
// a reference on the event memory when the event-memory allocator owns it,
// and, unless the object is not running (its value at +16 is -1: the memory
// is released again and 14 returned), fills a record from the free list
// (object, thread, handler, memory, sender, the object's value, whether the
// memory is owned, the event) and appends it to the immediate event queue,
// returning 0; BSEventTriggerEvent does the same but appends the record to
// the event hash table's bucket for the handler's byte at +6 (a head and
// tail pair). The functions, structs and globals are named by the symbols;
// BSEventDecrimentReferenceCount's body follows bsevents_ref.cpp (it is
// later in the original file; this unit marks it inline so that it is
// inlined without being emitted). The record, object and memory views and
// the result codes and the bucket view are inferred; the record list heads are file-local in
// the original and declared extern here. The rest of the file is in other
// units.
struct BSMachineThread_struct;
struct BSCode_EventHandlerEntry_struct;

/* inferred: the object's value at +16 (-1 when not running) */
struct BSObject {
    unsigned char unknown00[16];
    int m_value10;
};

class BSUtilObjectInstanceMemoryAllocator {
public:
    bool DoWeOwnThisMemory(void*);
    void FreeElement(void*);

    unsigned char unknown00[28];
};

extern BSUtilObjectInstanceMemoryAllocator g_pMemBlockAllocator;

/* inferred: an event memory block, its reference count at +32 */
struct EventMemoryBlockView {
    unsigned char unknown00[32];
    int m_references;
};

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

/* inferred: the handler entry's bucket byte at +6 */
struct BSCode_EventHandlerEntry_struct {
    unsigned char unknown00[6];
    unsigned char m_bucket;
};

inline void BSEventDecrimentReferenceCount(void* memory) {
    EventMemoryBlockView* block = (EventMemoryBlockView*)memory;
    block->m_references--;
    if (block->m_references <= 0 && block->m_references != (int)0xffff0000) {
        block->m_references = 0xffff0000;
        g_pMemBlockAllocator.FreeElement(block);
    }
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
