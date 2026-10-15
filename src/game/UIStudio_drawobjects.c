// A fragment of UIStudio.c (0x80087fac): UISDrawObjects resets the colour
// multiplier to 1 and the additive colour to 0, runs the rate functions for
// the elapsed ticks (the value times the rate at +8), then parses the objects
// of every screen. UISInfo_t and UISScreen_t are named by the mangled
// symbols; the views follow UIStudio_idle.c and the meanings of the fixed
// arguments are inferred. The rest of the file is in other units.
struct UISScreen_t {
    unsigned short group;
    unsigned short id;
    char field04[8];
};

struct UISInfo_t {
    char field00[8];
    unsigned long rate;
    char field0c[32];
    unsigned long screenCount;
    UISScreen_t* screens;
};

void UISSetColorMultipler(float, float, float, float);
void UISSetColorAdditive(float, float, float, float);
void _ParseRateFncs(UISInfo_t*, unsigned long);
void _ParseObjects(UISInfo_t*, UISScreen_t*, unsigned long, unsigned long);

extern "C" void UISDrawObjects(UISInfo_t* info, unsigned long value) {
    unsigned long ticks = info->rate * value;
    UISSetColorMultipler(1.0f, 1.0f, 1.0f, 1.0f);
    UISSetColorAdditive(0.0f, 0.0f, 0.0f, 0.0f);
    _ParseRateFncs(info, ticks);
    unsigned long i;
    unsigned long count = info->screenCount;
    for (i = 0; i < count; i++)
        _ParseObjects(info, &info->screens[i], 0, -2);
}
