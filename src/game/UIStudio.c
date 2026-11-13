// UIStudio.c: the UI studio (screens, controls, maps, initialisers, rate
// functions, modal screens and thread actions). Like the other UIS files it
// is compiled with deferred inlining, which emits the functions in reverse
// source order, so they are defined here from the end of the image to its
// start. UISInfo_t, UISScreen_t, UISScrData_t, UISControl_t,
// UISControlInfo_t, UISControlList_t, UISStackInfo_t, UISParam_t,
// UISThreadGroupInfoT, UISThreadActionT and UISTransformAction are named by
// the mangled symbols; their members (named by offset where the meaning is
// unknown), the other views and the parameter meanings are inferred. Where
// the functions read one slot in two ways, the views keep both names in an
// anonymous union. The loading path (UISInternalLoadScreen, _UISInternalLoad
// and UISLoadScreen) returns int: none of the three extends the result.
extern "C" {
void* memset(void*, int, unsigned long);
void* memmove(void*, const void*, unsigned long);
}

typedef void (*UISCallback)(void);
typedef void (*UISErrorFnc_t)(void);
typedef void (*UISPluginFnc_t)(unsigned long, unsigned long, int, int, int);
typedef void (*UISFunction_t)(unsigned long, unsigned long, unsigned long, unsigned long, unsigned long);
/* the visibility parser passes the wanted state by address */
typedef void (*UISVisFunction_t)(unsigned long, unsigned long, unsigned long, unsigned long*, unsigned long);

extern UISErrorFnc_t RuntimeErrorFnc;
extern signed char uisEventNotProcess;

enum UISThreadActionT { UISThreadActionUnknown = 0 };
enum UISTransformAction {};

/* inferred: a thread action's group record: the screen's group and id (or
   halfwords) and three values */
union UISThreadGroupInfoT {
    struct {
        unsigned short group;
        unsigned short thread;
        long value04;
        long value08;
        long value0c;
    } s;
    struct {
        unsigned short h0;
        unsigned short h2;
        unsigned short h4;
        unsigned short h6;
    } h;
};

/* inferred: the control state, active at +0 and enabled at +4 */
struct UISControlInfo_t {
    unsigned long active;
    unsigned long enable;
    char field08[88];
    unsigned long field60;
};

/* inferred: the control state's colour scales and offsets */
struct UISControlColorView {
    unsigned long active;
    char field04[4];
    char rect[52];
    float multiply[4];
    float add[4];
};

/* a control's event entry: flags (the event/hint kind in the top bits), an
   id and a code address */
struct UISEventEntry_t {
    unsigned short flags;
    unsigned short id;
    void* pc;
};

struct UISEventRefView {
    unsigned short id;
    short state;
    unsigned long pc;
};

struct UISSubControlView {
    unsigned long control;
    unsigned long eventCount;
    UISEventRefView* events;
};

struct UISEventLinkView {
    unsigned short value00;
    unsigned short id;
    unsigned long pc;
};

/* inferred: an action or initialiser: its function (0xffff: a sub-control
   link), state and argument */
struct UISMapAction_t {
    unsigned short type;
    short state;
    unsigned long data;
};

struct UISMap_t {
    unsigned long* condition;
    unsigned long count;
    UISMapAction_t* actions;
};

/* a screen control: its state, its maps (or sub-control tables) and its
   events */
struct UISControl_t {
    UISControlInfo_t* control;
    unsigned long count;
    union {
        UISMap_t** maps;
        UISSubControlView** subs;
    };
    unsigned long eventCount;
    union {
        UISEventEntry_t* events;
        UISEventLinkView* links;
    };
};
typedef UISControl_t UISControlEntry_t;

/* the screen data block before PatchScrData: offsets in place of pointers */
struct UISControlEntryView {
    unsigned long control;
    unsigned long subCount;
    UISSubControlView** subs;
    unsigned long eventCount;
    UISEventLinkView* events;
};

struct UISMapView {
    unsigned long value00;
    unsigned long value04;
    unsigned long data;
};

struct UISObjectView {
    unsigned long value00;
    unsigned long data;
};

/* inferred: an object record's plugin index */
struct UISObjectIndexView {
    unsigned short plugin;
    unsigned short value02;
    unsigned long data;
};

struct UISScrData_t {
    unsigned long controlCount;
    union {
        UISControlEntryView* controls;
        UISControl_t* entries;
    };
    unsigned long mapCount;
    UISMapView* maps;
    unsigned long fixupCount;
    unsigned long* fixups;
    unsigned long objectCount;
    UISObjectView* objects;
};

/* the visibility and initialiser parsers' view of the screen data */
struct UISControlList_t {
    unsigned long count;
    UISControlEntry_t* entries;
    char field08[16];
    unsigned long initCount;
    UISMapAction_t* inits;
};

/* a loaded screen (12 bytes): its group and id, the group and id of the
   screen it was opened from, and its data */
struct UISScreen_t {
    unsigned short group;
    unsigned short id;
    unsigned short parentGroup;
    unsigned short parentId;
    union {
        UISScrData_t* data;
        UISControlList_t* controls;
    };
};

struct UISParam_t {
    unsigned long value;
};

/* inferred: a word stack's state (the stack pointer at +12) */
struct UISStackInfo_t {
    unsigned long* value00;
    unsigned long* value04;
    unsigned long* value08;
    unsigned long* sp;
    unsigned long value10;
};

/* a modal screen record (40 bytes): the saved stack state, the control and
   screen it was opened from, the parameters and stack to resume, its id
   and group */
