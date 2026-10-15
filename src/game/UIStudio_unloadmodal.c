// A fragment of UIStudio.c (0x800885dc): the file-local
// UISInternalUnloadModal pops the top modal record (40 bytes, at +84, count
// at +80) when it is the given screen: the current screen (+36) becomes the
// screen it was opened from (none: -1, and done), and when that screen is
// known and the record has a parameter, the saved stack state is restored,
// the value is pushed on it and the stack processed (the stack pointer is
// put back unless processing ended with 3). A screen that is not on top but
// is open lower down, or was opened from, is refused (0). UISInfo_t,
// UISScreen_t, UISStackInfo_t, UISParam_t and UISControlInfo_t are named by
// the mangled symbols; the modal record, the stack view and the result
// meanings are inferred. The rest of the file is in other units.
struct UISParam_t;
struct UISControlInfo_t;

struct UISScreen_t {
    unsigned short group;
    unsigned short id;
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

struct UISInfo_t {
    char field00[36];
    unsigned long currentScreen;
    char field28[4];
    unsigned long screenCount;
    char field30[32];
    long modalCount;
    UISModal_t* modals;
};

unsigned long UISFindScreen(UISInfo_t*, unsigned short, unsigned short);
signed char UISStackProcess(UISInfo_t*, UISParam_t*, UISStackInfo_t*, UISScreen_t*, UISControlInfo_t*);

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
