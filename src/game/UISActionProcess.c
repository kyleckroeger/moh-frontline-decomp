// UI thread actions: a downward-growing stack of queued actions (action id,
// a 16-byte group record, a parameter count and the parameters), run in
// order or, for the activation pass, only the control activations.
// UISInfo_t, UISParam_t, UISThreadGroupInfoT, UISThreadActionT and the
// called functions are named by the mangled symbols; the record views and
// field meanings are inferred. The file is compiled with deferred inlining
// (UISAddThreadAction, last in the image, is inlined into
// _UISDoThreadAction), so functions are listed in reverse image order. The
// 'base' copy of the stack base and the separate declarations are inferred
// from the register assignment.
struct UISScreen_t;
struct UISControlInfo_t;
struct UISControlList_t;
struct UISStackInfo_t;

struct UISParam_t {
    unsigned long value;
};

// The action values are the case numbers of _UISDoThreadAction; their names
// are not known.
enum UISThreadActionT { UISThreadActionUnknown = 0 };

union UISThreadGroupInfoT {
    struct {
        unsigned short u0;
        unsigned short u2;
        unsigned short u4;
        unsigned short u6;
        long l8;
        unsigned short u12;
        unsigned short u14;
    } h;
    struct {
        short s0;
        short s2;
        long l4;
        long l8;
        long l12;
    } s;
};

struct UISInfo_t {
    char field000[4];
    unsigned long flags;
    char field008[80];
    UISStackInfo_t* stack;
    char field05C[68];
    UISParam_t* actionBase;
    UISParam_t* actionTop;
};

void UISInternalLoadScreen(UISInfo_t*, unsigned short, unsigned short, unsigned short, unsigned short,
                           unsigned char, UISParam_t*);
unsigned char UISInternalUnloadScreen(UISInfo_t*, unsigned short, unsigned short, long);
void UISDoHint(UISInfo_t*, unsigned long, unsigned long, UISParam_t*);
void UISProcessInternalEvents(UISInfo_t*, UISStackInfo_t*, long, unsigned long, unsigned long, UISParam_t*,
                              unsigned char);
void UISInternalActivateScreen(UISInfo_t*, unsigned char, unsigned short, unsigned short);
void UISInternalActivateControl(UISInfo_t*, unsigned char, unsigned long, UISControlInfo_t*, UISControlList_t*,
                                unsigned short, unsigned short);
void UISMoveScreenDrawPosition(UISInfo_t*, unsigned short, unsigned short, long);

void UISAddThreadAction(UISInfo_t* info, UISThreadActionT action, UISThreadGroupInfoT* group, long count,
                        UISParam_t* params) {
    UISParam_t* dst;
    UISParam_t* top = info->actionTop;

    top[0].value = action;
    *(UISThreadGroupInfoT*)(top - 5) = *group;
    top[-6].value = count;
    dst = top - 7;
    if (params) {
        for (long i = count - 1; i >= 0; i--)
            *dst-- = params[i];
    }
    info->actionTop = dst;
}

static UISParam_t* _UISDoThreadAction(UISInfo_t* info, UISParam_t* entry) {
    UISThreadGroupInfoT* group;
    UISThreadActionT action;
    long count;
    UISParam_t* params;
    params = entry - 5;
    group = (UISThreadGroupInfoT*)params;
    params--;
    action = (UISThreadActionT)entry[0].value;
    count = entry[-6].value;
    params -= count;

    switch (action) {
    case 0:
        UISInternalLoadScreen(info, group->h.u0, group->h.u2, group->h.u4, group->h.u6, count, params);
        break;
    case 1:
        if (!UISInternalUnloadScreen(info, group->h.u0, group->h.u2, group->h.l8))
            UISAddThreadAction(info, action, group, count, params);
        break;
    case 9:
        UISDoHint(info, *(unsigned long*)group, count, params);
        break;
    case 2:
        info->flags |= 2;
        UISProcessInternalEvents(info, (UISStackInfo_t*)&info->stack, *(unsigned long*)group, -8, count, params, 1);
        info->flags &= ~2;
        break;
    case 3:
        UISInternalActivateScreen(info, 1, group->h.u0, group->h.u2);
        break;
    case 4:
        UISInternalActivateScreen(info, 0, group->h.u0, group->h.u2);
        break;
    case 5:
        if (group->s.s2 == 0) {
            UISInternalActivateControl(info, 1, group->s.s0, (UISControlInfo_t*)group->s.l8,
                                       (UISControlList_t*)group->s.l4, group->h.u12, group->h.u14);
            group->s.s2 = 1;
        }
        break;
    case 6:
        if (group->s.s2 == 0) {
            UISInternalActivateControl(info, 0, group->s.s0, (UISControlInfo_t*)group->s.l8,
                                       (UISControlList_t*)group->s.l4, group->h.u12, group->h.u14);
            group->s.s2 = 1;
        }
        break;
    case 7:
        info->flags |= 2;
        UISProcessInternalEvents(info, (UISStackInfo_t*)&info->stack, group->s.l4, *(unsigned long*)group, count,
                                 params, 0);
        info->flags &= ~2;
        break;
    case 8:
        UISMoveScreenDrawPosition(info, group->h.u0, group->h.u2, group->s.l12);
        break;
    }
    return params - 1;
}

void UISProcessThreadAction(UISInfo_t* info, unsigned char activateOnly) {
    UISThreadGroupInfoT* group;
    UISParam_t* entry;
    UISParam_t* params;
    UISThreadActionT action;
    UISParam_t* base;

    entry = info->actionBase;
    base = entry;
    if (activateOnly) {
        for (; entry > info->actionTop; entry = params - 1) {
            params = entry - 5;
            group = (UISThreadGroupInfoT*)params;
            params--;
            action = (UISThreadActionT)entry[0].value;
            params -= entry[-6].value;
            switch (action) {
            case 5:
                if (group->s.s2 == 0) {
                    UISInternalActivateControl(info, 1, group->s.s0, (UISControlInfo_t*)group->s.l8,
                                               (UISControlList_t*)group->s.l4, group->h.u12, group->h.u14);
                    group->s.s2 = 1;
                }
                break;
            case 6:
                if (group->s.s2 == 0) {
                    UISInternalActivateControl(info, 0, group->s.s0, (UISControlInfo_t*)group->s.l8,
                                               (UISControlList_t*)group->s.l4, group->h.u12, group->h.u14);
                    group->s.s2 = 1;
                }
                break;
            }
        }
    } else {
        while (entry > info->actionTop)
            entry = _UISDoThreadAction(info, entry);
        info->actionTop = base;
    }
}
