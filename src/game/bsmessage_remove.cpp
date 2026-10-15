// A fragment of bsmessage.cpp (0x80039eb0): BSMessageRemoveHandler unlinks a
// message registration from its message's list when the handler entry is
// registered (bit 0 of its byte at +11; the list head for the message number
// at +8 lives in g_MessageRegistrationArray), then pushes the registration
// onto the free list. The function, the structure names and the globals are
// named by the symbols; the members are inferred from offsets.
struct BSCode_MessageHandlerEntry_struct {
    unsigned char unknown00[8];
    unsigned short message;
    unsigned char unknown0a;
    unsigned char flags;
};

struct BSMessageRegistration_struct {
    unsigned char unknown00[8];
    BSCode_MessageHandlerEntry_struct* entry;
    BSMessageRegistration_struct* prev;
    BSMessageRegistration_struct* next;
};

extern BSMessageRegistration_struct** g_MessageRegistrationArray;
extern BSMessageRegistration_struct* g_MessageRegistrationFreeListHead;

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
