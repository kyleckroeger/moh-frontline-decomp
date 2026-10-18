// A fragment of TibText.cpp (0x8008c828): TibTextDraw and TibTextProcessFnc.
// TibTextDraw skips drawing while the shell's flag at +0x19a7 is set without
// the one at +0x199b, colours the library font from the text's colour plus the
// interface studio's additive colour times its multiplier (alpha halved), sets
// a unit scale, transforms the text's position through the current matrix and
// draws it centred (16: half the rectangle's width and height subtracted from
// x), right-aligned (32: the whole width and height) or as is, scaled by the
// screen; a string with a carriage return before its end is drawn line by
// line through a 128-byte buffer, one rectangle height apart.
// TibTextProcessFnc is the text plugin's handler: -1 makes sure the library's
// font exists (loaded through the shell big file, made with render priority
// 111, coloured from the text's colour at +20 with alpha 128, set to the
// screen's maximum depth and the text's height) and turns the text's string
// offset into a pointer; -2 draws the text; -3 deletes the library's font
// (clearing the slot) and turns the string pointer back into an offset; 0 and
// 1 set two four-value colours from integers, 2 sets three values, 3 copies a
// string into the text's string (terminated when shorter), and 5 and 6 set one
// or two values at +52.
// TibTextGraphicDataT, UISParam_t, CFont, CColor, CScreen, CVector3, CMatrix,
// CMatrixStack, CShellMenu and the functions are named by the mangled symbols
// and LibraryInfo, g_screen, g_matStack and g_Shell by their symbols; the
// text, string, parameter, shell, screen and library views, the library object
// class (its table pointer after 28 bytes of members), the alignment values,
// the multi-line and line-drawing inlines (their copies in each case give the
// three line buffers), the font-making inline (which keeps the loaded name out
// of a separate copy) and the action meanings are inferred. The rest of the
// file is in other units.
enum ERenderPriority {};

struct CColor {
    CColor(unsigned char ar, unsigned char ag, unsigned char ab, unsigned char aa) : r(ar), g(ag), b(ab), a(aa) {}

    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

class CFont {
public:
    CFont(const char*, ERenderPriority);
    void SetColor(CColor);
    void SetDepth(unsigned long);
    void SetHeight(float);
    void SetScale(float, float);
    void DrawText(float, float, const char*);

    unsigned char unknown00[32];
    long m_handle;
    unsigned char unknown24[4];
};

class CScreen {
public:
    unsigned long GetDepthBufferMax();

    unsigned char unknown000[40];
    float m_scaleX;
    float m_scaleY;
    unsigned char unknown030[224];
};

extern CScreen g_screen;

struct UISString_t {
    unsigned char unknown00[4];
    unsigned long length;
    char* chars;
};

struct UISParam_t {
    long value;
};

struct TibTextGraphicDataT {
    long text;
    unsigned char unknown04[4];
    short id;
    short library;
    unsigned char unknown0c[4];
    long align;
    float color0[4];
    float color1[4];
    float value34;
    float value38;
    unsigned char unknown3c[4];
    float value40[3];
    short height;
};

class TibLibraryObject {
public:
    unsigned char unknown00[28];

    virtual ~TibLibraryObject();
};

struct LibraryInfoView {
    unsigned char unknown00[4];
    const char** names;
    long* objects;
};

extern LibraryInfoView* LibraryInfo;

const char* TLT_LoadFileFromShellBigFile(const char*, int*);

class CVector3 {
public:
    CVector3() {}
    CVector3(float x, float y, float z) {
        d.v[2] = z;
        d.v[1] = y;
        d.v[0] = x;
    }
    void Set(float x, float y, float z) {
        d.v[0] = x;
        d.v[1] = y;
        d.v[2] = z;
    }

    union {
        double pair[2];
        float v[4];
    } d;
} __attribute__((aligned(8)));

class CMatrix {
public:
    void TransformPoint(CVector3&, CVector3) const;

    float m[4][4];
};

class CMatrixStack {
public:
    CMatrix& Current() { return m_stack[m_index]; }