struct UISModal_t {
    UISStackInfo_t saved;
    UISControlInfo_t* control;
    UISScreen_t* from;
    UISParam_t* param;
    UISStackInfo_t* stack;
    unsigned short id;
    unsigned short group;
};

/* inferred: a rate function record (52 bytes) */
struct UISRateFnc_t {
    unsigned long id;
    void* pc;
    long time;
    long last;
    unsigned long period;
    unsigned long state;
    UISControlInfo_t* control;
    UISScreen_t* screen;
    unsigned long action;
    float target;
    float rate;
    void* donePc;
    UISControlInfo_t* actionControl;
};

/* the unload path's view of a rate function record: its screen */
struct UISScreenRefView {
    char field00[20];
    unsigned long dirty;
    char field18[4];
    UISScreen_t* screen;
    char field20[20];
};

typedef UISScrData_t* (*UISLoadFnc_t)(unsigned short, unsigned short);

/* the studio record (172 bytes, 'UIS_'): callbacks, the current screens,
   the screen, plugin, rate function and modal tables (capacity, count,
   table) and two word stacks; members named by offset are inferred */
struct UISInfo_t {
    unsigned long magic;
    unsigned long flags;
    unsigned long rate;
    union {
        unsigned long count0C;
        UISCallback message;
    };
    union {
        unsigned long count10;
        UISCallback resourceLoad;
        UISLoadFnc_t load;
    };
    union {
        unsigned long count14;
        UISCallback resourceFree;
        void (*unload)(unsigned short, unsigned short, void*);
    };
    union {
        void (*draw)(UISTransformAction, void*);
        UISCallback transform;
    };
    unsigned long count1C;
    union {
        long screen20;
        unsigned long eventMask;
    };
    union {
        long screen24;
        unsigned long currentScreen;
    };
    unsigned long screenCapacity;
    unsigned long screenCount;
    UISScreen_t* screens;
    unsigned long capacity34;
    union {
        unsigned long count38;
        unsigned long pluginCount;
        unsigned long functionCount;
    };
    union {
        void* table3C;
        void** plugins;
        UISFunction_t* functions;
        UISVisFunction_t* visFunctions;
    };
    union {
        unsigned long capacity40;
        long refCount;
    };
    union {
        unsigned long count44;
        long dirtyRefCount;
    };
    union {
        void* table48;
        UISScreenRefView* refs;
    };
    unsigned long capacity4C;
    unsigned long modalCount;
    union {
        void* table54;
        UISModal_t* modals;
    };
    UISStackInfo_t stack;
    UISStackInfo_t stack2;
    char field80[32];
    unsigned long* stackA0;
    unsigned long* stackA4;
    unsigned long** stackA8;
};

int UISExecuteFnc(UISInfo_t*, UISScreen_t*, UISControlInfo_t*, UISStackInfo_t*, void*, unsigned long, UISParam_t*,
                  unsigned long, UISParam_t*, unsigned char, long, UISParam_t*);
signed char UISStackProcess(UISInfo_t*, UISParam_t*, UISStackInfo_t*, UISScreen_t*, UISControlInfo_t*);
unsigned long UISFindScreen(UISInfo_t*, unsigned short, unsigned short);
void* UISFindEventPC(UISControl_t*, unsigned long);
void* UISFindSubControlEventPC(UISControl_t*, unsigned short, unsigned long);
UISControlInfo_t* UISFindSiblingEnableControl(UISScreen_t*, UISControlInfo_t*);
float* UISGetActionPtrValue(unsigned long, UISControlInfo_t*);
extern "C" float* UISGetColorMultipler(void);
extern "C" float* UISGetColorAdditive(void);
void UISSetColorMultipler(float, float, float, float);
void UISSetColorAdditive(float, float, float, float);
void UISRemoveUnNessaryRateFncs(UISInfo_t*);
void UISAddThreadAction(UISInfo_t*, UISThreadActionT, UISThreadGroupInfoT*, long, UISParam_t*);
void UISProcessThreadAction(UISInfo_t*, unsigned char);
void UISProcessInternalEvents(UISInfo_t*, UISStackInfo_t*, long, unsigned long, unsigned long, UISParam_t*,
                              unsigned char);
int UISInternalLoadScreen(UISInfo_t*, unsigned short, unsigned short, unsigned short, unsigned short, unsigned char,
                          UISParam_t*);
int PatchScrData(UISScrData_t*);
static long _DetermineVisibility(UISScreen_t*, void**, unsigned long, void*);
static void _ParseVisibility(UISInfo_t*, UISScreen_t*, unsigned long, unsigned long, void*, unsigned long);
static void _ParseInitialize(UISInfo_t*, UISScreen_t*, unsigned long, unsigned long);
static int _ParseMaps(UISInfo_t*, UISScreen_t*, UISStackInfo_t*, unsigned long, unsigned long, unsigned long,
                      unsigned long, UISParam_t*, signed char*);
static void _ParseObjects(UISInfo_t*, UISScreen_t*, unsigned long, unsigned long);
static void _ParseTransforms(UISInfo_t*, UISTransformAction, UISScreen_t*, unsigned long);
static void _ParseRateFncs(UISInfo_t*, unsigned long);
static int _UISInternalLoad(UISInfo_t*, unsigned short, unsigned short, unsigned char, unsigned char, UISParam_t*);


