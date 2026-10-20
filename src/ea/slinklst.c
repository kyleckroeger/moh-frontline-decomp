// Doubly linked lists with head, tail and count, and (at the end of the file)
// unsigned 64-bit multiply/divide helpers. SNDLINKLIST and SNDLINKNODE
// are named by the mangled symbols; their members are inferred from offsets.
struct SNDLINKNODE {
    SNDLINKNODE* next;
    SNDLINKNODE* prev;
};

struct SNDLINKLIST {
    SNDLINKNODE* head;
    SNDLINKNODE* tail;
    int count;
};

void SNDLINKI_init(SNDLINKLIST* list) {
    list->head = 0;
    list->tail = 0;
    list->count = 0;
}

void SNDLINKI_push(SNDLINKLIST* list, SNDLINKNODE* node) {
    node->next = list->head;
    node->prev = 0;
    if (list->head)
        list->head->prev = node;
    else
        list->tail = node;
    list->head = node;
    list->count++;
}

void SNDLINKI_pushtail(SNDLINKLIST* list, SNDLINKNODE* node) {
    node->next = 0;
    node->prev = list->tail;
    if (list->tail)
        list->tail->next = node;
    else
        list->head = node;
    list->tail = node;
    list->count++;
}

SNDLINKNODE* SNDLINKI_pop(SNDLINKLIST* list) {
    SNDLINKNODE* node = list->head;

    if (list->head) {
        list->head = list->head->next;
        if (!list->head)
            list->tail = 0;
        else
            list->head->prev = 0;
        list->count--;
    }
    return node;
}

void SNDLINKI_remove(SNDLINKLIST* list, SNDLINKNODE* node) {
    if (node == list->head)
        list->head = list->head->next;
    if (node == list->tail)
        list->tail = list->tail->prev;
    if (node->prev)
        node->prev->next = node->next;
    if (node->next)
        node->next->prev = node->prev;
    list->count--;
}

// The file ends with the unsigned 64-bit helpers: a 32x32 multiply built from
// 16-bit partial products and a 64/32 shift-subtract division (the high word
// must be below the divisor). SINT64 is named by the mangled symbols; its
// low/high word order is inferred from the stores.
struct SINT64 {
    unsigned int lo;
    unsigned int hi;
};

SINT64 iSNDmulu64(unsigned int a, unsigned int b) {
    SINT64 result;
    unsigned int carry;
    unsigned int al = a & 0xffff;
    unsigned int bl = b & 0xffff;
    unsigned int ah = a >> 16;
    unsigned int bh = b >> 16;
    unsigned int ll = al * bl;
    unsigned int m1 = al * bh;
    unsigned int m2 = ah * bl;
    unsigned int hh = ah * bh;
    carry = ((ll >> 16) + (m1 & 0xffff) + (m2 & 0xffff)) >> 16;
    result.lo = ll + (m1 << 16) + (m2 << 16);
    result.hi = (m1 >> 16) + (m2 >> 16) + hh + carry;
    return result;
}

unsigned int iSNDdivu64(SINT64 value, int divisor) {
    unsigned int lo = value.lo;
    unsigned int rem = value.hi;
    unsigned int quotient = 0;
    int i;
    for (i = 0; i < 32; i++) {
        rem <<= 1;
        rem += lo >> 31;
        quotient <<= 1;
        lo <<= 1;
        if (rem >= (unsigned int)divisor) {
            rem -= divisor;
            quotient++;
        }
    }
    return quotient;
}
