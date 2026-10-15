// A fragment of bsmessage.cpp (0x8003a1bc): BSRegisterMessage. For a
// registered handler entry (bit 0 of its byte at +11) it first looks through
// its message's registrations for one of the same thread whose entry has the
// same values at +4 and +10: one with the same entry value at +0 is returned
// as it is, any other is marked replaced (bit 1 of +22) and reported through
// the last argument. Then a registration is taken from the free list (none:
// 0), filled (object, thread, entry, value, flags) and, for a registered
// entry, pushed onto the front of its message's list. The function, the
// structure names and the globals are named by the symbols; the members
// follow bsmessage_remove.cpp and are otherwise inferred from offsets.
// BSEndMessage clears the registration array and free-list pointers.
struct BSObject;
struct BSMachineThread_struct;

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

extern BSMessageRegistration_struct** g_MessageRegistrationArray;
extern BSMessageRegistration_struct* g_MessageRegistrationFreeListHead;
extern BSMessageRegistration_struct* g_MessageRegistrationFreeList;

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