// The file-local _ParseInitialize runs a screen's initialisers: each
// screen-level one not yet run (its state at +2 is 0) is called through the
// studio's function table (+60, its size at +56) and marked run; then every
// action of the control's maps either recurses into the control it refers to
// (type 0xffff) or is called through the table, its state set to whether the
// value is -1. UISInfo_t and UISScreen_t are named by the mangled symbols; the
// views follow UISUtils_siblingenable.c and the initialiser, function-table and
// parameter meanings, the hoisted declarations and the reuse of one counter for
// the initialiser and action loops are inferred.
static void _ParseInitialize(UISInfo_t* info, UISScreen_t* screen, unsigned long index, unsigned long value) {
    UISMapAction_t* action;
    UISControlEntry_t* entry;
    unsigned long j;
    UISMap_t* map;
    UISMapAction_t* init;
    unsigned long i;
    short state;
    if (!screen->controls)
        return;
    entry = &screen->controls->entries[index];
    state = value == 0xffffffff;
    for (j = 0; j < screen->controls->initCount; j++) {
        init = &screen->controls->inits[j];
        if (init->state == 0) {
            if (init->type < info->functionCount && info->functions[init->type])
                info->functions[init->type](init->data, value, 0, 0, 0);
            init->state = 1;
        }
    }
    for (i = 0; i < entry->count; i++) {
        map = entry->maps[i];
        for (j = 0; j < map->count; j++) {
            action = &map->actions[j];
            if (action->type == 0xffff) {
                _ParseInitialize(info, screen, action->data, value);
            } else if (action->type < info->functionCount && info->functions[action->type]) {
                info->functions[action->type](action->data, value, 0, 0, 0);
                action->state = state;
            }
        }
    }
}

// The file-local _DetermineVisibility searches a screen's control tree for the
// node held in *found, returning its visibility (the value its first word
// points to; 0 without one) and storing the node that holds it, or -1. Level 10
// is the screen (its first control may be the node, else the search continues
// there), level 8 a control (its maps are searched when it is visible) and
// level 7 a map (the controls its 0xffff actions refer to are searched when its
// condition holds). UISScreen_t is named by the mangled symbol; the views
// follow UISUtils_siblingenable.c and the level meanings are inferred (the
// found pointer and the result are cleared in one assignment, as the code
// shares the zero).
static long _DetermineVisibility(UISScreen_t* screen, void** found, unsigned long level, void* node) {
    void* target;
    if (!found || !(target = *found) || !node || !screen)
        return -1;
    switch (level) {
    case 10: {
        UISControlList_t* list = screen->controls;
        if (list && list->entries->control == target) {
            UISControlInfo_t* control;
            long result;
            *found = (void*)(result = 0);
            control = screen->controls->entries->control;
            if (control)
                result = control->active;
            return result;
        }
        node = list->entries;
    }
    case 8: {
        UISControlEntry_t* entry = (UISControlEntry_t*)node;
        if (entry->control == target) {
            *found = entry;
            return entry->control ? entry->control->active : 0;
        }
        if (entry->control && entry->control->active) {
            for (unsigned long i = 0; i < entry->count; i++) {
                long result = _DetermineVisibility(screen, found, 7, entry->maps[i]);
                if (result != -1)
                    return result;
            }
        }
        break;
    }
    case 7: {
        UISMap_t* map = (UISMap_t*)node;
        if (map->condition == target) {
            *found = map;
            return map->condition ? *map->condition : 0;
        }
        if (map->condition && *map->condition) {
            for (unsigned long i = 0; i < map->count; i++) {
                UISMapAction_t* action = &map->actions[i];
                if (action && action->type == 0xffff) {
                    long result = _DetermineVisibility(screen, found, 8, &screen->controls->entries[action->data]);
                    if (result != -1)
                        return result;
                }
            }
        }
        break;
    }
    }
    return -1;
}

// The file-local _ParseVisibility walks a screen's control tree (the levels as
// in _DetermineVisibility) setting the wanted visibility: a node already so,
// or hidden below the top (depth above 0), stops the walk; controls recurse
// into their maps and maps into the controls their 0xffff actions refer to,
// other actions being called through the studio's function table (+60, its size
// at +56) with event -4 and the wanted value. UISInfo_t and UISScreen_t are
// named by the mangled symbols; the views follow _DetermineVisibility, and
// the meanings are inferred (the comparisons with the wanted value are written
// as an exclusive or because the code tests one, as in UISUpdateVisibility).
static void _ParseVisibility(UISInfo_t* info, UISScreen_t* screen, unsigned long want, unsigned long level, void* node,
                             unsigned long depth) {
    if (!info || !screen || (!node && level != 10))
        return;
    switch (level) {
    case 10: {
        UISControlList_t* list = screen->controls;
        if (!list)
            return;
        UISControlEntry_t* first = list->entries;
        if (!first->control)
            return;
        unsigned long visible = first->control->active;
        if ((visible ^ want) == 0)
            return;
        if (visible != 0 || depth == 0)
            node = first;
        else
            return;
    }
    case 8: {
        UISControlEntry_t* entry = (UISControlEntry_t*)node;
        if (!entry->control)
            return;
        unsigned long visible = entry->control->active;
        if ((visible ^ want) == 0)
            return;
        if (visible == 0 && depth != 0)
            return;
        for (unsigned long i = 0; i < entry->count; i++)
            _ParseVisibility(info, screen, want, 7, entry->maps[i], depth + 1);
        break;
    }
    case 7: {
        UISMap_t* map = (UISMap_t*)node;
        if (!map->condition)
            return;
        unsigned long visible = *map->condition;
        if ((visible ^ want) == 0)
            return;
        if (visible == 0 && depth != 0)
            return;
        for (unsigned long i = 0; i < map->count; i++) {
            UISMapAction_t* action = &map->actions[i];
            if (action && action->type == 0xffff)
                _ParseVisibility(info, screen, want, 8, &screen->controls->entries[action->data], depth + 1);
            else if (action->type < info->functionCount && info->functions[action->type])
                info->visFunctions[action->type](action->data, -4, 1, &want, 0);
        }
        break;
    }
    }
}

