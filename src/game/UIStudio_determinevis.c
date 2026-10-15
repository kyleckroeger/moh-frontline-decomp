// A fragment of UIStudio.c (0x80089cc8): the file-local _DetermineVisibility
// searches a screen's control tree for the node held in *found, returning
// its visibility (the value its first word points to; 0 without one) and
// storing the node that holds it, or -1. Level 10 is the screen (its first
// control may be the node, else the search continues there), level 8 a
// control (its maps are searched when it is visible) and level 7 a map (the
// controls its 0xffff actions refer to are searched when its condition
// holds). UISScreen_t is named by the mangled symbol; the views follow
// UISUtils_siblingenable.c and the level meanings are inferred (the found
// pointer and the result are cleared in one assignment, as the code shares
// the zero). The rest of
// the file is in other units.
struct UISMapAction_t {
    unsigned short type;
    unsigned long control;
};

struct UISMap_t {
    unsigned long* condition;
    unsigned long count;
    UISMapAction_t* actions;
};

struct UISControlEntry_t {
    unsigned long* control;
    unsigned long count;
    UISMap_t** maps;
    char field0C[8];
};

struct UISControlList_t {
    unsigned long count;
    UISControlEntry_t* entries;
};

struct UISScreen_t {
    char field00[8];
    UISControlList_t* controls;
};

static long _DetermineVisibility(UISScreen_t* screen, void** found, unsigned long level, void* node) {
    void* target;
    if (!found || !(target = *found) || !node || !screen)
        return -1;
    switch (level) {
    case 10: {
        UISControlList_t* list = screen->controls;
        if (list && list->entries->control == target) {
            unsigned long* control;
            long result;
            *found = (void*)(result = 0);
            control = screen->controls->entries->control;
            if (control)
                result = *control;
            return result;
        }
        node = list->entries;
    }
    case 8: {
        UISControlEntry_t* entry = (UISControlEntry_t*)node;
        if (entry->control == target) {
            *found = entry;
            return entry->control ? *entry->control : 0;
        }
        if (entry->control && *entry->control) {
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
                    long result = _DetermineVisibility(screen, found, 8, &screen->controls->entries[action->control]);
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
