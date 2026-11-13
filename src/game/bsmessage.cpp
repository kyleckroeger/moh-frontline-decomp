// bsmessage.cpp: BSMessageIsGroupCategoryMember reports whether two trigger
// objects share a group of the category in the high half of the match value:
// both group lists (on the object's core at +68, a count word followed by
// sorted values whose high half is the category) are skipped to the category
// and then merged. BSProcessMessages takes up to the given number of messages
// from the 1024-entry message queue and, for each whose receiver is still the
// same instance (the value at +16), finds the first handler in the target's
// list matching the message's entry (number, value and kind; a handler flagged
// 4 ends the search), runs it when it was sent by the registered sender or its
// entry allows any sender (bit 1), pushing the four message arguments on the
// handler thread's stack, and then processes the immediate events.
// BSMessageRemoveHandler unlinks a message registration from its message's
// list when the handler entry is registered (bit 0 of its byte at +11; the
// list head for the message number at +8 lives in g_MessageRegistrationArray),
// then pushes the registration onto the free list. BSSendMessage (the trigger
// is the sender's when there is a sender) offers the message to each
// registration of its number that is not replaced (bit 1 of +22) and whose
// object is running, by the entry's kind at +10: 0 always; 1 and 4 by the low
// half of the value at +4 (1: the object is the sender or target; 2: the
// sender is the object's trigger's object, for kind 1 only; 3: the trigger's
// object or the target's is the object, and for kind 1 not the sender); 2 when
// the trigger is in the group in the high half of the value; 3 when the two
// triggers share a group of that category (BSMessageIsGroupCategoryMember, its
// result read as a byte). A matching registration is queued, unless the queue
// is full, with the sender, target, trigger, data and the registration's
// object, thread, entry, instance value and value. BSRegisterMessage: for a
// registered handler entry it first looks through its message's registrations
// for one of the same thread whose entry has the same values at +4 and +10:
// one with the same entry value at +0 is returned as it is, any other is
// marked replaced (bit 1 of +22) and reported through the last argument. Then
// a registration is taken from the free list (none: 0), filled (object,
// thread, entry, value, flags) and, for a registered entry, pushed onto the
// front of its message's list. BSEndMessage clears the registration array and
// free-list pointers. BSInitMessage takes the message memory (the 729-entry
// registration array and the registration records) in one block, empties the
// queue and the array and links the records into the free list.
// BSMessageGetMemoryRequirements returns the block's size. The functions,
// globals and the handler-entry and registration structs are named by the
// symbols; the core, list, queue, node, target, thread and object views, the
// members and the inline helpers are inferred from offsets. The queue record
// is read as the processing arguments and as the sending fields; where those
// views differ the record has anonymous unions.
struct BSStateView;
struct BSMachineThread_struct;
struct TriggerObject_struct;

struct BSObject {
    unsigned char unknown00[8];
    TriggerObject_struct* trigger;
    unsigned char unknown0c[4];
    int m_value10;
};

struct TriggerCoreView {
    unsigned char unknown00[68];
    unsigned int* groups;
};

struct TriggerObject_struct {
    unsigned char unknown00[4];
    TriggerCoreView* core;
    BSObject* object;
};

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

/* a message handler entry (also the key a message is matched against) */
struct BSCode_MessageHandlerEntry_struct {
    union {
        unsigned int code;
        unsigned long value00;
    };
    union {
        unsigned int value;
        unsigned long value04;
    };
    unsigned short message;
    union {
        signed char kind;
        signed char value0a;
    };
    unsigned char flags;
};

