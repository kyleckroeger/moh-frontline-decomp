// A fragment of UIStudio.c (0x80087f08): UISIdleProcess runs the queued
// thread actions, then the idle maps of every screen with the studio's busy
// flag (bit 1 of the flags at +4) set for each pass. UISInfo_t, UISScreen_t,
// UISStackInfo_t and UISParam_t are named by the mangled symbols; the views
// follow UISUtils_dohint.c and the parameter meanings (-10 the idle event)
// are inferred. The rest of the file is in other units.
struct UISParam_t;
struct UISStackInfo_t;

struct UISScreen_t {
    unsigned short group;
    unsigned short id;
    char field04[8];
};

struct UISInfo_t {
    char field00[4];
    unsigned long flags;
    char field08[36];
    long screenCount;
    UISScreen_t* screens;
    char field34[36];
    char stack[4];
};

void UISProcessThreadAction(UISInfo_t*, unsigned char);
void _ParseMaps(UISInfo_t*, UISScreen_t*, UISStackInfo_t*, unsigned long, unsigned long, unsigned long,
                unsigned long, UISParam_t*, signed char*);

extern "C" void UISIdleProcess(UISInfo_t* info, unsigned long value) {
    UISProcessThreadAction(info, 0);
    long i;
    long count = info->screenCount;
    for (i = 0; i < count; i++) {
        UISScreen_t* screen = &info->screens[i];
        signed char result = 0;
        info->flags |= 2;
        _ParseMaps(info, screen, (UISStackInfo_t*)info->stack, 0, value, -10, 0, 0, &result);
        info->flags &= ~2;
    }
}
