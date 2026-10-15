// A fragment of UIStudio.c (0x800889a0): UISInternalLoadScreen loads a screen
// that is not loaded yet through the studio's load function (+16), appends
// it (its ids, two values and the patched screen data), runs its
// initialisers, enables its first control, sends it internal event -9 (when
// patching reported so) and -2 (with the given flag, or the parameter's
// byte at +40 when 0xff, and the parameter), and returns 1 (0 when it was
// loaded already or could not be loaded). UISInfo_t, UISScreen_t,
// UISScrData_t, UISStackInfo_t, UISParam_t, UISTransformAction and the
// functions are named by the symbols; the views follow UIStudio_idle.c and
// UISUtils_siblingenable.c, _ParseInitialize is file-local in the original
// and declared without static here, and the meanings of the values, events
// and results are inferred. The rest of the file is in other units.
struct UISStackInfo_t;
enum UISTransformAction {};

struct UISControlInfo_t {
    unsigned long active;
    unsigned long enable;
};

struct UISControlEntry_t {
    UISControlInfo_t* control;
};

struct UISScrData_t {
    unsigned long count;
    UISControlEntry_t* entries;
};

struct UISScreen_t {
    unsigned short group;
    unsigned short id;
    unsigned short value04;
    unsigned short value06;
    UISScrData_t* data;
};

/* inferred: the parameter's flag byte at +40 */
struct UISParam_t {
    char field00[40];
    unsigned long flag;
};

typedef UISScrData_t* (*UISLoadFnc_t)(unsigned short, unsigned short);

struct UISInfo_t {
    char field00[4];
    unsigned long flags;
    char field08[8];
    UISLoadFnc_t load;
    char field14[24];
    unsigned long screenCount;
    UISScreen_t* screens;
    char field34[36];
    char stack[4];
};

unsigned long UISFindScreen(UISInfo_t*, unsigned short, unsigned short);
int PatchScrData(UISScrData_t*);
void _ParseInitialize(UISInfo_t*, UISScreen_t*, unsigned long, unsigned long);
void _ParseTransforms(UISInfo_t*, UISTransformAction, UISScreen_t*, unsigned long);
void _ParseMaps(UISInfo_t*, UISScreen_t*, UISStackInfo_t*, unsigned long, unsigned long, unsigned long,
                unsigned long, UISParam_t*, signed char*);

unsigned char UISInternalLoadScreen(UISInfo_t* info, unsigned short group, unsigned short id, unsigned short value04,
                                    unsigned short value06, unsigned char flag, UISParam_t* param) {
    unsigned long index = UISFindScreen(info, group, id);
    if (index < info->screenCount)
        return 0;
    UISScrData_t* data = info->load(group, id);
    if (!data)
        return 0;
    info->screenCount++;
    UISScreen_t* screen = &info->screens[index];
    screen->group = group;
    screen->id = id;
    screen->value04 = value04;
    screen->value06 = value06;
    screen->data = data;
    int patched = PatchScrData(screen->data);
    _ParseInitialize(info, screen, 0, -1);
    screen->data->entries->control->enable = 1;
    signed char result;
    if (patched) {
        result = 0;
        info->flags |= 2;
        _ParseMaps(info, screen, (UISStackInfo_t*)info->stack, 0, -1, -9, 0, 0, &result);
        info->flags &= ~2;
    }
    _ParseTransforms(info, (UISTransformAction)0, screen, 0);
    result = 0;
    if (flag == 0xff)
        flag = param->flag;
    info->flags |= 2;
    _ParseMaps(info, screen, (UISStackInfo_t*)info->stack, 0, -1, -2, flag, param, &result);
    info->flags &= ~2;
    return 1;
}
