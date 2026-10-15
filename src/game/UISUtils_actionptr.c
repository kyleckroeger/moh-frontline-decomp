// A fragment of UISUtils.c (0x8008a9fc): the setters and getters of the
// additive and multiplier colour factors (four floats each, file-local in the
// original and declared extern here; the result type of the getters is
// inferred; the file's inlining is deferred, so they are defined in reverse
// order after the function they precede), then UISGetActionPtrValue, which
// returns the
// address of the control value an action (16 to 39) changes, the value at
// +72 for the others. UISControlInfo_t is named by the mangled symbol; the
// result type follows UISEvent.c and the values (named by offset) are
// inferred. The rest of the file is in other units.
struct UISControlInfo_t {
    char field00[8];
    float value08;
    float value0C;
    float value10;
    float value14;
    float value18;
    float value1C;
    float value20;
    float value24;
    float value28;
    float value2C;
    float value30;
    float value34;
    char field38[4];
    float value3C;
    float value40;
    float value44;
    float value48;
    float value4C;
    float value50;
    float value54;
    float value58;
};

extern float _AdditiveColorFactor[4];  // file-local in the original
extern float _MultiplerColorFactor[4]; // file-local in the original

float* UISGetActionPtrValue(unsigned long action, UISControlInfo_t* control) {
    switch (action) {
    case 16:
        return &control->value20;
    case 17:
        return &control->value24;
    case 18:
        return &control->value28;
    case 19:
        return &control->value08;
    case 20:
        return &control->value0C;
    case 21:
        return &control->value10;
    case 22:
        return &control->value2C;
    case 23:
        return &control->value30;
    case 24:
        return &control->value34;
    case 25:
        return &control->value14;
    case 26:
        return &control->value18;
    case 27:
        return &control->value1C;
    case 32:
        return &control->value58;
    case 33:
        return &control->value4C;
    case 34:
        return &control->value50;
    case 35:
        return &control->value54;
    case 36:
        return &control->value48;
    case 37:
        return &control->value3C;
    case 38:
        return &control->value40;
    case 39:
        return &control->value44;
    }
    return &control->value48;
}

extern "C" float* UISGetColorMultipler() {
    return _MultiplerColorFactor;
}

extern "C" float* UISGetColorAdditive() {
    return _AdditiveColorFactor;
}

void UISSetColorMultipler(float r, float g, float b, float a) {
    _MultiplerColorFactor[3] = a;
    _MultiplerColorFactor[0] = r;
    _MultiplerColorFactor[1] = g;
    _MultiplerColorFactor[2] = b;
}

void UISSetColorAdditive(float r, float g, float b, float a) {
    _AdditiveColorFactor[3] = a;
    _AdditiveColorFactor[0] = r;
    _AdditiveColorFactor[1] = g;
    _AdditiveColorFactor[2] = b;
}
