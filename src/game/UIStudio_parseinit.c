// A fragment of UIStudio.c (0x80089ebc): the file-local _ParseInitialize runs
// a screen's initialisers: each screen-level one not yet run (its state at +2
// is 0) is called through the studio's function table (+60, its size at +56)
// and marked run; then every action of the control's maps either recurses
// into the control it refers to (type 0xffff) or is called through the
// table, its state set to whether the value is -1. UISInfo_t and
// UISScreen_t are named by the mangled symbols; the views follow
// UISUtils_siblingenable.c and the initialiser, function-table and
// parameter meanings, the hoisted declarations and the reuse of one counter
// for the initialiser and action loops are inferred. The rest of the file is
// in other units.
typedef void (*UISFunction_t)(unsigned long, unsigned long, unsigned long, unsigned long, unsigned long);

struct UISInfo_t {
    char field00[56];
    unsigned long functionCount;
    UISFunction_t* functions;
};

/* inferred: an action or initialiser: its function, state and argument */
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

struct UISControlEntry_t {
    void* control;
    unsigned long count;
    UISMap_t** maps;
    char field0C[8];
};

struct UISControlList_t {
    unsigned long count;
    UISControlEntry_t* entries;
    char field08[16];
    unsigned long initCount;
    UISMapAction_t* inits;
};

struct UISScreen_t {
    char field00[8];
    UISControlList_t* controls;
};

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