/* a registration: the object and thread that handle the entry */
struct BSMessageRegistration_struct {
    BSObject* object;
    BSMachineThread_struct* thread;
    BSCode_MessageHandlerEntry_struct* entry;
    BSMessageRegistration_struct* prev;
    BSMessageRegistration_struct* next;
    unsigned short value;
    unsigned short flags;
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
    union {
        int arg1;
        BSObject* sender;
    };
    union {
        int arg2;
        BSObject* target;
    };
    BSObject* object;
    union {
        MessageTargetView* receiver;
        BSMachineThread_struct* thread;
    };
    union {
        int arg3;
        TriggerObject_struct* trigger;
    };
    union {
        int arg0;
        void* data;
    };
    int value;
    union {
        BSCode_MessageHandlerEntry_struct* key;
        BSCode_MessageHandlerEntry_struct* entry;
    };
    union {
        unsigned short senderValue;
        unsigned short registrationValue;
    };
};

extern MessageView g_MessageQueue[1024];
extern int g_MessageQueueHead;
extern int g_MessageQueueTail;
extern int* g_piStackTop;
extern BSMessageRegistration_struct** g_MessageRegistrationArray;
extern BSMessageRegistration_struct* g_MessageRegistrationFreeListHead;
extern BSMessageRegistration_struct* g_MessageRegistrationFreeList;
extern int g_iBSMessageRegistrationListSize;
extern unsigned char* g_pBSMessageMemory;
extern int g_pBSMessageMemoryOffset;

