// A fragment of UIStudio.c (0x80087cf4): UISInternalActivateControl
// enables or disables (activate) a control of a screen: it finds the
// control's entry among the screen's 20-byte controls (from the last) and the
// last control with a plain sub-control event naming that index; when
// enabling, a sibling-enable control is first disabled through a recursive
// call; then it sets the control's enable value and runs the entry's
// activate (-6) or deactivate (-7) event and, for the sub-control, the
// matching sub-control event, passing the value. UISInfo_t, UISScreen_t,
// UISControlInfo_t, UISControlList_t, UISStackInfo_t, UISParam_t and the
// functions are named by the mangled symbols; the control, event and studio
// views, the hoisted declarations and the event meanings are inferred. The
// rest of the file is in other units.
struct UISParam_t;
struct UISStackInfo_t;

struct UISControlInfo_t {
    unsigned long active;
    unsigned long enable;
};

struct UISEventEntry_t {
    unsigned short flags;
    unsigned short id;
    void* pc;
};

/* inferred: a screen control (20 bytes): the control and its events */
struct UISControl_t {
    UISControlInfo_t* control;
    char field04[8];
    long eventCount;
    UISEventEntry_t* events;
};

struct UISControlList_t {
    unsigned long count;
    UISControl_t* entries;
};

struct UISScreen_t {
    unsigned short group;
    unsigned short id;
    char field04[4];
    UISControlList_t* controls;
};

struct UISInfo_t {
    char field00[44];
    unsigned long screenCount;
    UISScreen_t* screens;
    char field34[36];
    char stack[4];
};

unsigned long UISFindScreen(UISInfo_t*, unsigned short, unsigned short);
UISControlInfo_t* UISFindSiblingEnableControl(UISScreen_t*, UISControlInfo_t*);
void* UISFindEventPC(UISControl_t*, unsigned long);
void* UISFindSubControlEventPC(UISControl_t*, unsigned short, unsigned long);
void UISExecuteFnc(UISInfo_t*, UISScreen_t*, UISControlInfo_t*, UISStackInfo_t*, void*, unsigned long, UISParam_t*,
                   unsigned long, UISParam_t*, unsigned char, long, UISParam_t*);

void UISInternalActivateControl(UISInfo_t* info, unsigned char activate, unsigned long value, UISControlInfo_t* control,
                                UISControlList_t* list, unsigned short id, unsigned short group) {
    unsigned long i;
    UISScreen_t* screen;
    UISControl_t* sub;
    UISControl_t* found;
    unsigned long event;
    UISControl_t* entry;
    unsigned long index;
    unsigned long count;
    UISControlList_t* controls;
    unsigned long j;
    void* pc;

    index = UISFindScreen(info, group, id);
    found = 0;
    if (index >= info->screenCount)
        return;
    screen = &info->screens[index];
    event = activate == 1 ? -6 : -7;
    controls = screen->controls;
    i = (count = controls->count);
    while (i-- != 0) {
        UISControl_t* entry = &controls->entries[i];
        if (entry->control == control) {
            found = entry;
            break;
        }
    }
    sub = 0;
    j = count;
    while (j-- != 0) {
        entry = &screen->controls->entries[j];
        for (long k = 0; k < entry->eventCount; k++) {
            unsigned short flags = entry->events[k].flags;
            if ((flags & 0xc000) == 0 && (flags & 0x2fff) == (short)i) {
                sub = entry;
                break;
            }
        }
    }
    if (activate == 1) {
        UISControlInfo_t* sibling = UISFindSiblingEnableControl(screen, control);
        if (sibling)
            UISInternalActivateControl(info, 0, 0, sibling, 0, id, group);
    }
    pc = UISFindEventPC(found, event);
    found->control->enable = activate;
    if (pc)
        UISExecuteFnc(info, screen, found->control, (UISStackInfo_t*)info->stack, pc, 0, 0, 0, 0, 1, value, 0);
    if (sub) {
        pc = UISFindSubControlEventPC(sub, i, event);
        if (pc)
            UISExecuteFnc(info, screen, sub->control, (UISStackInfo_t*)info->stack, pc, 0, 0, 0, 0, 1, value, 0);
    }
}
