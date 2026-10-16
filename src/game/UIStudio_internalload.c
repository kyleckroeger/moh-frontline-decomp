// A fragment of UIStudio.c (0x800887dc): the file-local _UISInternalLoad
// loads a screen unless it is loaded already (0): the current screen (none
// when there is no screen) is passed as the one it is opened from; it is
// loaded as a modal one when asked or when modal screens are open, which
// first appends a cleared 40-byte modal record for it; after
// UISInternalLoadScreen the record takes the loaded screen's first control
// and the screen it was opened from, or is dropped again when the screen did
// not load; the load result is returned. UISInfo_t, UISScreen_t,
// UISScrData_t, UISStackInfo_t, UISParam_t, UISTransformAction and the
// functions are named by the symbols; the views follow UIStudio_idle.c and
// UISUtils_siblingenable.c, and the modal record view and the meanings of
// the values and results are inferred. It is a unit of its own: in the
// loadscreen unit the deferred inliner would inline UISInternalLoadScreen.
// The rest of the file is in other units.
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

/* inferred: a modal record (40 bytes), as in UIStudio_unloadmodal.c */
struct UISModal_t {
    char field00[20];
    UISControlInfo_t* control;
    UISScreen_t* from;
    char field1C[8];
    unsigned short id;
    unsigned short group;
};

struct UISInfo_t {
    char field00[4];
    unsigned long flags;
    char field08[8];
    UISLoadFnc_t load;
    char field14[16];
    unsigned long currentScreen;
    char field28[4];
    unsigned long screenCount;
    UISScreen_t* screens;
    char field34[28];
    unsigned long modalCount;
    UISModal_t* modals;
    char stack[4];
};

extern "C" void* memset(void*, int, unsigned long);
unsigned long UISFindScreen(UISInfo_t*, unsigned short, unsigned short);
int PatchScrData(UISScrData_t*);
void _ParseInitialize(UISInfo_t*, UISScreen_t*, unsigned long, unsigned long);
void _ParseTransforms(UISInfo_t*, UISTransformAction, UISScreen_t*, unsigned long);
void _ParseMaps(UISInfo_t*, UISScreen_t*, UISStackInfo_t*, unsigned long, unsigned long, unsigned long,
                unsigned long, UISParam_t*, signed char*);

unsigned char UISInternalLoadScreen(UISInfo_t*, unsigned short, unsigned short, unsigned short, unsigned short,
                                    unsigned char, UISParam_t*);

static unsigned char _UISInternalLoad(UISInfo_t* info, unsigned short group, unsigned short id, unsigned char modal,
                                      unsigned char flag, UISParam_t* param) {
    unsigned char result;
    short fromGroup;
    short fromId;
    unsigned long slot = 0;
    if (info->screenCount == 0 || info->currentScreen == 0xffffffff) {
        fromGroup = -1;
        fromId = -1;
    } else {
        UISScreen_t* current = &info->screens[info->currentScreen];
        fromGroup = current->group;
        fromId = current->id;
    }
    unsigned char loaded = UISFindScreen(info, group, id) < info->screenCount;
    if (loaded == 1)
        return 0;
    if (info->modalCount && !loaded)
        modal = 1;
    UISModal_t* record;
    if (modal) {
        slot = info->modalCount;
        record = &info->modals[slot];
        memset(record, 0, sizeof(UISModal_t));
        record->group = group;
        record->id = id;
        info->modalCount++;
    }
    result = UISInternalLoadScreen(info, group, id, fromGroup, fromId, flag, param);
    if (modal) {
        long index = UISFindScreen(info, group, id);
        if (index < (long)info->screenCount) {
            record = &info->modals[slot];
            UISScreen_t* loaded = &info->screens[index];
            record->control = loaded->data->entries->control;
            index = UISFindScreen(info, fromGroup, fromId);
            if (index < (long)info->screenCount)
                record->from = &info->screens[index];
        } else {
            info->modalCount--;
        }
    }
    return result;
}
