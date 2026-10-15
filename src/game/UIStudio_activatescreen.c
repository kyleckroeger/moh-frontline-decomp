// A fragment of UIStudio.c (0x80087b50): UISInternalActivateScreen sends the
// current screen internal event -5 (when no modal is open), then, when
// activating, makes the found screen current, enables its first control and
// sends it internal event -4; each event is sent with the studio's busy flag
// (bit 1 of +4) set. UISInfo_t, UISScreen_t, UISStackInfo_t, UISParam_t
// and UISControlInfo_t are named by the mangled symbols; the views follow
// UIStudio_internal.c, UISUtils_siblingenable.c and UIStudio_unloadmodal.c,
// UISProcessInternalEvents's body follows UIStudio_internal.c (the file
// defers inlining; this unit marks it inline so that it is inlined without
// being emitted), and the event meanings are inferred. The rest of the file
// is in other units.
struct UISParam_t;
struct UISStackInfo_t;

struct UISControlInfo_t {
    unsigned long active;
    unsigned long enable;
};

struct UISControlEntry_t {
    UISControlInfo_t* control;
};

struct UISControlList_t {
    unsigned long count;
    UISControlEntry_t* entries;
};

struct UISScreen_t {
    unsigned short group;
    unsigned short id;
    char field04[4];
    UISControlList_t* controls;
};

struct UISInfo_t {
    char field00[4];
    unsigned long flags;
    char field08[28];
    unsigned long currentScreen;
    char field28[4];
    unsigned long screenCount;
    UISScreen_t* screens;
    char field34[28];
    unsigned long modalCount;
    char field54[4];
    char stack[4];
};

unsigned long UISFindScreen(UISInfo_t*, unsigned short, unsigned short);
void _ParseMaps(UISInfo_t*, UISScreen_t*, UISStackInfo_t*, unsigned long, unsigned long, unsigned long,
                unsigned long, UISParam_t*, signed char*);

inline void UISProcessInternalEvents(UISInfo_t* info, UISStackInfo_t* stack, long event, unsigned long a,
                                     unsigned long b, UISParam_t* param, unsigned char all) {
    unsigned long i;
    unsigned long end;
    if (all) {
        end = info->screenCount;
        i = 0;
    } else {
        i = info->currentScreen;
        end = i + 1;
        if (i == 0xffffffff)
            return;
    }
    for (; i < end; i++) {
        UISScreen_t* screen = &info->screens[i];
        signed char result = 0;
        _ParseMaps(info, screen, stack, 0, event, a, b, param, &result);
    }
}

void UISInternalActivateScreen(UISInfo_t* info, unsigned char activate, unsigned short group, unsigned short id) {
    if (info->modalCount == 0) {
        info->flags |= 2;
        UISProcessInternalEvents(info, (UISStackInfo_t*)info->stack, 0, -5, 0, 0, 0);
        info->flags &= ~2;
    }
    if (activate) {
        unsigned long index = UISFindScreen(info, group, id);
        if (index < info->screenCount) {
            UISScreen_t* screen = &info->screens[index];
            screen->controls->entries->control->enable = 1;
            screen->controls->entries->control->active = 1;
            info->currentScreen = index;
            info->flags |= 2;
            UISProcessInternalEvents(info, (UISStackInfo_t*)info->stack, 0, -4, 0, 0, 0);
            info->flags &= ~2;
        }
    }
}