// UISInit lays out a studio in one block: after its 172-byte header come the
// screens (12 bytes each), a table of pointers, records of 52 and of 40 bytes,
// and two word stacks; each table's capacity is kept with a zero count, the
// stacks' pointers are set up, the value given is stored, the current screens
// are none (-1), the counters are cleared and so is RuntimeErrorFnc. UISInfo_t
// is named by the mangled symbol, the magic is 'UIS_'; the members (named by
// offset) and the parameter meanings are inferred. UISGetMemSize returns the
// size of that block for the same counts. PatchScrData, after them, turns a
// loaded screen data block's offsets into pointers (once: it returns -1 when
// the control table already lies past the block): the controls, their
// sub-control tables, sub-controls and event code addresses (events with id
// 0xffff are skipped; patched sub-control events get their state cleared), the
// map table and the map data, the fixup fields (an index into the maps, or 0
// when out of range) and the object data. UISScrData_t is named by the mangled
// symbol; its views and the separate loop counters are inferred. The file-local
// _ParseRateFncs, after it, advances the running rate functions (52-byte
// records in the table at +72; with flag bit 2 set and the unneeded ones
// removed before and after) by the elapsed time: for each whole period passed
// it runs the function's code (with its id, the period start and, without an
// action, the period; the result scales the step) and, with an action, steps
// the action's value towards its target by the rate, stopping there (state 1)
// and running the completion code. The rate function view, the hoisted
// declarations and the state values are inferred. The file-local, recursive
// _ParseMaps, after that, runs a screen control's event: for a disabled control
// (or one without the flag at +96) only the events -11, -10..-8 and -5..-2 are
// processed; each sub-control link (id 0xffff) is parsed recursively and, when
// that leaves the sub-control enabled, its matching sub-control event code is
// run (2 ends the parse); then the queued thread actions are run and the
// control's own event code (for -6/-7 only when the parameters name this
// control); the control's enabled state is returned through the last argument.
// UISControl_t is named by the mangled symbols; its typed view (a second view
// of the patched control entry), the control state view and the event meanings
// are inferred. The file-local, recursive _ParseObjects, before _ParseMaps in
// the image, draws a screen control: it runs each screen object's plugin, and
// for an active control saves the current colour scale and offset (in arrays
// initialised to 1 and 0), combines them with the control's, calls the draw
// callback (+24, mode 1, with the control's rectangle), runs the plugins of its
// sub-controls' events (id 0xffff recurses), then restores the colours and
// calls the draw callback with mode 2. The plugin, object, colour views, the
// reuse of one counter for the object and event loops and the mixed
// direct/local reads of the control's colour scales (which the register use
// requires) are inferred. The file-local, recursive _ParseTransforms, after
// _ParseObjects in the image, passes a transform action to the draw callback
// for a control and, through its active sub-controls' sub-control links (id
// 0xffff), for each control below it (the compiler inlines the recursion three
// levels deep). The callback's parameter type is inferred from the calls.
static void _ParseTransforms(UISInfo_t* info, UISTransformAction action, UISScreen_t* screen, unsigned long index) {
    if (screen->data) {
        UISControl_t* control = &((UISControl_t*)screen->data->controls)[index];
        info->draw(action, ((UISControlColorView*)control->control)->rect);
        for (unsigned long j = 0; j < control->count; j++) {
            UISSubControlView* sub = control->subs[j];
            if (*(unsigned long*)sub->control) {
                for (unsigned long k = 0; k < sub->eventCount; k++) {
                    UISEventRefView* event = &sub->events[k];
                    if (event->id == 0xffff)
                        _ParseTransforms(info, action, screen, event->pc);
                }
            }
        }
    }
}

