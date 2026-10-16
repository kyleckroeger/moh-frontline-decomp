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
// mangled symbol; its views and the separate loop counters are inferred. The
// rest of the file is in other units.
typedef void (*UISErrorFnc_t)(void);

extern UISErrorFnc_t RuntimeErrorFnc;

struct UISInfo_t {
    unsigned long magic;
    unsigned long flags;
    unsigned long value08;
    unsigned long count0C;
    unsigned long count10;
    unsigned long count14;
    unsigned long count18;
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

struct UISEventRefView {
    unsigned short id;
    unsigned short state;
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
    info->count18 = 0;
    info->count1C = 0;
    info->count10 = 0;
    info->count14 = 0;
    RuntimeErrorFnc = 0;
    info->screen20 = -1;
}
