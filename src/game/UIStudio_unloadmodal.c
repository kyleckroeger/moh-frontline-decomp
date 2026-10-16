// A fragment of UIStudio.c (0x800882b8): UISInternalUnloadScreen unloads a
// screen: it first pops its modal record (the file-local
// UISInternalUnloadModal after it refuses a screen that is not on top), clears
// the current screen if it is this one, gives the screen's children its own
// parent, marks the screen references dirty, parses the screen's maps (with
// flag bit 1 set), initialisation and transforms for unloading, calls the
// studio's unload callback with the screen's data, removes the 12-byte screen
// record (moving the later records down and fixing the modal and reference
// records that point at them, and the current screen index) and, when no
// screen is current, makes the parent (or the last screen) current through
// thread action 3. The file is compiled with deferred inlining, so the
// functions are listed in reverse image order. UISInfo_t, UISScreen_t,
// UISStackInfo_t, UISParam_t, UISControlInfo_t, UISThreadGroupInfoT,
// UISThreadActionT and UISTransformAction are named by the mangled symbols;
// the screen, modal, reference, studio and stack views, the hoisted
// declarations and the result meanings are inferred. The rest of the file is
// in other units.
struct UISParam_t;
struct UISControlInfo_t;

struct UISScreen_t {
    unsigned short group;
    unsigned short id;
    unsigned short parentGroup;
    unsigned short parentId;
    void* data;
};

/* inferred: the stack state, its stack pointer at +12 */
struct UISStackInfo_t {
    unsigned long value00;
    unsigned long value04;
    unsigned long value08;
    unsigned long* sp;
    unsigned long value10;
};

/* inferred: a modal record */
struct UISModal_t {
    UISStackInfo_t saved;
    UISControlInfo_t* control;
    UISScreen_t* from;
    UISParam_t* param;
    UISStackInfo_t* stack;
    unsigned short id;
    unsigned short group;
};

/* inferred: a record that refers to a screen */
struct UISScreenRefView {
    char field00[20];
    unsigned long dirty;
    char field18[4];
    UISScreen_t* screen;
    char field20[20];
};

struct UISInfo_t {
    char field00[4];
    unsigned long flags;
    char field08[12];
    void (*unload)(unsigned short, unsigned short, void*);
    char field18[12];
    unsigned long currentScreen;
    char field28[4];
    unsigned long screenCount;
    UISScreen_t* screens;
    char field34[12];
    long refCount;
    long dirtyRefCount;
    UISScreenRefView* refs;
    char field4c[4];
    long modalCount;
    UISModal_t* modals;
    UISStackInfo_t stack;
};

struct UISThreadGroupInfoT {
    unsigned short group;
    unsigned short thread;
    long value04;
    long value08;
    long value0c;
};

enum UISThreadActionT {};
enum UISTransformAction {};

unsigned long UISFindScreen(UISInfo_t*, unsigned short, unsigned short);
signed char UISStackProcess(UISInfo_t*, UISParam_t*, UISStackInfo_t*, UISScreen_t*, UISControlInfo_t*);
void UISRemoveUnNessaryRateFncs(UISInfo_t*);
void _ParseMaps(UISInfo_t*, UISScreen_t*, UISStackInfo_t*, unsigned long, unsigned long, unsigned long, unsigned long,
                UISParam_t*, signed char*);
void _ParseInitialize(UISInfo_t*, UISScreen_t*, unsigned long, unsigned long);
void _ParseTransforms(UISInfo_t*, UISTransformAction, UISScreen_t*, unsigned long);
void UISAddThreadAction(UISInfo_t*, UISThreadActionT, UISThreadGroupInfoT*, long, UISParam_t*);
extern "C" void* memmove(void*, const void*, unsigned long);

