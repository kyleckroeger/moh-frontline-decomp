// A fragment of UIStudio.c (0x80088120-0x800882b8; the file is compiled with
// deferred inlining, so the functions are written in reverse image order).
// UISProcessEvent runs the
// pending thread actions, then, if the event is enabled in the studio's event
// mask, refreshes the active screen from the top of the screen stack and
// parses the event maps of either every screen or only the active one (none
// when no screen is active); uisEventNotProcess suppresses parsing unless it
// is 1 and the first event value is negative. UISSetScreenActive queues
// thread action 3 with a group record holding the two screen values (the rest
// of the record is left unset, no parameters) and runs the queued actions at
// once unless flag bit 1 of the studio record is set. UISInfo_t,
// UISThreadGroupInfoT, UISThreadActionT and UISParam_t are named by the
// mangled symbols; the views and the parameter meanings are inferred (see
// UISActionProcess.c). The rest of the file is not part of this unit.
struct UISParam_t;

enum UISThreadActionT { UISThreadActionUnknown = 0 };

union UISThreadGroupInfoT {
    struct {
        short s0;
        short s2;
        long l4;
        long l8;
        long l12;
    } s;
};

struct UISScreen_t {
    char field00[12];
};

struct UISStackInfo_t;

struct UISStackEntryView {
    char field00[36];
    unsigned short screen;
    unsigned short control;
};

struct UISInfo_t {
    char field000[4];
    unsigned long flags;
    char field008[24];
    unsigned long eventMask;
    unsigned long activeScreen;
    char field028[4];
    unsigned long screenCount;
    UISScreen_t* screens;
    char field034[28];
    long stackDepth;
    UISStackEntryView* stack;
    char stackInfo[4];
};

extern signed char uisEventNotProcess;
unsigned long UISFindScreen(UISInfo_t*, unsigned short, unsigned short);
void _ParseMaps(UISInfo_t*, UISScreen_t*, UISStackInfo_t*, unsigned long, unsigned long, unsigned long,
                unsigned long, UISParam_t*, signed char*);

void UISAddThreadAction(UISInfo_t* info, UISThreadActionT action, UISThreadGroupInfoT* group, long count,
                        UISParam_t* params);
void UISProcessThreadAction(UISInfo_t* info, unsigned char activation);

extern "C" void UISSetScreenActive(UISInfo_t* info, unsigned short screen, unsigned short control) {
    UISThreadGroupInfoT group;

    group.s.s0 = screen;
    group.s.s2 = control;
    UISAddThreadAction(info, (UISThreadActionT)3, &group, 0, 0);
    if (!(info->flags & 2))
        UISProcessThreadAction(info, 0);
}

extern "C" void UISProcessEvent(UISInfo_t* info, long event, unsigned long p1, unsigned long p2,
                                UISParam_t* params, unsigned char all) {
    unsigned long i;
    unsigned long end;
    signed char result;

    UISProcessThreadAction(info, 0);
    if (!(info->eventMask & (1 << event)))
        return;
    long top = info->stackDepth - 1;
    if (top >= 0)
        info->activeScreen = UISFindScreen(info, info->stack[top].control, info->stack[top].screen);
    if (all) {
        end = info->screenCount;
        i = 0;
    } else {
        i = info->activeScreen;
        end = i + 1;
        if (i == (unsigned long)-1)
            return;
    }
    for (; i < end; i++) {
        UISScreen_t* screen = &info->screens[i];
        if ((uisEventNotProcess == 1 && (long)p1 < 0) || !uisEventNotProcess) {
            result = 0;
            info->flags |= 2;
            _ParseMaps(info, screen, (UISStackInfo_t*)info->stackInfo, 0, event, p1, p2, params, &result);
            info->flags &= ~2;
        }
    }
}