static void _ParseObjects(UISInfo_t* info, UISScreen_t* screen, unsigned long index, unsigned long value) {
    UISControlColorView* color;
    UISControl_t* control;
    unsigned long j;
    unsigned long k;
    UISSubControlView* sub;
    float multiply[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    float add[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    if (screen->data) {
        control = &((UISControl_t*)screen->data->controls)[index];
        for (k = 0; k < screen->data->objectCount; k++) {
            UISObjectIndexView* object = &((UISObjectIndexView*)screen->data->objects)[k];
            if (object->plugin < info->count38) {
                UISPluginFnc_t fnc = ((UISPluginFnc_t*)info->table3C)[object->plugin];
                if (fnc)
                    fnc(object->data, value, 0, 0, 0);
            }
        }
        if (control->control->active) {
            float* current = UISGetColorMultipler();
            multiply[0] = current[0];
            multiply[1] = current[1];
            multiply[2] = current[2];
            multiply[3] = current[3];
            current = UISGetColorAdditive();
            add[0] = current[0];
            add[1] = current[1];
            add[2] = current[2];
            add[3] = current[3];
            color = (UISControlColorView*)control->control;
            UISSetColorMultipler(multiply[0] * ((UISControlColorView*)control->control)->multiply[0],
                                 multiply[1] * ((UISControlColorView*)control->control)->multiply[1],
                                 multiply[2] * color->multiply[2], multiply[3] * color->multiply[3]);
            UISSetColorAdditive(add[0] + color->add[0], add[1] + color->add[1], add[2] + color->add[2], add[3] + color->add[3]);
            info->draw((UISTransformAction)1, ((UISControlColorView*)control->control)->rect);
            for (j = 0; j < control->count; j++) {
                sub = control->subs[j];
                if (*(unsigned long*)sub->control) {
                    for (k = 0; k < sub->eventCount; k++) {
                        UISEventRefView* event = &sub->events[k];
                        if (event->id == 0xffff) {
                            _ParseObjects(info, screen, event->pc, value);
                        } else if (event->state && event->id < info->count38) {
                            UISPluginFnc_t fnc = ((UISPluginFnc_t*)info->table3C)[event->id];
                            if (fnc)
                                fnc(event->pc, value, 0, 0, 0);
                        }
                    }
                }
            }
            UISSetColorMultipler(multiply[0], multiply[1], multiply[2], multiply[3]);
            UISSetColorAdditive(add[0], add[1], add[2], add[3]);
            info->draw((UISTransformAction)2, ((UISControlColorView*)control->control)->rect);
        }
    }
}

static int _ParseMaps(UISInfo_t* info, UISScreen_t* screen, UISStackInfo_t* stack, unsigned long index,
                      unsigned long value, unsigned long event, unsigned long count, UISParam_t* params,
                      signed char* enabled) {
    UISControl_t* control;
    int result = 0;
    UISScrData_t* data = screen->data;
    if (!data || index >= data->controlCount)
        return 0;
    control = &((UISControl_t*)data->controls)[index];
    if (!control->control->enable || !control->control->field60) {
        if (!(event + 10 <= 2 || event + 5 <= 3 || event == (unsigned long)-11))
            goto done;
    }
    for (unsigned long i = 0; i < control->eventCount; i++) {
        UISEventLinkView* link = &control->links[i];
        if (link->id == 0xffff) {
            signed char sub = 0;
            result = _ParseMaps(info, screen, stack, link->pc, value, event, count, params, &sub);
            if (sub == 1) {
                void* pc = UISFindSubControlEventPC(control, link->pc, event);
                if (pc) {
                    int status = UISExecuteFnc(info, screen, control->control, stack, pc, count, params, 0, 0, 1, index, 0);
                    if (status == 2)
                        return status;
                }
            }
        }
    }
    UISProcessThreadAction(info, 1);
    {
        void* pc = UISFindEventPC(control, event);
        if (event == (unsigned long)-6 || event == (unsigned long)-7) {
            if ((unsigned long)control->control != params[2].value)
                pc = 0;
        }
        if (pc)
            result = UISExecuteFnc(info, screen, control->control, stack, pc, count, params, 0, 0, 1, value, 0);
    }
done:
    *enabled = control->control->enable;
    return result;
}

static void _ParseRateFncs(UISInfo_t* info, unsigned long elapsed) {
    UISRateFnc_t* rate;
    unsigned long i;
    unsigned long count;
    UISScreen_t* screen;
    UISStackInfo_t* stack;
    long remaining;
    UISParam_t params[3];
    float result;
    UISRemoveUnNessaryRateFncs(info);
    stack = &info->stack2;
    info->flags |= 4;
    count = info->count44;
    for (i = 0; i < count; i++) {
        rate = &((UISRateFnc_t*)info->table48)[i];
        if (rate->state == 2) {
            screen = rate->screen;
            if (rate->period != 0) {
                rate->time += elapsed;
                remaining = rate->time - rate->last;
                while (remaining >= (long)rate->period) {
                    float value;
                    remaining -= rate->period;
                    rate->last += rate->period;
                    if (rate->pc) {
                        if (rate->state != 2)
                            break;
                        result = 1.0f;
                        unsigned long n;
                        params[0].value = rate->id;
                        params[1].value = rate->last;
                        if (!rate->action) {
                            params[2].value = rate->period;
                            n = 3;
                        } else {
                            n = 2;
                        }
                        UISExecuteFnc(info, screen, rate->control, stack, rate->pc, n, params,
                                      0, 0, 0, -1, (UISParam_t*)&result);
                        value = result;
                    } else {
                        value = 1.0f;
                    }
                    if (rate->action) {
                        float* target = UISGetActionPtrValue(rate->action, rate->actionControl);
                        float step = rate->rate;
                        if (value < 0.0f)
                            value = 1.0f;
                        float next = step * value + *target;
                        if ((step > 0.0f && next < rate->target) || (step < 0.0f && next > rate->target)) {
                            *target = next;
                        } else {
                            *target = rate->target;
                            rate->state = 1;
                            if (rate->donePc) {
                                params[0].value = rate->id;
                                UISExecuteFnc(info, screen, rate->control, stack, rate->donePc,
                                              1, params, 0, 0, 0, -1, 0);
                            }
                        }
                    }
                }
            }
        }
    }
    info->flags &= ~4;
    UISRemoveUnNessaryRateFncs(info);
}

int PatchScrData(UISScrData_t* data) {
    if ((unsigned long)data->controls < (unsigned long)data) {
        data->controls = (UISControlEntryView*)((unsigned long)data + (unsigned long)data->controls);
        for (unsigned long i = 0; i < data->controlCount; i++) {
            UISControlEntryView* entry = &data->controls[i];
            entry->control = (unsigned long)data + entry->control;
            entry->subs = (UISSubControlView**)((unsigned long)data + (unsigned long)entry->subs);
            unsigned long n = entry->subCount;
            while (n-- != 0) {
                entry->subs[n] = (UISSubControlView*)((unsigned long)data + (unsigned long)entry->subs[n]);
                UISSubControlView* sub = entry->subs[n];
                sub->control = (unsigned long)data + sub->control;
                sub->events = (UISEventRefView*)((unsigned long)data + (unsigned long)sub->events);
                unsigned long m = sub->eventCount;
                while (m-- != 0) {
                    UISEventRefView* event = &sub->events[m];
                    if (event->id != 0xffff) {
                        event->state = 0;
                        event->pc = (unsigned long)data + event->pc;
                    }
                }
            }
            entry->events = (UISEventLinkView*)((unsigned long)data + (unsigned long)entry->events);
            unsigned long k = entry->eventCount;
            while (k-- != 0) {
                UISEventLinkView* link = &entry->events[k];
                if (link->id != 0xffff)
                    link->pc = (unsigned long)data + link->pc;
            }
        }
        data->maps = (UISMapView*)((unsigned long)data + (unsigned long)data->maps);
        unsigned long n = data->mapCount;
        while (n-- != 0)
            data->maps[n].data = (unsigned long)data + data->maps[n].data;
        data->fixups = (unsigned long*)((char*)data + (unsigned long)data->fixups);
        unsigned long f = data->fixupCount;
        while (f-- != 0) {
            unsigned long* field = (unsigned long*)((char*)data + data->fixups[f]);
            if (*field < data->mapCount)
                *field = (unsigned long)&data->maps[*field];
            else
                *field = 0;
        }
        data->objects = (UISObjectView*)((unsigned long)data + (unsigned long)data->objects);
        n = data->objectCount;
        while (n-- != 0) {
            if (data->objects[n].data)
                data->objects[n].data = (unsigned long)data + data->objects[n].data;
        }
        return 1;
    }
    return -1;
}

extern "C" unsigned long UISGetMemSize(unsigned long screens, unsigned long count34, unsigned long count40,
                                      unsigned long count4c, unsigned long stack1, unsigned long stack2) {
    return 172 + screens * 12 + count34 * 4 + count40 * 52 + count4c * 40 + (stack2 + stack1) * 4;
}

extern "C" void UISInit(UISInfo_t* info, unsigned long screens, unsigned long count34, unsigned long count40,
                        unsigned long count4C, unsigned long stack1, unsigned long stack2, unsigned long value) {
    unsigned long offset;
    info->magic = 0x5549535f;
    info->flags = 0;
    offset = sizeof(UISInfo_t);
    info->screenCapacity = screens;
    info->screenCount = 0;
    info->screens = (UISScreen_t*)((char*)info + offset);
    offset += screens * 12;
    info->capacity34 = count34;
    info->count38 = 0;
    info->table3C = (char*)info + offset;
    offset += count34 * 4;
    info->capacity40 = count40;
    info->count44 = 0;
    info->table48 = (char*)info + offset;
    offset += count40 * 52;
    info->capacity4C = count4C;
    info->modalCount = 0;
    info->table54 = (char*)info + offset;
    offset += count4C * 40;
    info->stack.value00 = info->stack.value04 = info->stack.sp = (unsigned long*)((char*)info + offset);
    offset += stack1 * 4;
    info->stack.value08 = info->stack.value04 + stack1;
    info->stackA0 = info->stack.value08 - 1;
    info->stackA4 = info->stack.value08 - 1;
    info->stackA8 = &info->stack.value04;
    info->stack2.value00 = info->stack2.value04 = info->stack2.sp = (unsigned long*)((char*)info + offset);
    offset += stack2 * 4;
    info->stack2.value08 = info->stack2.value04 + stack2;
    info->rate = value;
    info->screen24 = -1;
    info->count0C = 0;
    info->draw = 0;
    info->count1C = 0;
    info->count10 = 0;
    info->count14 = 0;
    RuntimeErrorFnc = 0;
    info->screen20 = -1;
}

// UI studio shutdown and the registration of the client's transform, resource
// and message callbacks in the studio record. UISShutdown stops every thread
// group, last first (a thread action 1 for the group's id and thread, processed
// at once), then clears the event flag and the studio's active word. The
// callback slots, the group table and the action record are inferred from their
// offsets in an inferred view of UISInfo_t, and the callback types are left
// generic.
extern "C" {
void UISShutdown(UISInfo_t* info) {
    for (int i = info->screenCount - 1; i >= 0; i--) {
        unsigned short id;
        unsigned short thread;
        UISScreen_t* entry;
        UISThreadGroupInfoT group;
        UISParam_t param;
        entry = &info->screens[i];
        thread = entry->id;
        id = entry->group;
        param.value = 0;
        group.s.group = id;
        group.s.thread = thread;
        UISAddThreadAction(info, (UISThreadActionT)1, &group, 1, &param);
        UISProcessThreadAction(info, 0);
    }
    uisEventNotProcess = 0;
    info->magic = 0;
}

void UISRegisterMessageFnc(UISInfo_t* info, UISCallback message) {
    info->message = message;
}

void UISRegisterResourceFncs(UISInfo_t* info, UISCallback load, UISCallback release) {
    info->resourceLoad = load;
    info->resourceFree = release;
}

void UISRegisterTransformFncs(UISInfo_t* info, UISCallback transform) {
    info->transform = transform;
}
}

// UISInternalLoadScreen loads a screen that is not loaded yet through the
// studio's load function (+16), appends it (its ids, two values and the patched
// screen data), runs its initialisers, enables its first control, sends it
// internal event -9 (when patching reported so) and -2 (with the given flag, or
// the parameter's byte at +40 when 0xff, and the parameter), and returns 1 (0
// when it was loaded already or could not be loaded). UISInfo_t, UISScreen_t,
// UISScrData_t, UISStackInfo_t, UISParam_t, UISTransformAction and the
// functions are named by the symbols; the views follow UISIdleProcess and
// UISUtils_siblingenable.c and the meanings of the values, events and results
// are inferred. UISRegisterPluginFnc stores a plugin function in the studio's
// plugin table (+60) at the given index and counts it (+56; the table's element
// type is inferred; the file's inlining is deferred, so it is defined before
// the function it follows).
extern "C" void UISRegisterPluginFnc(UISInfo_t* info, unsigned long index, void* function) {
    info->plugins[index] = function;
    info->pluginCount++;
}

int UISInternalLoadScreen(UISInfo_t* info, unsigned short group, unsigned short id, unsigned short value04,
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
    screen->parentGroup = value04;
    screen->parentId = value06;
    screen->data = data;
    int patched = PatchScrData(screen->data);
    _ParseInitialize(info, screen, 0, -1);
    screen->data->entries->control->enable = 1;
    signed char result;
    if (patched) {
        result = 0;
        info->flags |= 2;
        _ParseMaps(info, screen, &info->stack, 0, -1, -9, 0, 0, &result);
        info->flags &= ~2;
    }
    _ParseTransforms(info, (UISTransformAction)0, screen, 0);
    result = 0;
    if (flag == 0xff)
        flag = param[10].value;
    info->flags |= 2;
    _ParseMaps(info, screen, &info->stack, 0, -1, -2, flag, param, &result);
    info->flags &= ~2;
    return 1;
}

// The file-local _UISInternalLoad loads a screen unless it is loaded already
// (0): the current screen (none when there is no screen) is passed as the one
// it is opened from; it is loaded as a modal one when asked or when modal
// screens are open, which first appends a cleared 40-byte modal record for it;
// after UISInternalLoadScreen the record takes the loaded screen's first
// control and the screen it was opened from, or is dropped again when the
// screen did not load; the load result is returned. UISInfo_t, UISScreen_t,
// UISScrData_t, UISStackInfo_t, UISParam_t, UISTransformAction and the
// functions are named by the symbols; the views follow UISIdleProcess and
// UISUtils_siblingenable.c, and the modal record view and the meanings of the
// values and results are inferred.
static int _UISInternalLoad(UISInfo_t* info, unsigned short group, unsigned short id, unsigned char modal,
                                      unsigned char flag, UISParam_t* param) {
    int result;
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

// UISLoadScreen either queues thread action 0 with a group record holding the
// screen and resolution (the next two halfwords -1) when flag bit 1 of the
// studio record is set, returning 1, or loads the screen at once through
// _UISInternalLoad. UISInfo_t, UISThreadGroupInfoT, UISThreadActionT and
// UISParam_t are named by the mangled symbols; the views follow
// UISProcessEvent (the halfword view of the group record is inferred) and the
// parameter meanings are inferred.
extern "C" int UISLoadScreen(UISInfo_t* info, unsigned short screen, unsigned short resolution, unsigned char flag,
                             UISParam_t* params) {
    if (info->flags & 2) {
        UISThreadGroupInfoT group;

        group.h.h0 = screen;
        group.h.h2 = resolution;
        group.h.h4 = 0xffff;
        group.h.h6 = 0xffff;
        UISAddThreadAction(info, (UISThreadActionT)0, &group, flag, params);
    } else {
        return _UISInternalLoad(info, screen, resolution, 0, flag, params);
    }
    return 1;
}

// UISInternalUnloadScreen unloads a screen: it first pops its modal record (the
// file-local UISInternalUnloadModal after it refuses a screen that is not on
// top), clears the current screen if it is this one, gives the screen's
// children its own parent, marks the screen references dirty, parses the
// screen's maps (with flag bit 1 set), initialisation and transforms for
// unloading, calls the studio's unload callback with the screen's data, removes
// the 12-byte screen record (moving the later records down and fixing the modal
// and reference records that point at them, and the current screen index) and,
// when no screen is current, makes the parent (or the last screen) current
// through thread action 3. The file is compiled with deferred inlining, so the
// functions are listed in reverse image order. UISInfo_t, UISScreen_t,
// UISStackInfo_t, UISParam_t, UISControlInfo_t, UISThreadGroupInfoT,
// UISThreadActionT and UISTransformAction are named by the mangled symbols; the
// screen, modal, reference, studio and stack views, the hoisted declarations
// and the result meanings are inferred.
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
                    thread.s.group = current->group;
                    thread.s.thread = current->id;
                    UISAddThreadAction(info, (UISThreadActionT)3, &thread, 0, 0);
                }
            } else {
                info->currentScreen = -1;
            }
        }
    }
    return 1;
}

