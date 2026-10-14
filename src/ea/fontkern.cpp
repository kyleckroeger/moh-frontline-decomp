// fontkern.cpp, the whole file: FONT_getkern, the kerning between a character and the next one. A font
// either stores each character's kerning pairs in its own list (found
// through the character's kerning index) or keeps one table of pairs keyed
// by both characters. FONT_getkern is named by its symbol; the font,
// character and kerning-pair layouts and the inline lookups are inferred.
struct FONTKERNVIEW {
    unsigned short next;
    signed char amount;
    unsigned char first;
};

struct FONTCHARVIEW {
    unsigned short code;
    unsigned char unknown02[9];
    unsigned char kernCount;
    unsigned short kernIndex;
};

struct FONTVIEW {
    unsigned char unknown00[12];
    unsigned int flags;
    unsigned char unknown10[8];
    int kernOffset;
};

static inline FONTKERNVIEW* findcharkern(FONTKERNVIEW* pairs, int count, int next) {
    for (int i = 0; i < count; i++) {
        if (next == pairs[i].next)
            return &pairs[i];
    }
    return 0;
}

static inline FONTKERNVIEW* findtablekern(FONTKERNVIEW* pairs, int count, int first, int next) {
    for (int i = 0; i < count; i++) {
        if (next == pairs[i].next && first == pairs[i].first)
            return &pairs[i];
    }
    return 0;
}

extern "C" int FONT_getkern(FONTVIEW* font, FONTCHARVIEW* ch, int next) {
    int kern = 0;
    int count = ch->kernCount;
    if (count) {
        FONTKERNVIEW* pair;
        if (font->flags & 0x40000) {
            pair = findcharkern((FONTKERNVIEW*)((char*)font + font->kernOffset + ch->kernIndex * 4 + 4), count, next);
        } else {
            int* table = (int*)((char*)font + font->kernOffset);
            FONTKERNVIEW* pairs = (FONTKERNVIEW*)(table + 1);
            pair = findtablekern(pairs, *table, ch->code, next);
        }
        if (pair)
            kern = pair->amount;
    }
    return kern;
}
