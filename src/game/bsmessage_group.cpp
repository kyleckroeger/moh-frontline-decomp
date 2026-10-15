// A fragment of bsmessage.cpp (0x80039b84): BSMessageIsGroupCategoryMember,
// which reports whether two trigger objects share a group of the category in
// the high half of the match value: both group lists (on the object's core at
// +68, a count word followed by sorted values whose high half is the category)
// are skipped to the category and then merged; then BSProcessMessages, which
// takes up to the given number of messages from the 1024-entry message queue
// and, for each whose receiver is still the same instance (the value at +16),
// finds the first handler in the target's list matching the message's entry
// (number, value and kind; a handler flagged 4 ends the search), runs it when
// it was sent by the registered sender or its entry allows any sender (bit 1),
// pushing the four message arguments on the handler thread's stack, and then
// processes the immediate events. The functions, globals and the
// handler-entry and registration structs are named by the symbols; the core,
// list, queue, node, target, thread and object views, the members and the two
// helpers (inline, inferred) are inferred from offsets.
struct TriggerCoreView {
    unsigned char unknown00[68];
    unsigned int* groups;
};

struct TriggerObject_struct {
    unsigned char unknown00[4];
    TriggerCoreView* core;
};

int BSMessageIsGroupCategoryMember(TriggerObject_struct* first, TriggerObject_struct* second, unsigned int match) {
    unsigned int bv;
    unsigned int av;
    unsigned int* a;
    unsigned int* b;
    unsigned int category;
    unsigned int* pa;
    unsigned int* lastA;
    unsigned int* lastB;
    TriggerCoreView* firstCore;
    TriggerCoreView* secondCore;
    unsigned int* pb;

    if (!first || !second)
        return 0;
    category = match >> 16;
    firstCore = first->core;
    secondCore = second->core;
    if ((a = firstCore->groups) != 0) {
        if ((b = secondCore->groups) != 0) {
            pa = a + 1;
            lastA = a + a[0];
            pb = b + 1;
            lastB = b + b[0];
            for (; pa <= lastA; pa++) {
                if ((*pa >> 16) >= category)
                    break;
            }
            for (; pb <= lastB; pb++) {
                if ((*pb >> 16) >= category)
                    break;
            }
            while (pa <= lastA && pb <= lastB) {
                av = *pa;
                if ((av >> 16) != category)
                    break;
                bv = *pb;
                if ((bv >> 16) != category)
                    break;
                if (av < bv)
                    pa++;
                else if (av > bv)
                    pb++;
                else
                    return 1;
            }
        }
    }
    return 0;
}

struct BSStateView;
struct BSCode_struct {
    int** codeBases;
};

struct BSMachineThread_struct {
    int* frame;
    int* stackTop;
    void* context;
    int* instruction;
    BSStateView* state;
    BSCode_struct* level;
};

struct BSObject {
    unsigned char unknown00[16];
    int m_value10;
};

/* a message handler entry (also the key a message is matched against) */
struct BSCode_MessageHandlerEntry_struct {
    unsigned int code;
    unsigned int value;
    unsigned short message;
    signed char kind;
    unsigned char flags;
};

/* a registration: the object and thread that handle the entry */
struct BSMessageRegistration_struct {
    BSObject* object;
    BSMachineThread_struct* thread;
    BSCode_MessageHandlerEntry_struct* entry;
};

/* inferred: a node of the receiver's handler list */
struct MessageNodeView {
    BSMessageRegistration_struct* record;
    unsigned char unknown04[4];
    MessageNodeView* next;
    unsigned char unknown0c[2];
    unsigned short sender;
};

struct MessageTargetView {
    unsigned char unknown00[24];
    MessageNodeView* handlers;
};

/* inferred: a queued message (36 bytes) */
struct MessageView {
    int arg1;
    int arg2;
    BSObject* object;
    MessageTargetView* target;
    int arg3;
    int arg0;
    int value;
    BSCode_MessageHandlerEntry_struct* key;
    unsigned short sender;
};

extern MessageView g_MessageQueue[1024];
extern int g_MessageQueueHead;
extern int g_MessageQueueTail;
extern int* g_piStackTop;

void BSEventCurrentlyProcessingMessages(bool);
int BSEventProcessImmediateEvents();
int BSExecuteThread(BSObject*, BSMachineThread_struct*);

inline MessageNodeView* FindMessageHandler(MessageNodeView* node, const BSCode_MessageHandlerEntry_struct* key) {
    for (; node; node = node->next) {
        BSCode_MessageHandlerEntry_struct* handler = node->record->entry;
        if (handler->flags & 4)
            return 0;
        if (handler->message == key->message && handler->value == key->value && handler->kind == key->kind)
            return node;
    }
    return 0;
}

inline void StartMessageThread(BSMessageRegistration_struct* record, int arg1, int arg2, int arg3, int arg0) {
    BSMachineThread_struct* thread = record->thread;
    thread->stackTop = g_piStackTop + 2;
    thread->frame = g_piStackTop + 2;
    thread->stackTop[0] = arg0;
    thread->stackTop[1] = arg1;
    thread->stackTop[2] = arg2;
    thread->stackTop[3] = arg3;
    thread->stackTop += 3;
    thread->instruction = thread->level->codeBases[record->entry->code >> 24] + (record->entry->code & 0xFFFFFF);
    BSExecuteThread(record->object, record->thread);
}

int BSProcessMessages(int max) {
    int limit;
    int processed = 0;
    int count = g_MessageQueueTail - g_MessageQueueHead;
    if (count < 0)
        count += 1024;
    if (max < count)
        limit = max;
    else
        limit = count;
    while (processed < limit) {
        BSEventCurrentlyProcessingMessages(true);
        MessageView* message = &g_MessageQueue[g_MessageQueueHead];
        processed++;
        g_MessageQueueHead = g_MessageQueueHead + 1 >= 1024 ? 0 : g_MessageQueueHead + 1;
        if (message->value == message->object->m_value10) {
            MessageNodeView* node = FindMessageHandler(message->target->handlers, message->key);
            bool run = false;
            if (node) {
                if (node->sender != message->sender)
                    run = (node->record->entry->flags >> 1) & 1;
                else
                    run = true;
            }
            if (run)
                StartMessageThread(node->record, message->arg1, message->arg2, message->arg3, message->arg0);
        }
        BSEventCurrentlyProcessingMessages(false);
        BSEventProcessImmediateEvents();
    }
    return 0;
}
