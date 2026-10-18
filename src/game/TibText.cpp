// A fragment of TibText.cpp (0x8008ce84): TibTextProcessFnc, the text
// plugin's handler: -1 makes sure the library's font exists (loaded through
// the shell big file, made with render priority 111, coloured from the text's
// colour at +20 with alpha 128, set to the screen's maximum depth and the
// text's height) and turns the text's string offset into a pointer; -2 draws
// the text; -3 deletes the library's font (clearing the slot) and turns the
// string pointer back into an offset; 0 and 1 set two four-value colours from
// integers, 2 sets three values, 3 copies a string into the text's string
// (terminated when shorter), and 5 and 6 set one or two values at +52.
// TibTextGraphicDataT, UISParam_t, CFont, CColor, CScreen and the functions
// are named by the mangled symbols and LibraryInfo and g_screen by their
// symbols; the text, string, parameter and library views, the library object
// class (its table pointer after 28 bytes of members), the font-making inline
// (which keeps the loaded name out of a separate copy) and the action meanings
// are inferred. The rest of the file is in other units.
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

    unsigned char unknown00[40];
};

class CScreen {
public:
    unsigned long GetDepthBufferMax();

    unsigned char unknown000[272];
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
    unsigned char unknown0c[8];
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
        strncpy(string->chars, source->chars, string->length);
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
