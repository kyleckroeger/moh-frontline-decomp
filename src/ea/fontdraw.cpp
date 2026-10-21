// FONT_switchcase, a file-local helper that gives the other case of an ASCII
// or Latin-1 letter (0 for any other character), the 8-bit text entry points
// (FONT_drawtexta and FONT_getrecta forward to the templated drawers for
// unsigned char text, FONT_drawtexta with no user data), and the weak
// FONT_getrectx<unsigned char> instance: the bounding box of a text in font
// units scaled by the font's scale, walking its glyphs (found through their
// table slot, else by binary search; a missing glyph tries the other case,
// then the 127 glyph, and a newline moves down one line), each placed after
// its kerning against the previous character and its offsets, advancing by
// its (wide) advance; outputs left/top/width/height (0 when empty). The file
// is compiled with -inline on, -fp_contract off and -use_lmw_stmw off. FONT,
// the functions and the template names come from the mangled symbols; the
// font and glyph record views and the local helper are inferred, as are the
// result types. FONT_drawtextx and the rest of the file are not part of this
// unit.
/* inferred: a glyph record (12 bytes, or 16 with the wide-advance flag):
   code, size, advance and offsets */
struct FONTCHAR {
    unsigned short code;
    unsigned char width;
    unsigned char height;
    unsigned char unknown04[4];
    signed char advance;
    signed char xoffset;
    signed char yoffset;
    unsigned char unknown0b[3];
    short wideAdvance;
};

/* inferred: the font record's glyph count (+10), flags (+12; 0x40000 =
   16-byte glyphs with wide advances), line spacing bytes (+18, +19), glyph
   table offset (+20) and scale (+56, +60) */
struct FONT {
    unsigned char unknown00[10];
    unsigned short count;
    unsigned int flags;
    unsigned char unknown10[2];
    unsigned char descent;
    unsigned char ascent;
    int table;
    unsigned char unknown18[32];
    float scaleX;
    float scaleY;
};

extern "C" {
void* FONT_bsearch(int, char*, int, int);
int FONT_getkern(FONT*, FONTCHAR*, unsigned char);
}

static int FONT_switchcase(int ch) {
    if (ch >= 'A' && ch <= 'Z')
        return ch + 32;
    if (ch >= 'a' && ch <= 'z')
        return ch - 32;
    if (ch >= 0xc0 && ch <= 0xd6)
        return ch + 32;
    if (ch >= 0xd8 && ch <= 0xde)
        return ch + 32;
    if (ch >= 0xe0 && ch <= 0xf6)
        return ch - 32;
    if (ch >= 0xf8 && ch <= 0xfe)
        return ch - 32;
    return 0;
}

/* inferred: a glyph by character code, from its slot when the table is
   indexed by code - 32, otherwise by binary search */
static inline FONTCHAR* FONT_findchar(FONT* font, int ch) {
    int stride = 12;
    char* table = (char*)font + font->table;
    if (font->flags & 0x40000)
        stride = 16;
    int count = font->count;
    if (ch - 32 < count) {
        FONTCHAR* glyph = (FONTCHAR*)(table + (ch - 32) * stride);
        if (glyph->code == ch)
            return glyph;
    }
    return (FONTCHAR*)FONT_bsearch(ch, table, count, stride);
}

template <class T> int FONT_drawtextx(FONT*, float, float, const T*, void*);

template <class T> void FONT_getrectx(FONT* font, const T* text, float* x, float* y, float* w, float* h) {
    float scaleX = font->scaleX;
    float scaleY = font->scaleY;
    float penX = 0.0f;
    float penY = 0.0f;
    float minX = 1.0e7f;
    float maxX = -1.0e7f;
    float minY = 1.0e7f;
    float maxY = -1.0e7f;
    int prev = 0;
    while (*text) {
        FONTCHAR* glyph = FONT_findchar(font, *text);
        if (!glyph) {
            if (*text == '\n') {
                prev = 0;
                text++;
                penX = 0.0f;
                penY += (int)(scaleY * (font->ascent + font->descent));
                continue;
            }
            int other = FONT_switchcase(*text);
            if (other)
                glyph = FONT_findchar(font, other);
            if (!glyph)
                glyph = FONT_findchar(font, 127);
            if (!glyph) {
                text++;
                continue;
            }
        }
        int kern = FONT_getkern(font, glyph, prev);
        penX += kern * scaleX;
        float left = penX + glyph->xoffset * scaleX;
        float top = penY + glyph->yoffset * scaleY;
        if (left < minX)
            minX = left;
        if (top < minY)
            minY = top;
        float right = left + glyph->width * scaleX;
        float bottom = top + glyph->height * scaleY;
        if (right > maxX)
            maxX = right;
        if (bottom > maxY)
            maxY = bottom;
        if (font->flags & 0x40000)
            penX += glyph->wideAdvance * scaleX;
        else
            penX += glyph->advance * scaleX;
        prev = *text;
        text++;
    }
    if (x)
        *x = maxX > minX ? minX : 0.0f;
    if (y)
        *y = maxY > minY ? minY : 0.0f;
    if (w)
        *w = maxX > minX ? maxX - minX : 0.0f;
    if (h)
        *h = maxY > minY ? maxY - minY : 0.0f;
}

extern "C" {
int FONT_drawtexta(FONT* font, float x, float y, const unsigned char* text) {
    return FONT_drawtextx<unsigned char>(font, x, y, text, 0);
}

int FONT_getrecta(FONT* font, const unsigned char* text, float* x, float* y, float* w, float* h) {
    FONT_getrectx<unsigned char>(font, text, x, y, w, h);
}
}