static unsigned char UISInternalUnloadModal(UISInfo_t* info, unsigned short group, unsigned short id, long value) {
    UISStackInfo_t* stack;
    UISModal_t* modal;
    UISParam_t* param;
    unsigned long* sp;
    long count = info->modalCount;
    if (count > 0) {
        modal = &info->modals[count - 1];
        if (modal->id == id && modal->group == group) {
            info->modalCount = count - 1;
            if (modal->from) {
                info->currentScreen = UISFindScreen(info, modal->from->group, modal->from->id);
            } else {
                info->currentScreen = -1;
                return 1;
            }
            if (info->currentScreen < info->screenCount) {
                param = modal->param;
                if (param) {
                    stack = modal->stack;
                    *stack = modal->saved;
                    sp = stack->sp;
                    sp[-1] = value;
                    if (UISStackProcess(info, modal->param, stack, modal->from, modal->control) != 3)
                        stack->sp = (unsigned long*)param;
                }
            }
        } else {
            while (count-- != 0) {
                UISModal_t* other = &info->modals[count];
                if (other->id == id && other->group == group)
                    return 0;
                if (other->from && other->from->id == id && other->from->group == group)
                    return 0;
            }
        }
    }
    return 1;
}

unsigned char UISInternalUnloadScreen(UISInfo_t* info, unsigned short group, unsigned short id, long value) {
    UISScreen_t* screen;
    UISScreen_t* from;
    unsigned long index;
    unsigned short parentId;
    unsigned short parentGroup;
    unsigned long count;
    index = UISFindScreen(info, group, id);
    if (index < info->screenCount) {
        screen = &info->screens[index];
        parentGroup = screen->parentGroup;
        parentId = screen->parentId;
        if (!UISInternalUnloadModal(info, group, id, value))
            return 0;
        if (index == info->currentScreen)
            info->currentScreen = -1;
        UISScreen_t* child;
        for (unsigned long i = 0; i < info->screenCount; i++) {
            child = &info->screens[i];
            if (child->parentGroup == group && child->parentId == id) {
                child->parentGroup = parentGroup;
                child->parentId = parentId;
            }
        }
        count = info->dirtyRefCount;
        while (count-- != 0) {
            if (info->refs[count].screen == screen)
                info->refs[count].dirty = 1;
        }
        UISRemoveUnNessaryRateFncs(info);
        signed char result = 0;
        info->flags |= 2;
        _ParseMaps(info, screen, &info->stack, 0, -1, -3, 0, 0, &result);
        info->flags &= ~2;
        _ParseInitialize(info, screen, 0, -3);
        _ParseTransforms(info, (UISTransformAction)3, screen, 0);
        info->unload(screen->group, screen->id, screen->data);
        screen->data = 0;
        info->screenCount--;
        for (; index < info->screenCount; index++) {
            screen = &info->screens[index];
            from = &info->screens[index + 1];
            memmove(screen, from, sizeof(UISScreen_t));
            count = info->modalCount;
            while (count-- != 0) {
                if (info->modals[count].from == from)
                    info->modals[count].from = screen;
            }
            count = info->refCount;
            while (count-- != 0) {
                if (info->refs[count].screen == from)
                    info->refs[count].screen = screen;
            }
            if (index + 1 == info->currentScreen)
                info->currentScreen = index;
        }
        if (info->currentScreen == (unsigned long)-1) {
            if (info->screenCount != 0) {
                UISScreen_t* current;
                info->currentScreen = UISFindScreen(info, parentGroup, parentId);
                if (info->currentScreen < info->screenCount) {
                    current = &info->screens[info->currentScreen];
                } else {
                    info->currentScreen = info->screenCount - 1;
                    current = &info->screens[info->currentScreen];
                }
                if (current) {
                    UISThreadGroupInfoT thread;
                    thread.group = current->group;
                    thread.thread = current->id;
                    UISAddThreadAction(info, (UISThreadActionT)3, &thread, 0, 0);
                }
            } else {
                info->currentScreen = -1;
            }
        }
    }
    return 1;
}
