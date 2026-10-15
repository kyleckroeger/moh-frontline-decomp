// A fragment of UIStudio.c (0x80088060): UISProcessInternalEvents parses an
// event through the maps of every screen, or, unless the flag is set, only
// of the current screen (+36, none when -1). UISInfo_t, UISScreen_t,
// UISStackInfo_t and UISParam_t are named by the mangled symbols; the views
// follow UIStudio_idle.c and the parameter meanings are inferred. The rest
// of the file is in other units.
struct UISParam_t;
struct UISStackInfo_t;

struct UISScreen_t {
    unsigned short group;
    unsigned short id;
    char field04[8];
};

struct UISInfo_t {
    char field00[36];
    unsigned long currentScreen;
    char field28[4];
    unsigned long screenCount;
    UISScreen_t* screens;
};

void _ParseMaps(UISInfo_t*, UISScreen_t*, UISStackInfo_t*, unsigned long, unsigned long, unsigned long,
                unsigned long, UISParam_t*, signed char*);

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
