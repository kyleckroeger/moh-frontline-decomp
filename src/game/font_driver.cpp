// A fragment of font.cpp; the file name is this project's. CFontDriver, the functions at the start of the file: the static font-driver
// hooks that destroy (nothing to do) and create a font's texture for the
// current CFont, and EndDraw, which hands stringPacket's batched characters
// (with the font's colour and scale) to the CFont render bin; StartDraw and
// Draw follow in font_draw.cpp. The class and structure names come
// from the mangled symbols; the members, the FONT, FONTCHAR, packet and
// font-data views and CTexture's zeroing constructor are inferred from
// offsets. CFont is a non-virtual view here. EndDraw's colour goes through a
// word on the stack: the inferred inline FontDataView::GetColor copies the
// colour word into a local and returns it viewed as a CColor (the shared
// game/CColor.h, implicit copy).
extern "C" void* memset(void*, int, unsigned long);

struct Shape;
#include "game/CColor.h"


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
    /* inferred: copies the colour as one word through a local (the original's stack word) */
    CColor GetColor() const { unsigned int w = *(unsigned int*)&color; return *(CColor*)&w; }
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
