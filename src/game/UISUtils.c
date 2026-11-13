// UISUtils.c: UI studio utilities. The file's inlining is deferred, which
// emits functions in reverse source order, so they are defined here from the
// end of the image to its start.
// Lookups: a control's event and hint entries (8 bytes: flags with the
// event/hint kind in the top bits, an id and a code address) and a screen by
// its two ids; the file-local _UISFindHintPC comes first in the image.
// UISExecuteFnc pushes a result slot, the parameters and the control on the
// UIS stack and runs it from the given code pointer (it is called, not
// inlined: '#pragma auto_inline off' keeps it out of _ParseHints). The setters and getters
// of the additive and multiplier colour factors (four floats each; the
// result type of the getters is inferred), then UISGetActionPtrValue, which
// returns the address of the control value an action (16 to 39) changes, the
// value at +72 for the others, then UISStringFormat, which scans a counted
// format string for conversions ('%', s, f, c, d, x) and collects one
// argument word per conversion from the parameters (two for a float, widened
// to double; a null string stays 0), formats into a 512-byte static buffer
// and copies at most the output string's length into it, terminated.
// UISFindSiblingEnableControl finds the group of the screen's controls whose
// maps refer (through an action of type 0xffff) to the control, then returns
// the first control those maps refer to whose value at +4 is 1 (none: 0).
// UISMoveScreenDrawPosition moves a screen up or down the draw order by
// swapping it with its neighbour the given number of times (stopping at
// either end), keeping the current screen (+36) and the screen pointers of
// the records at +72 (52 bytes, +28) and the modal records at +84 (40 bytes,
// +24) on the same screens; UISFindScreen is inlined into it.
// The file-local, recursive _ParseHints runs a hint over a screen control:
// each sub-control link (id 0xffff) is parsed first, then the control's code
// for the hint (the first event with flag 0x4000 and the hint's id) is run
// with the value and parameters. The compiler inlines the recursion three
// levels deep and _UISFindHintPC into the upper levels; the deepest level
// calls it. UISDoHint runs the hint parser over every screen with the
// studio's busy flag (bit 1 of the flags at +4) set for the duration of each
// pass.
// UISInfo_t, UISScreen_t, UISScrData_t, UISControl_t, UISControlInfo_t,
// UISStackInfo_t, UISParam_t and UISString_t are named by the mangled
// symbols; their members (named by offset where the meaning is unknown), the
// control list, map, action and record views, the inline helpers and the
// hint meanings are inferred.
extern "C" {
void* memcpy(void*, const void*, unsigned long);
int sprintf(char*, const char*, ...);
char* strncpy(char*, const char*, unsigned long);
}

/* inferred: a counted string (the length at +4, the text at +8) */
struct UISString_t {
    unsigned long field00;
    unsigned long length;
    char* text;
};

/* inferred: a 4-byte parameter read as a word, a float or a string */
union UISStringParamView {
    unsigned long value;
    float number;
    UISString_t* string;
};

struct UISControlInfo_t {
    char field00[4];
    unsigned long enable;
    float value08;
    float value0C;
    float value10;
    float value14;
    float value18;
    float value1C;
    float value20;
    float value24;
    float value28;
    float value2C;
    float value30;
    float value34;
    char field38[4];
    float value3C;
    float value40;
    float value44;
    float value48;
    float value4C;
    float value50;
    float value54;
    float value58;
};

struct UISEventEntry_t {
    unsigned short flags;
    unsigned short id;
    void* pc;
};

struct UISMapAction_t {
    unsigned short type;
    unsigned long control;
};

struct UISMap_t {
    unsigned long* condition;
    unsigned long count;
    UISMapAction_t* actions;
};

/* a screen's control: its maps (the sibling search's view) and its events */
struct UISControl_t {
    UISControlInfo_t* control;
    unsigned long count;
    UISMap_t** maps;
    unsigned long eventCount;
    UISEventEntry_t* events;
};
typedef UISControl_t UISControlEntry_t;

struct UISScrData_t {
    unsigned long controlCount;
    UISControl_t* controls;
};

/* the sibling search's view of the same list */
struct UISControlList_t {
    unsigned long count;
    UISControlEntry_t* entries;
};

struct UISScreen_t {
    unsigned short group;
    unsigned short id;
    char field04[4];
    union {
        UISScrData_t* data;
        UISControlList_t* controls;
    };
};

struct UISRecord48_t {
    char field00[28];
    UISScreen_t* screen;
    char field20[20];
};

struct UISModal_t {
    char field00[24];
    UISScreen_t* from;
    char field1C[12];
};

