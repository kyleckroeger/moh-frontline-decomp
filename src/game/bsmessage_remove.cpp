// A fragment of bsmessage.cpp (0x80039eb0): BSMessageRemoveHandler unlinks a
// message registration from its message's list when the handler entry is
// registered (bit 0 of its byte at +11; the list head for the message number
// at +8 lives in g_MessageRegistrationArray), then pushes the registration
// onto the free list. BSSendMessage (the trigger is the sender's when there is
// a sender) offers the message to each registration of its number that is not
// replaced (bit 1 of +22) and whose object is running, by the entry's kind at
// +10: 0 always; 1 and 4 by the low half of the value at +4 (1: the object is
// the sender or target; 2: the sender is the object's trigger's object, for
// kind 1 only; 3: the trigger's object or the target's is the object, and for
// kind 1 not the sender); 2 when the trigger is in the group in the high half
// of the value; 3 when the two triggers share a group of that category
// (BSMessageIsGroupCategoryMember, its result read as a byte). A matching
// registration is queued, unless the 1024-entry queue is full, with the
// sender, target, trigger, data and the registration's object, thread, entry,
// instance value and value. The functions, the structure names and the globals
// are named by the symbols; the members follow bsmessage_register.cpp and are
// otherwise inferred from offsets, as are the queue, object and trigger views
// and the two helpers (inline, inferred).
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

struct BSCode_MessageHandlerEntry_struct {
    unsigned long value00;
    unsigned long value04;
    unsigned short message;
    signed char value0a;
    unsigned char flags;
};

struct BSMessageRegistration_struct {
    BSObject* object;
    BSMachineThread_struct* thread;
    BSCode_MessageHandlerEntry_struct* entry;
    BSMessageRegistration_struct* prev;
    BSMessageRegistration_struct* next;
    unsigned short value;
    unsigned short flags;
};

/* inferred: a queued message (36 bytes) */
struct MessageView {
    BSObject* sender;
    BSObject* target;
    BSObject* object;
    BSMachineThread_struct* thread;
    TriggerObject_struct* trigger;
    void* data;
    int value;
    BSCode_MessageHandlerEntry_struct* entry;
    unsigned short registrationValue;
};

extern BSMessageRegistration_struct** g_MessageRegistrationArray;
extern BSMessageRegistration_struct* g_MessageRegistrationFreeListHead;
extern MessageView g_MessageQueue[1024];
extern int g_MessageQueueHead;
extern int g_MessageQueueTail;

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

bool BSMessageIsGroupCategoryMember(TriggerObject_struct*, TriggerObject_struct*, unsigned int);

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
