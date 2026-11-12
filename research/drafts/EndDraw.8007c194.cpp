// CFontDriver, the functions at the start of the file: the font driver hooks
// that create a font's texture and batch characters into stringPacket for the
// CFont render bin. The class and structure names come from the mangled
// symbols; the members and the packet, FONT, FONTCHAR and font-data views are
// inferred from offsets. CFontDriver and CFont are non-virtual views here.
extern "C" void* memset(void*, int, unsigned long);

struct Shape;
struct CColor {
 unsigned char r, g, b, a;
    CColor() {}
    CColor(const CColor& c) {
        unsigned char cr = c.r;
        unsigned char cg = c.g;
        unsigned char ca = c.a;
        unsigned char cb = c.b;
        r = cr;
        g = cg;
        b = cb;
        a = ca;
    }
    CColor& operator=(const CColor& c) { a = c.a; g = c.g; b = c.b; r = c.r; return *this; }
};


class CTexture {
public:
    CTexture() { memset(this, 0, sizeof(CTexture)); }
    void Set(Shape*, bool);

    unsigned char unknown00[64];
    void* m_font;
    struct FONT* m_owner;
};

class CRenderBin;

class CRenderList {
public:
    void Render(CRenderBin&, void*);
};

struct FontDataView {
    CColor GetColor() const { CColor c; *(unsigned int*)&c = *(unsigned int*)&color; return c; }
    unsigned char unknown00[32];
    CColor color;
    unsigned char unknown24[20];
    float scaleX;
    float scaleY;
};

class CFont {
public:
    unsigned char unknown00[32];
    FontDataView* m_data;
    CTexture* m_texture;
};

struct FONT {
    unsigned char unknown00[28];
    int shapeOffset;
    unsigned char unknown20[80];
    CFont* font;
};

struct FONTCHAR {
    unsigned char unknown00[2];
    unsigned char u;
    unsigned char v;
    unsigned short width;
    unsigned short height;
    unsigned char unknown08;
    signed char xOffset;
    signed char yOffset;
};

struct FontPacketCharView {
    unsigned char u;
    unsigned char v;
    unsigned short width;
    unsigned short height;
    unsigned char unknown06[2];
    float x;
    float y;
};

struct FontPacketView {
    void SetColor(CColor c) { color = c; }
    int count;
    float scaleX;
    float scaleY;
    CColor color;
    FontPacketCharView chars[256];
};

extern FontPacketView stringPacket;
extern CRenderList* g_pRenderList;
extern CFont* g_pCurrentFont;

class CFontDriver {
public:
    static void DestroyFont(FONT*);
    static void CreateFont(FONT*);
    static void EndDraw(FONT*);
    static void StartDraw(FONT*);
    static void Draw(FONT*, const FONTCHAR*, float, float);
};

void CFontDriver::DestroyFont(FONT*) {
}

void CFontDriver::CreateFont(FONT* font) {
    if (g_pCurrentFont) {
        Shape* shape = (Shape*)((char*)font + font->shapeOffset);
        CTexture* texture = new CTexture;
        texture->Set(shape, false);
        texture->m_owner = font;
        g_pCurrentFont->m_texture = texture;
        texture->m_font = g_pCurrentFont;
        font->font = g_pCurrentFont;
    }
}

void CFontDriver::EndDraw(FONT* font) {
    CFont* renderFont = font->font;
    stringPacket.color = renderFont->m_data->GetColor();
    stringPacket.scaleX = renderFont->m_data->scaleX;
    stringPacket.scaleY = renderFont->m_data->scaleY;
    g_pRenderList->Render(*(CRenderBin*)renderFont, &stringPacket);
}

void CFontDriver::StartDraw(FONT*) {
    stringPacket.count = 0;
}

void CFontDriver::Draw(FONT*, const FONTCHAR* c, float x, float y) {
    stringPacket.chars[stringPacket.count].u = c->u;
    stringPacket.chars[stringPacket.count].v = c->v;
    stringPacket.chars[stringPacket.count].width = c->width;
    stringPacket.chars[stringPacket.count].height = c->height;
    stringPacket.chars[stringPacket.count].x = x + c->xOffset;
    stringPacket.chars[stringPacket.count].y = y + c->yOffset;
    stringPacket.count++;
}