    unsigned long m_size;
    int m_index;
    CMatrix* m_stack;
};

extern CMatrixStack* g_matStack;

class CShellMenu {
public:
    unsigned char unknown0000[6555];
    unsigned char m_value199b;
    unsigned char unknown199c[11];
    unsigned char m_value19a7;
};

extern CShellMenu g_Shell;

extern "C" {
float* UISGetColorMultipler(void);
float* UISGetColorAdditive(void);
void FONT_getrecta(long, const char*, float*, float*, float*, float*);
unsigned long strlen(const char*);
char* strchr(const char*, int);
char* strrchr(const char*, int);
void* memset(void*, int, unsigned long);
void* memcpy(void*, const void*, unsigned long);
}

static inline bool IsMultiLine(const char* chars) {
    int length = strlen(chars);
    const char* last = strrchr(chars, 13);
    int end = last ? last - chars + 1 : length;
    if (length != end)
        return true;
    return false;
}

static inline void DrawLines(CFont* font, const char* chars, float px, float py, float step) {
    float x = px;
    float y = py;
    int remaining = strlen(chars);
    while (remaining > 0) {
        char line[128];
        memset(line, 0, 128);
        const char* next = strchr(chars, 13);
        if (next) {
            int count = next - chars;
            remaining -= count;
            remaining -= 2;
            memcpy(line, chars, count);
            font->DrawText(x, y, line);
            y += step;
            chars = next + 2;
        } else {
            int count = strlen(chars);
            remaining -= count;
            memcpy(line, chars, count);
            font->DrawText(x, y, line);
            y += step;
        }
    }
}

void TibTextDraw(TibTextGraphicDataT* text) {
    if (g_Shell.m_value19a7 && !g_Shell.m_value199b)
        return;
    float* add = UISGetColorAdditive();
    float* mul = UISGetColorMultipler();
    if (text->library < 0)
        return;
    CFont* font = (CFont*)LibraryInfo[text->library].objects[text->id];
    font->SetColor(CColor(mul[0] * (text->color0[0] + add[0]), mul[1] * (text->color0[1] + add[1]),
                          mul[2] * (text->color0[2] + add[2]), 0.5f * (mul[3] * (text->color0[3] + add[3]))));
    font->SetScale(1.0f, 1.0f);
    CVector3 screen(0.0f, 0.0f, 0.0f);
    CVector3 point(0.0f, 0.0f, 0.0f);
    point.Set(text->value34, text->value38, 0.0f);
    g_matStack->Current().TransformPoint(screen, point);
    if (text->align == 16) {
        float left, top, width, height;
        FONT_getrecta(font->m_handle, ((UISString_t*)text->text)->chars, &left, &top, &width, &height);
        screen.d.v[0] -= 0.5f * width;
        screen.d.v[0] -= 0.5f * height;
        screen.d.v[0] *= g_screen.m_scaleX;
        screen.d.v[1] *= g_screen.m_scaleY;
        if (IsMultiLine(((UISString_t*)text->text)->chars))
            DrawLines(font, ((UISString_t*)text->text)->chars, screen.d.v[0], screen.d.v[1], height);
        else
            font->DrawText(screen.d.v[0], screen.d.v[1], ((UISString_t*)text->text)->chars);
    } else if (text->align == 32) {
        bool multi = IsMultiLine(((UISString_t*)text->text)->chars);
        float left, top, width, height;
        FONT_getrecta(font->m_handle, ((UISString_t*)text->text)->chars, &left, &top, &width, &height);
        screen.d.v[0] -= width;
        screen.d.v[0] -= height;
        screen.d.v[0] *= g_screen.m_scaleX;
        screen.d.v[1] *= g_screen.m_scaleY;
        if (multi)
            DrawLines(font, ((UISString_t*)text->text)->chars, screen.d.v[0], screen.d.v[1], height);
        else
            font->DrawText(screen.d.v[0], screen.d.v[1], ((UISString_t*)text->text)->chars);
    } else {
        bool multi = IsMultiLine(((UISString_t*)text->text)->chars);
        float left, top, width, height;
        FONT_getrecta(font->m_handle, ((UISString_t*)text->text)->chars, &left, &top, &width, &height);
        screen.d.v[0] *= g_screen.m_scaleX;
        screen.d.v[1] *= g_screen.m_scaleY;
        if (multi)
            DrawLines(font, ((UISString_t*)text->text)->chars, screen.d.v[0], screen.d.v[1], height);
        else
            font->DrawText(screen.d.v[0], screen.d.v[1], ((UISString_t*)text->text)->chars);
    }
}
void TibTextDraw(TibTextGraphicDataT*);
extern "C" char* strncpy(char*, const char*, unsigned long);

static inline CFont* MakeFont(const char* name) { return new CFont(name, (ERenderPriority)111); }
void TibTextProcessFnc(void* object, unsigned long action, unsigned long, UISParam_t* params, UISParam_t*) {
    TibTextGraphicDataT* text = (TibTextGraphicDataT*)object;
    switch (action) {
    case (unsigned long)-1: {
        LibraryInfoView* library = &LibraryInfo[text->library];
        if (!library->objects[text->id]) {
            CFont* font = MakeFont(TLT_LoadFileFromShellBigFile(library->names[text->id], 0));
            CColor color(text->color0[0], text->color0[1], text->color0[2], 128);
            font->SetColor(color);
            font->SetDepth(g_screen.GetDepthBufferMax());
            font->SetHeight(text->height);
            LibraryInfo[text->library].objects[text->id] = (long)font;
        }
        text->text = (long)text + text->text;
        break;
    }
    case (unsigned long)-2:
        TibTextDraw(text);
        break;
    case (unsigned long)-3: {
        long font = LibraryInfo[text->library].objects[text->id];
        if (font) {
            delete (TibLibraryObject*)font;
            LibraryInfo[text->library].objects[text->id] = 0;
        }
        text->text -= (long)text;
        break;
    }
    case 0:
        text->color0[3] = params[3].value;
        text->color0[2] = params[2].value;
        text->color0[1] = params[1].value;
        text->color0[0] = params[0].value;
        break;
    case 1:
        text->color1[3] = params[3].value;
        text->color1[2] = params[2].value;
        text->color1[1] = params[1].value;
        text->color1[0] = params[0].value;
        break;
    case 2:
        text->value40[0] = *(float*)&params[2].value;
        text->value40[1] = *(float*)&params[1].value;
        text->value40[2] = *(float*)&params[0].value;
        break;
    case 3: {
        UISString_t* source = (UISString_t*)params[0].value;
        UISString_t* string = (UISString_t*)text->text;
        strncpy(((UISString_t*)text->text)->chars, source->chars, string->length);
        if (source->length < ((UISString_t*)text->text)->length)
            ((UISString_t*)text->text)->chars[source->length] = 0;
        break;
    }
    case 6:
        text->value34 = *(float*)&params[0].value;
        text->value38 = *(float*)&params[1].value;
        break;
    case 5:
        text->value34 = *(float*)&params[0].value;
        break;
    }
}
