// A fragment of UISUtils.c (0x8008a420): UISMoveScreenDrawPosition moves a
// screen up or down the draw order by swapping it with its neighbour the
// given number of times (stopping at either end), keeping the current
// screen (+36) and the screen pointers of the records at +72 (52 bytes,
// +28) and the modal records at +84 (40 bytes, +24) on the same screens.
// UISInfo_t and UISScreen_t are named by the mangled symbols; the views
// follow UISUtils.c and UIStudio_unloadmodal.c, and UISFindScreen's body
// follows UISUtils.c (the file defers inlining; this unit marks it inline so
// that it is inlined without being emitted). The rest of the file is in
// other units.
extern "C" void* memcpy(void*, const void*, unsigned long);

struct UISScreen_t {
    unsigned short group;
    unsigned short id;
    char field04[8];
};

/* inferred: a record (52 bytes) with a screen at +28 */
struct UISRecord48_t {
    char field00[28];
    UISScreen_t* screen;
    char field20[20];
};

/* inferred: a modal record (40 bytes) with the screen it was opened from at +24 */
struct UISModal_t {
    char field00[24];
    UISScreen_t* from;
    char field1C[12];
};

struct UISInfo_t {
    char field00[36];
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
};

inline unsigned long UISFindScreen(UISInfo_t* info, unsigned short group, unsigned short id) {
    unsigned long i;

    for (i = 0; i < info->screenCount; i++) {
        UISScreen_t* screen = &info->screens[i];

        if (screen->group == group && screen->id == id)
            break;
    }
    return i;
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