// . UISProcessEvent runs the pending thread actions, then, if the event is
// enabled in the studio's event mask, refreshes the active screen from the top
// of the screen stack and parses the event maps of either every screen or only
// the active one (none when no screen is active); uisEventNotProcess suppresses
// parsing unless it is 1 and the first event value is negative.
// UISSetScreenActive queues thread action 3 with a group record holding the two
// screen values (the rest of the record is left unset, no parameters) and runs
// the queued actions at once unless flag bit 1 of the studio record is set.
// UISInfo_t, UISThreadGroupInfoT, UISThreadActionT and UISParam_t are named by
// the mangled symbols; the views and the parameter meanings are inferred (see
// UISActionProcess.c).
extern "C" void UISSetScreenActive(UISInfo_t* info, unsigned short screen, unsigned short control) {
    UISThreadGroupInfoT group;

    group.s.group = screen;
    group.s.thread = control;
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
    long top = info->modalCount - 1;
    if (top >= 0)
        info->currentScreen = UISFindScreen(info, info->modals[top].group, info->modals[top].id);
    if (all) {
        end = info->screenCount;
        i = 0;
    } else {
        i = info->currentScreen;
        end = i + 1;
        if (i == (unsigned long)-1)
            return;
    }
    for (; i < end; i++) {
        UISScreen_t* screen = &info->screens[i];
        if ((uisEventNotProcess == 1 && (long)p1 < 0) || !uisEventNotProcess) {
            result = 0;
            info->flags |= 2;
            _ParseMaps(info, screen, &info->stack, 0, event, p1, p2, params, &result);
            info->flags &= ~2;
        }
    }
}