bool BSMessageIsGroupCategoryMember(TriggerObject_struct* first, TriggerObject_struct* second, unsigned int match) {
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
            MessageNodeView* node = FindMessageHandler(message->receiver->handlers, message->key);
            bool run = false;
            if (node) {
                if (node->sender != message->senderValue)
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

void BSMessageRemoveHandler(BSMessageRegistration_struct* registration) {
    if (registration->entry->flags & 1) {
        if (registration->prev)
            registration->prev->next = registration->next;
        else
            g_MessageRegistrationArray[registration->entry->message] = registration->next;
        if (registration->next)
            registration->next->prev = registration->prev;
    }
    registration->next = g_MessageRegistrationFreeListHead;
    g_MessageRegistrationFreeListHead = registration;
}


inline bool IsGroupMember(TriggerObject_struct* trigger, unsigned int match) {
    if (!trigger)
        return false;
    unsigned int category = match >> 16;
    unsigned int* groups = trigger->core->groups;
    if (groups) {
        unsigned int* group = groups + 1;
        unsigned int* last = group + groups[0];
        for (; group <= last; group++) {
            if ((*group & 0xff) == category)
                return true;
        }
    }
    return false;
}

inline void QueueMessage(BSObject* sender, BSObject* target, TriggerObject_struct* source, BSMessageRegistration_struct* registration, void* data) {
    if (g_MessageQueueHead == (g_MessageQueueTail + 1 >= 1024 ? 0 : g_MessageQueueTail + 1))
        return;
    MessageView* queued = &g_MessageQueue[g_MessageQueueTail];
    queued->sender = sender;
    queued->target = target;
    queued->trigger = source;
    queued->object = registration->object;
    queued->thread = registration->thread;
    queued->data = data;
    queued->entry = registration->entry;
    queued->value = registration->object->m_value10;
    queued->registrationValue = registration->value;
    g_MessageQueueTail = g_MessageQueueTail + 1 >= 1024 ? 0 : g_MessageQueueTail + 1;
}

int BSSendMessage(unsigned short message, BSObject* sender, TriggerObject_struct* trigger, BSObject* target, void* data) {
    TriggerObject_struct* source = trigger;
    if (sender)
        source = sender->trigger;
    for (BSMessageRegistration_struct* registration = g_MessageRegistrationArray[message]; registration;
         registration = registration->next) {
        if (registration->flags & 2)
            continue;
        BSObject* object = registration->object;
        if (object->m_value10 == -1)
            continue;
        BSCode_MessageHandlerEntry_struct* entry = registration->entry;
        bool send = false;
        switch (entry->value0a) {
        case 0:
            send = true;
            break;
        case 1:
            switch ((unsigned short)entry->value04) {
            case 1:
                if (object == sender)
                    send = true;
                break;
            case 2:
                if (object->trigger->object == sender)
                    send = true;
                break;
            case 3:
                if (source && source->object == object && source->object != sender)
                    send = true;
                break;
            }
            break;
        case 4:
            switch ((unsigned short)entry->value04) {
            case 1:
                if (object == target)
                    send = true;
                break;
            case 2:
                break;
            case 3:
                if (target && target->trigger->object == object)
                    send = true;
                break;
            }
            break;
        case 2:
            send = IsGroupMember(source, entry->value04);
            break;
        case 3:
            send = BSMessageIsGroupCategoryMember(source, object->trigger, entry->value04);
            break;
        }
        if (!send)
            continue;
        QueueMessage(sender, target, source, registration, data);
    }
    return 0;
}

void* BSUtilGetMemory(int, int);

BSMessageRegistration_struct* BSRegisterMessage(BSCode_MessageHandlerEntry_struct* entry, BSObject* object,
                                                unsigned short value, BSMachineThread_struct* thread,
                                                BSMessageRegistration_struct** replaced) {
    *replaced = 0;
    if (entry->flags & 1) {
        for (BSMessageRegistration_struct* other = g_MessageRegistrationArray[entry->message]; other;
             other = other->next) {
            BSCode_MessageHandlerEntry_struct* otherEntry = other->entry;
            if (thread == other->thread && entry->value04 == otherEntry->value04 &&
                entry->value0a == otherEntry->value0a) {
                if (entry->value00 == otherEntry->value00)
                    return other;
                other->flags |= 2;
                *replaced = other;
                break;
            }
        }
    }
    BSMessageRegistration_struct* registration = g_MessageRegistrationFreeListHead;
    g_MessageRegistrationFreeListHead = registration->next;
    if (!registration)
        return 0;
    registration->object = object;
    registration->thread = thread;
    registration->entry = entry;
    registration->value = value;
    registration->flags = 0;
    if (entry->flags & 1) {
        registration->next = g_MessageRegistrationArray[entry->message];
        registration->prev = 0;
        g_MessageRegistrationArray[entry->message] = registration;
        if (registration->next)
            registration->next->prev = registration;
    } else {
        registration->next = 0;
        registration->prev = 0;
    }
    return registration;
}

int BSEndMessage() {
    g_MessageRegistrationArray = 0;
    g_MessageRegistrationFreeList = 0;
    return 0;
}

inline void LinkFreeList() {
    for (int i = 0; i < g_iBSMessageRegistrationListSize - 1; i++)
        g_MessageRegistrationFreeList[i].next = &g_MessageRegistrationFreeList[i + 1];
}

inline void ClearRegistrations() {
    for (int i = 0; i < 729; i++)
        g_MessageRegistrationArray[i] = 0;
}

int BSInitMessage() {
    g_pBSMessageMemory = (unsigned char*)BSUtilGetMemory(g_iBSMessageRegistrationListSize * 24 + 2916, 0);
    g_pBSMessageMemoryOffset = 0;
    g_MessageQueueHead = 0;
    g_MessageQueueTail = 0;
    g_MessageRegistrationArray = (BSMessageRegistration_struct**)(g_pBSMessageMemory + g_pBSMessageMemoryOffset);
    g_pBSMessageMemoryOffset += 2916;
    ClearRegistrations();
    g_MessageRegistrationFreeList = (BSMessageRegistration_struct*)(g_pBSMessageMemory + g_pBSMessageMemoryOffset);
    g_pBSMessageMemoryOffset += g_iBSMessageRegistrationListSize * 24;
    LinkFreeList();
    g_MessageRegistrationFreeList[g_iBSMessageRegistrationListSize - 1].next = 0;
    g_MessageRegistrationFreeListHead = g_MessageRegistrationFreeList;
    return 0;
}

int BSMessageGetMemoryRequirements() {
    return g_iBSMessageRegistrationListSize * 24 + 2916;
}
