// A fragment of UISUtils.c (0x8008a774-0x8008ab18). The file's inlining is
// deferred, so the functions are defined in reverse image order. The setters
// and getters of the additive and multiplier colour factors (four floats
// each, file-local in the original and declared extern here; the result type
// of the getters is inferred), then UISGetActionPtrValue, which returns the
// address of the control value an action (16 to 39) changes, the value at
// +72 for the others, then UISStringFormat, which scans a counted format
// string for conversions ('%', s, f, c, d, x) and collects one argument word
// per conversion from the parameters (two for a float, widened to double;
// a null string stays 0), formats into a 512-byte static buffer and copies at
// most the output string's length into it, terminated. UISControlInfo_t and
// the other types are named by the mangled symbols; the result type follows
// UISEvent.c and the values (named by offset) and string/parameter views are
// inferred. The rest of the file is in other units.
/* inferred: a counted string (the length at +4, the text at +8) */
struct UISString_t {
    unsigned long field00;
    unsigned long length;
    char* text;
};

/* inferred: a 4-byte parameter read as a word, a float or a string */
union UISStringParamView {
    unsigned long value;
    float number;
    UISString_t* string;
};

struct UISScrData_t;
struct UISParam_t;

extern "C" {
int sprintf(char*, const char*, ...);
char* strncpy(char*, const char*, unsigned long);
}

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

void UISStringFormat(UISScrData_t* data, UISString_t* out, UISString_t* format, unsigned long count, UISParam_t* params) {
    static char Temp[512];
    unsigned long args[40] = { 0 };
    char* text;
    bool more;
    unsigned long length;
    unsigned long i;
    int n;
    int k;
    n = 0;
    k = 0;
    if (format) {
        text = format->text;
        length = format->length;
        for (i = 0; i < length; i++) {
            if (text[i] == '%') {
                more = true;
                while (more && i++ < length) {
                    switch (text[i]) {
                    case '%':
                        more = false;
                        break;
                    case 's':
                        if (((UISStringParamView*)params)[k].string)
                            args[n] = (unsigned long)((UISStringParamView*)params)[k].string->text;
                        more = false;
                        k++;
                        n++;
                        break;
                    case 'f':
                        *(double*)&args[n] = ((UISStringParamView*)params)[k].number;
                        k++;
                        more = false;
                        n += 2;
                        break;
                    case 'd':
                        args[n] = ((UISStringParamView*)params)[k].value;
                        n++;
                        more = false;
                        k++;
                        break;
                    case 'c':
                        args[n] = ((UISStringParamView*)params)[k].value;
                        n++;
                        more = false;
                        k++;
                        break;
                    case 'x':
                        args[n] = ((UISStringParamView*)params)[k].value;
                        n++;
                        more = false;
                        k++;
                        break;
                    }
                }
            }
        }
        if (out) {
            char* dest = out->text;
            unsigned long size = out->length;
            sprintf(Temp, text, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                    args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17],
                    args[18], args[19]);
            strncpy(dest, Temp, size);
            dest[size] = 0;
        }
    }
}