struct UISInfo_t {
    char field00[4];
    unsigned long flags;
    char field08[28];
    unsigned long currentScreen;
    char field28[4];
    unsigned long screenCount;
    UISScreen_t* screens;
    char field34[16];
    unsigned long count44;
    UISRecord48_t* records;
    char field4C[4];
    unsigned long modalCount;
    UISModal_t* modals;
    char stack[4];
};

struct UISParam_t {
    unsigned long value;
};

/* inferred: the stack state (stack pointer at +12, code pointer at +16) */
struct UISStackInfo_t {
    unsigned long value00;
    unsigned long value04;
    unsigned long value08;
    UISParam_t* sp;
    void* pc;
};

static float _MultiplerColorFactor[4];
static float _AdditiveColorFactor[4];

signed char UISStackProcess(UISInfo_t*, UISParam_t*, UISStackInfo_t*, UISScreen_t*, UISControlInfo_t*);
signed char UISExecuteFnc(UISInfo_t*, UISScreen_t*, UISControlInfo_t*, UISStackInfo_t*, void*, unsigned long,
                          UISParam_t*, unsigned long, UISParam_t*, unsigned char, long, UISParam_t*);
static void _ParseHints(UISInfo_t*, UISScreen_t*, UISStackInfo_t*, unsigned long, unsigned long, unsigned long,
                        UISParam_t*);

