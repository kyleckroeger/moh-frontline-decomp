// A fragment of UISUtils.c (0x8008a0d4): the file-local, recursive _ParseHints
// runs a hint over a screen control: each sub-control link (id 0xffff) is
// parsed first, then the control's code for the hint (the first event with
// flag 0x4000 and the hint's id) is run with the value and parameters. The
// compiler inlines the recursion three levels deep and the file-local
// _UISFindHintPC into the upper levels; the deepest level calls the original
// copy (UISUtils.c), so the copy compiled here is discarded. UISInfo_t,
// UISScreen_t, UISStackInfo_t, UISParam_t, UISControl_t and
// UISControlInfo_t are named by the mangled symbols; the views follow
// UISUtils.c and the hint meanings are inferred. The rest of the file is in
// other units.
struct UISParam_t;
struct UISStackInfo_t;
struct UISControlInfo_t;

struct UISEventEntry_t {
    unsigned short flags;
    unsigned short id;
    void* pc;
};

struct UISControl_t {
    UISControlInfo_t* control;
    char field04[8];
    unsigned long eventCount;
    UISEventEntry_t* events;
};

struct UISScrData_t {
    unsigned long controlCount;
    UISControl_t* controls;
};

struct UISScreen_t {
    unsigned short group;
    unsigned short id;
    char field04[4];
    UISScrData_t* data;
};

struct UISInfo_t;

int UISExecuteFnc(UISInfo_t*, UISScreen_t*, UISControlInfo_t*, UISStackInfo_t*, void*, unsigned long, UISParam_t*,
                  unsigned long, UISParam_t*, unsigned char, long, UISParam_t*);

static void* _UISFindHintPC(UISControl_t* control, unsigned long id) {
    for (unsigned long i = 0; i < control->eventCount; i++) {
        UISEventEntry_t* entry = &control->events[i];

        if ((entry->flags & 0x4000) && entry->id == (unsigned short)id)
            return entry->pc;
    }
    return 0;
}

static void _ParseHints(UISInfo_t* info, UISScreen_t* screen, UISStackInfo_t* stack, unsigned long index,
                        unsigned long hint, unsigned long value, UISParam_t* params) {
    UISControl_t* control = &screen->data->controls[index];
    for (unsigned long i = 0; i < control->eventCount; i++) {
        UISEventEntry_t* entry = &control->events[i];
        if (entry->id == 0xffff)
            _ParseHints(info, screen, stack, (unsigned long)entry->pc, hint, value, params);
    }
    void* pc = _UISFindHintPC(control, hint);
    if (pc)
        UISExecuteFnc(info, screen, control->control, stack, pc, value, params, 0, 0, 0, 0, 0);
}