// UISProcessInternalEvents parses an event through the maps of every screen,
// or, unless the flag is set, only of the current screen (+36, none when -1).
// UISInfo_t, UISScreen_t, UISStackInfo_t and UISParam_t are named by the
// mangled symbols; the views follow UISIdleProcess and the parameter meanings
// are inferred.
void UISProcessInternalEvents(UISInfo_t* info, UISStackInfo_t* stack, long event, unsigned long a, unsigned long b,
                              UISParam_t* param, unsigned char all) {
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

// UISDrawObjects resets the colour multiplier to 1 and the additive colour to
// 0, runs the rate functions for the elapsed ticks (the value times the rate at
// +8), then parses the objects of every screen. UISInfo_t and UISScreen_t are
// named by the mangled symbols; the views follow UISIdleProcess and the
// meanings of the fixed arguments are inferred.
extern "C" void UISDrawObjects(UISInfo_t* info, unsigned long value) {
    unsigned long ticks = info->rate * value;
    UISSetColorMultipler(1.0f, 1.0f, 1.0f, 1.0f);
    UISSetColorAdditive(0.0f, 0.0f, 0.0f, 0.0f);
    _ParseRateFncs(info, ticks);
    unsigned long i;
    unsigned long count = info->screenCount;
    for (i = 0; i < count; i++)
        _ParseObjects(info, &info->screens[i], 0, -2);
}

// UISIdleProcess runs the queued thread actions, then the idle maps of every
// screen with the studio's busy flag (bit 1 of the flags at +4) set for each
// pass. UISInfo_t, UISScreen_t, UISStackInfo_t and UISParam_t are named by the
// mangled symbols; the views follow UISUtils_dohint.c and the parameter
// meanings (-10 the idle event) are inferred.
extern "C" void UISIdleProcess(UISInfo_t* info, unsigned long value) {
    UISProcessThreadAction(info, 0);
    long i;
    long count = info->screenCount;
    for (i = 0; i < count; i++) {
        UISScreen_t* screen = &info->screens[i];
        signed char result = 0;
        info->flags |= 2;
        _ParseMaps(info, screen, &info->stack, 0, value, -10, 0, 0, &result);
        info->flags &= ~2;
    }
}

// UISInternalActivateControl enables or disables (activate) a control of a
// screen: it finds the control's entry among the screen's 20-byte controls
// (from the last) and the last control with a plain sub-control event naming
// that index; when enabling, a sibling-enable control is first disabled through
// a recursive call; then it sets the control's enable value and runs the
// entry's activate (-6) or deactivate (-7) event and, for the sub-control, the
// matching sub-control event, passing the value. UISInfo_t, UISScreen_t,
// UISControlInfo_t, UISControlList_t, UISStackInfo_t, UISParam_t and the
// functions are named by the mangled symbols; the control, event and studio
// views, the hoisted declarations and the event meanings are inferred.
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
        for (long k = 0; k < (long)entry->eventCount; k++) {
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
        UISExecuteFnc(info, screen, found->control, &info->stack, pc, 0, 0, 0, 0, 1, value, 0);
    if (sub) {
        pc = UISFindSubControlEventPC(sub, i, event);
        if (pc)
            UISExecuteFnc(info, screen, sub->control, &info->stack, pc, 0, 0, 0, 0, 1, value, 0);
    }
}

// UISInternalActivateScreen sends the current screen internal event -5 (when no
// modal is open), then, when activating, makes the found screen current,
// enables its first control and sends it internal event -4; each event is sent
// with the studio's busy flag (bit 1 of +4) set. UISInfo_t, UISScreen_t,
// UISStackInfo_t, UISParam_t and UISControlInfo_t are named by the mangled
// symbols; the views follow UIStudio_internal.c, UISUtils_siblingenable.c and
// UISInternalUnloadModal, UISProcessInternalEvents's body follows
// UIStudio_internal.c (the file defers inlining; this unit marks it inline so
// that it is inlined without being emitted), and the event meanings are
// inferred.
void UISInternalActivateScreen(UISInfo_t* info, unsigned char activate, unsigned short group, unsigned short id) {
    if (info->modalCount == 0) {
        info->flags |= 2;
        UISProcessInternalEvents(info, &info->stack, 0, -5, 0, 0, 0);
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
            UISProcessInternalEvents(info, &info->stack, 0, -4, 0, 0, 0);
            info->flags &= ~2;
        }
    }
}

// UISUpdateVisibility determines a screen's current visibility and, when it is
// known and differs from the wanted one, parses the screen's visibility change.
// UISInfo_t and UISScreen_t are named by the mangled symbols; the parameter
// meanings are inferred, and the comparison is written as an exclusive or
// because the code tests one (xor.).
void UISUpdateVisibility(UISInfo_t* info, UISScreen_t* screen, unsigned long value, void* data, unsigned long show) {
    long visible = _DetermineVisibility(screen, &data, 10, screen);
    show = show != 0;
    if (visible != -1 && (visible ^ show) != 0)
        _ParseVisibility(info, screen, show, value, data, 0);
}