unsigned long UISFindScreen(UISInfo_t* info, unsigned short group, unsigned short id) {
    unsigned long i;

    for (i = 0; i < info->screenCount; i++) {
        UISScreen_t* screen = &info->screens[i];

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

inline void Push(UISStackInfo_t* stack, unsigned long value) {
    stack->sp->value = value;
    stack->sp++;
}

#pragma auto_inline off
signed char UISExecuteFnc(UISInfo_t* info, UISScreen_t* screen, UISControlInfo_t* control, UISStackInfo_t* stack,
                          void* pc, unsigned long count, UISParam_t* params, unsigned long count2,
                          UISParam_t* params2, unsigned char extra, long extraValue, UISParam_t* result) {
    UISParam_t* frame;
    UISScreen_t* const scr = screen;
    frame = stack->sp;
    if (!result)
        frame->value = 0;
    else
        frame->value = result->value;
    stack->sp++;
    if (extra)
        Push(stack, extraValue);
    for (unsigned long i = 0; i < count; i++)
        Push(stack, params[i].value);
    for (unsigned long i = 0; i < count2; i++)
        Push(stack, params2[i].value);
    Push(stack, (unsigned long)control);
    Push(stack, 0);
    stack->pc = pc;
    int status = UISStackProcess(info, frame, stack, scr, control);
    if (result)
        result->value = frame->value;
    if (status != 3)
        stack->sp = frame;
    return status;
}
#pragma auto_inline reset

float* UISGetActionPtrValue(unsigned long action, UISControlInfo_t* control) {
    switch (action) {
    case 16:
        return &control->value20;
    case 17:
        return &control->value24;
    case 18:
        return &control->value28;
    case 19:
        return &control->value08;
    case 20:
        return &control->value0C;
    case 21:
        return &control->value10;
    case 22:
        return &control->value2C;
    case 23:
        return &control->value30;
    case 24:
        return &control->value34;
    case 25:
        return &control->value14;
    case 26:
        return &control->value18;
    case 27:
        return &control->value1C;
    case 32:
        return &control->value58;
    case 33:
        return &control->value4C;
    case 34:
        return &control->value50;
    case 35:
        return &control->value54;
    case 36:
        return &control->value48;
    case 37:
        return &control->value3C;
    case 38:
        return &control->value40;
    case 39:
        return &control->value44;
    }
    return &control->value48;
}

extern "C" float* UISGetColorMultipler() {
    return _MultiplerColorFactor;
}

extern "C" float* UISGetColorAdditive() {
    return _AdditiveColorFactor;
}

void UISSetColorMultipler(float r, float g, float b, float a) {
    _MultiplerColorFactor[3] = a;
    _MultiplerColorFactor[0] = r;
    _MultiplerColorFactor[1] = g;
    _MultiplerColorFactor[2] = b;
}

void UISSetColorAdditive(float r, float g, float b, float a) {
    _AdditiveColorFactor[3] = a;
    _AdditiveColorFactor[0] = r;
    _AdditiveColorFactor[1] = g;
    _AdditiveColorFactor[2] = b;
}

void UISStringFormat(UISScrData_t* data, UISString_t* out, UISString_t* format, unsigned long count, UISParam_t* params) {
    static char Temp[512];
    unsigned long args[40] = { 0 };
    char* text;
    bool more;
    unsigned long length;
    unsigned long i;
    int n;
    int k;
    n = 0;
    k = 0;
    if (format) {
        text = format->text;
        length = format->length;
        for (i = 0; i < length; i++) {
            if (text[i] == '%') {
                more = true;
                while (more && i++ < length) {
                    switch (text[i]) {
                    case '%':
                        more = false;
                        break;
                    case 's':
                        if (((UISStringParamView*)params)[k].string)
                            args[n] = (unsigned long)((UISStringParamView*)params)[k].string->text;
                        more = false;
                        k++;
                        n++;
                        break;
                    case 'f':
                        *(double*)&args[n] = ((UISStringParamView*)params)[k].number;
                        k++;
                        more = false;
                        n += 2;
                        break;
                    case 'd':
                        args[n] = ((UISStringParamView*)params)[k].value;
                        n++;
                        more = false;
                        k++;
                        break;
                    case 'c':
                        args[n] = ((UISStringParamView*)params)[k].value;
                        n++;
                        more = false;
                        k++;
                        break;
                    case 'x':
                        args[n] = ((UISStringParamView*)params)[k].value;
                        n++;
                        more = false;
                        k++;
                        break;
                    }
                }
            }
        }
        if (out) {
            char* dest = out->text;
            unsigned long size = out->length;
            sprintf(Temp, text, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                    args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17],
                    args[18], args[19]);
            strncpy(dest, Temp, size);
            dest[size] = 0;
        }
    }
}

static inline unsigned char RefersTo(UISControlEntry_t* entries, UISControlEntry_t* entry,
                                     UISControlInfo_t* control) {
    for (unsigned long i = 0; i < entry->count; i++) {
        UISMap_t* map = entry->maps[i];
        if (*map->condition != 0) {
            for (unsigned long j = 0; j < map->count; j++) {
                UISMapAction_t* action = &map->actions[j];
                if (action->type == 0xffff && entries[action->control].control == control)
                    return 1;
            }
        }
    }
    return 0;
}

static inline UISControlInfo_t* FindEnabled(UISControlEntry_t* entries, UISControlEntry_t* entry) {
    for (unsigned long i = 0; i < entry->count; i++) {
        UISMap_t* map = entry->maps[i];
        if (*map->condition != 0) {
            for (unsigned long j = 0; j < map->count; j++) {
                UISMapAction_t* action = &map->actions[j];
                if (action->type == 0xffff && entries[action->control].control->enable == 1)
                    return entries[action->control].control;
            }
        }
    }
    return 0;
}

UISControlInfo_t* UISFindSiblingEnableControl(UISScreen_t* screen, UISControlInfo_t* control) {
    unsigned long count;
    UISControlList_t* list = screen->controls;
    count = list->count;
    for (unsigned long i = 0; i < count; i++) {
        UISControlEntry_t* entries = list->entries;
        UISControlEntry_t* entry = &entries[i];
        if (RefersTo(entries, entry, control))
            return FindEnabled(entries, entry);
    }
    return 0;
}

void UISMoveScreenDrawPosition(UISInfo_t* info, unsigned short group, unsigned short id, long delta) {
    long count;
    long index = UISFindScreen(info, group, id);
    count = info->screenCount;
    if (index < count) {
        long steps;
        long current;
        long step;
        if (delta >= 0) {
            steps = delta;
            step = 1;
        } else if (delta < 0) {
            steps = -delta;
            step = -1;
        }
        current = index;
        while (steps-- != 0) {
            long previous = current;
            current += step;
            if (current >= count || current < 0)
                break;
            if (info->currentScreen == current)
                info->currentScreen = previous;
            else if (info->currentScreen == previous)
                info->currentScreen = current;
            for (unsigned long i = 0; i < info->count44; i++) {
                if (info->records[i].screen == &info->screens[current])
                    info->records[i].screen = &info->screens[previous];
                else if (info->records[i].screen == &info->screens[previous])
                    info->records[i].screen = &info->screens[current];
            }
            for (unsigned long j = 0; j < info->modalCount; j++) {
                if (info->modals[j].from == &info->screens[current])
                    info->modals[j].from = &info->screens[previous];
                else if (info->modals[j].from == &info->screens[previous])
                    info->modals[j].from = &info->screens[current];
            }
            UISScreen_t swap;
            memcpy(&swap, &info->screens[current], sizeof(UISScreen_t));
            memcpy(&info->screens[current], &info->screens[previous], sizeof(UISScreen_t));
            memcpy(&info->screens[previous], &swap, sizeof(UISScreen_t));
        }
    }
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

void UISDoHint(UISInfo_t* info, unsigned long hint, unsigned long value, UISParam_t* params) {
    unsigned long i;
    unsigned long count = info->screenCount;
    for (i = 0; i < count; i++) {
        UISScreen_t* screen = &info->screens[i];
        info->flags |= 2;
        _ParseHints(info, screen, (UISStackInfo_t*)info->stack, 0, hint, value, params);
        info->flags &= ~2;
    }
}
