// UI lookups: a control's event and hint entries (8 bytes: flags with the
// event/hint kind in the top bits, an id and a code address) and a screen
// by its two ids. UISControl_t, UISInfo_t and UISScreen_t are named by the
// mangled symbols; the members are inferred from offsets.
struct UISEventEntry_t {
    unsigned short flags;
    unsigned short id;
    void* pc;
};

struct UISControl_t {
    char field00[12];
    unsigned long eventCount;
    UISEventEntry_t* events;
};

struct UISScreenEntry_t {
    unsigned short group;
    unsigned short id;
    char field04[8];
};

struct UISInfo_t {
    char field00[44];
    unsigned long screenCount;
    UISScreenEntry_t* screens;
};

// The hint parser, screen positioning, string formatting, colour factors,
// action values and function execution come first in the original file and
// are not part of this unit. Like the other UI files this is compiled with
// deferred inlining, which emits functions in reverse source order, so the
// four lookups are listed from the end of the image; the file-local
// _UISFindHintPC (only called from the hint parser) comes first in the image.

unsigned long UISFindScreen(UISInfo_t* info, unsigned short group, unsigned short id) {
    unsigned long i;

    for (i = 0; i < info->screenCount; i++) {
        UISScreenEntry_t* screen = &info->screens[i];

        if (screen->group == group && screen->id == id)
            break;
    }
    return i;
}

void* UISFindEventPC(UISControl_t* control, unsigned long id) {
    for (unsigned long i = 0; i < control->eventCount; i++) {
        UISEventEntry_t* entry = &control->events[i];

        if ((entry->flags & 0x8000) && entry->id == (unsigned short)id)
            return entry->pc;
    }
    return 0;
}

void* UISFindSubControlEventPC(UISControl_t* control, unsigned short sub, unsigned long id) {
    for (unsigned long i = 0; i < control->eventCount; i++) {
        UISEventEntry_t* entry = &control->events[i];

        if (!(entry->flags & 0xC000) && entry->id == (unsigned short)id && (entry->flags & 0x2FFF) == sub)
            return entry->pc;
    }
    return 0;
}

static void* _UISFindHintPC(UISControl_t* control, unsigned long id) {
    for (unsigned long i = 0; i < control->eventCount; i++) {
        UISEventEntry_t* entry = &control->events[i];

        if ((entry->flags & 0x4000) && entry->id == (unsigned short)id)
            return entry->pc;
    }
    return 0;
}
