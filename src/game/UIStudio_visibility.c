// A fragment of UIStudio.c (0x80087ab8): UISUpdateVisibility determines a
// screen's current visibility and, when it is known and differs from the
// wanted one, parses the screen's visibility change. UISInfo_t and
// UISScreen_t are named by the mangled symbols; the parameter meanings are
// inferred, and the comparison is written as
// an exclusive or because the code tests one (xor.). The rest of the file is in other units.
struct UISInfo_t;
struct UISScreen_t;

long _DetermineVisibility(UISScreen_t*, void**, unsigned long, void*);
void _ParseVisibility(UISInfo_t*, UISScreen_t*, unsigned long, unsigned long, void*, unsigned long);

void UISUpdateVisibility(UISInfo_t* info, UISScreen_t* screen, unsigned long value, void* data, unsigned long show) {
    long visible = _DetermineVisibility(screen, &data, 10, screen);
    show = show != 0;
    if (visible != -1 && (visible ^ show) != 0)
        _ParseVisibility(info, screen, show, value, data, 0);
}
