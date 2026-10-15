// A fragment of UISUtils.c (0x8008a044): UISDoHint runs the hint parser over
// every screen with the studio's busy flag (bit 1 of the flags at +4) set
// for the duration of each pass. UISInfo_t, UISScreen_t, UISStackInfo_t and
// UISParam_t are named by the mangled symbols; the members follow
// UISUtils.c (the flags word and the stack record at +88 are inferred). The
// rest of the file is in other units.
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
    unsigned long screenCount;
    UISScreen_t* screens;
    char field34[36];
    char stack[4];
};

void _ParseHints(UISInfo_t*, UISScreen_t*, UISStackInfo_t*, unsigned long, unsigned long, unsigned long,
                 UISParam_t*);

void UISDoHint(UISInfo_t* info, unsigned long hint, unsigned long value, UISParam_t* params) {
    unsigned long i;
    unsigned long count = info->screenCount;
    for (i = 0; i < count; i++) {
        UISScreen_t* screen = &info->screens[i];
        info->flags |= 2;
        _ParseHints(info, screen, (UISStackInfo_t*)info->stack, 0, hint, value, params);
        info->flags &= ~2;
    }
}
