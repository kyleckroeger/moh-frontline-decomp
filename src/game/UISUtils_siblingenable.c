// A fragment of UISUtils.c (0x8008a608): UISFindSiblingEnableControl finds
// the group of the screen's controls whose maps refer (through an action of
// type 0xffff) to the control, then returns the first control those maps
// refer to whose value at +4 is 1 (none: 0). UISScreen_t and
// UISControlInfo_t are named by the mangled symbols; the control list, map
// and action views and the two inline search helpers are inferred. The rest of
// the file is in other units.
struct UISControlInfo_t {
    char field00[4];
    unsigned long enable;
};

/* inferred: a map action: its type and the control it refers to */
struct UISMapAction_t {
    unsigned short type;
    unsigned long control;
};

/* inferred: a map: its condition, its actions and their count */
struct UISMap_t {
    unsigned long* condition;
    unsigned long count;
    UISMapAction_t* actions;
};

/* inferred: a screen control: the control and its maps */
struct UISControlEntry_t {
    UISControlInfo_t* control;
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
