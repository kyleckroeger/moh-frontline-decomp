// A fragment of UIStudio.c (0x80089ac0): the file-local _ParseVisibility
// walks a screen's control tree (the levels as in
// UIStudio_determinevis.c) setting the wanted visibility: a node already so,
// or hidden below the top (depth above 0), stops the walk; controls recurse
// into their maps and maps into the controls their 0xffff actions refer to,
// other actions being called through the studio's function table (+60, its
// size at +56) with event -4 and the wanted value. UISInfo_t and
// UISScreen_t are named by the mangled symbols; the views follow
// UIStudio_determinevis.c, and the meanings are
// inferred (the comparisons with the wanted value are written as an
// exclusive or because the code tests one, as in UIStudio_visibility.c). The rest of the file is in other units.
typedef void (*UISFunction_t)(unsigned long, unsigned long, unsigned long, unsigned long*, unsigned long);

struct UISInfo_t {
    char field00[56];
    unsigned long functionCount;
    UISFunction_t* functions;
};

struct UISMapAction_t {
    unsigned short type;
    unsigned long data;
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
        unsigned long visible = *first->control;
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
        unsigned long visible = *entry->control;
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
                info->functions[action->type](action->data, -4, 1, &want, 0);
        }
        break;
    }
    }
}
