// A fragment of UIStudio.c (0x80088c34): UISInit lays out a studio in one
// block: after its 172-byte header come the screens (12 bytes each), a table
// of pointers, records of 52 and of 40 bytes, and two word stacks; each
// table's capacity is kept with a zero count, the stacks' pointers are set
// up, the value given is stored, the current screens are none (-1), the
// counters are cleared and so is RuntimeErrorFnc. UISInfo_t is named by the
// mangled symbol, the magic is 'UIS_'; the members (named by offset) and
// the parameter meanings are inferred. UISGetMemSize returns the size of
// that block for the same counts (the file's inlining is deferred, so it is
// defined before the function it follows). PatchScrData, after them, turns
// a loaded screen data block's offsets into pointers (once: it returns -1
// when the control table already lies past the block): the controls, their
// sub-control tables, sub-controls and event code addresses (events with id
// 0xffff are skipped; patched sub-control events get their state cleared),
// the map table and the map data, the fixup fields (an index into the maps,
// or 0 when out of range) and the object data. UISScrData_t is named by the
// mangled symbol; its views and the separate loop counters are inferred.
// The file-local _ParseRateFncs, after it, advances the running rate
// functions (52-byte records in the table at +72; with flag bit 2 set and the
// unneeded ones removed before and after) by the elapsed time: for each
// whole period passed it runs the function's code (with its id, the period
// start and, without an action, the period; the result scales the step) and,
// with an action, steps the action's value towards its target by the rate,
// stopping there (state 1) and running the completion code. The rate
// function view, the hoisted declarations and the state values are inferred.
// The file-local, recursive _ParseMaps, after that, runs a screen control's
// event: for a disabled control (or one without the flag at +96) only the
// events -11, -10..-8 and -5..-2 are processed; each sub-control link (id
// 0xffff) is parsed recursively and, when that leaves the sub-control
// enabled, its matching sub-control event code is run (2 ends the parse);
// then the queued thread actions are run and the control's own event code
// (for -6/-7 only when the parameters name this control); the control's
// enabled state is returned through the last argument. UISControl_t is named
// by the mangled symbols; its typed view (a second view of the patched
// control entry), the control state view and the event meanings are inferred.
// The file-local, recursive _ParseObjects, before _ParseMaps in the image,
// draws a screen control: it runs each screen object's plugin, and for an
// active control saves the current colour scale and offset (in arrays
// initialised to 1 and 0), combines them with the control's, calls the draw
// callback (+24, mode 1, with the control's rectangle), runs the plugins of
// its sub-controls' events (id 0xffff recurses), then restores the colours
// and calls the draw callback with mode 2. The plugin, object, colour views,
// the reuse of one counter for the object and event loops and the mixed
// direct/local reads of the control's colour scales (which the register use
// requires) are inferred. The rest of the file is in other units.
typedef void (*UISErrorFnc_t)(void);

extern UISErrorFnc_t RuntimeErrorFnc;

struct UISInfo_t {
    unsigned long magic;
    unsigned long flags;
    unsigned long value08;
    unsigned long count0C;
    unsigned long count10;
    unsigned long count14;
    void (*draw)(int, void*);
    unsigned long count1C;
    long screen20;
    long screen24;
    unsigned long screenCapacity;
    unsigned long screenCount;
    void* screens;
    unsigned long capacity34;
    unsigned long count38;
    void* table3C;
    unsigned long capacity40;
    unsigned long count44;
    void* table48;
    unsigned long capacity4C;
    unsigned long count50;
    void* table54;
    unsigned long* stack58;
    unsigned long* stack5C;
    unsigned long* stack60;
    unsigned long* stack64;
    char field68[4];
    unsigned long* stack6C;
    unsigned long* stack70;
    unsigned long* stack74;
    unsigned long* stack78;
    char field7C[36];
    unsigned long* stackA0;
    unsigned long* stackA4;
    unsigned long** stackA8;
};

struct UISStackInfo_t;

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

struct UISScrData_t {
    unsigned long controlCount;
    UISControlEntryView* controls;
    unsigned long mapCount;
    UISMapView* maps;
    unsigned long fixupCount;
    unsigned long* fixups;
    unsigned long objectCount;
    UISObjectView* objects;
};

/* inferred: the control state, enabled at +4 */
struct UISControlInfo_t {
    unsigned long active;
    unsigned long enable;
    char field08[88];
    unsigned long field60;
};

/* inferred: the typed view of a screen control (UISControlEntryView above) */
struct UISControl_t {
    UISControlInfo_t* control;
    unsigned long subCount;
    UISSubControlView** subs;
    unsigned long eventCount;
    UISEventLinkView* events;
};

struct UISScreen_t {
    unsigned short group;
    unsigned short id;
    unsigned short parentGroup;
    unsigned short parentId;
    UISScrData_t* data;
};

struct UISParam_t {
    unsigned long value;
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

void UISRemoveUnNessaryRateFncs(UISInfo_t*);
float* UISGetActionPtrValue(unsigned long, UISControlInfo_t*);
int UISExecuteFnc(UISInfo_t*, UISScreen_t*, UISControlInfo_t*, UISStackInfo_t*, void*, unsigned long, UISParam_t*,
                   unsigned long, UISParam_t*, unsigned char, long, UISParam_t*);

typedef void (*UISPluginFnc_t)(unsigned long, unsigned long, int, int, int);

extern "C" float* UISGetColorMultipler(void);
extern "C" float* UISGetColorAdditive(void);
void UISSetColorMultipler(float, float, float, float);
void UISSetColorAdditive(float, float, float, float);

/* inferred: an object record's plugin index */
struct UISObjectIndexView {
    unsigned short plugin;
    unsigned short value02;
    unsigned long data;
};

/* inferred: the control state's colour scales and offsets */
struct UISControlColorView {
    unsigned long active;
    char field04[4];
    char rect[52];
    float multiply[4];
    float add[4];
};

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
            info->draw(1, ((UISControlColorView*)control->control)->rect);
            for (j = 0; j < control->subCount; j++) {
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
            info->draw(2, ((UISControlColorView*)control->control)->rect);
        }
    }
}

void UISProcessThreadAction(UISInfo_t*, unsigned char);
void* UISFindEventPC(UISControl_t*, unsigned long);
void* UISFindSubControlEventPC(UISControl_t*, unsigned short, unsigned long);

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
        UISEventLinkView* link = &control->events[i];
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
    stack = (UISStackInfo_t*)&info->stack6C;
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
    info->screens = (char*)info + offset;
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
    info->count50 = 0;
    info->table54 = (char*)info + offset;
    offset += count4C * 40;
    info->stack58 = info->stack5C = info->stack64 = (unsigned long*)((char*)info + offset);
    offset += stack1 * 4;
    info->stack60 = info->stack5C + stack1;
    info->stackA0 = info->stack60 - 1;
    info->stackA4 = info->stack60 - 1;
    info->stackA8 = &info->stack5C;
    info->stack6C = info->stack70 = info->stack78 = (unsigned long*)((char*)info + offset);
    offset += stack2 * 4;
    info->stack74 = info->stack70 + stack2;
    info->value08 = value;
    info->screen24 = -1;
    info->count0C = 0;
    info->draw = 0;
    info->count1C = 0;
    info->count10 = 0;
    info->count14 = 0;
    RuntimeErrorFnc = 0;
    info->screen20 = -1;
}
