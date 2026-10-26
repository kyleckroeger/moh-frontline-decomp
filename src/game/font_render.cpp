// A fragment of font.cpp (0x8007c320): CFont::Link (sets up the orthographic
// projection, vertex formats and TEV stages for text, loads the font texture,
// then draws every queued text run as quads: positions scaled by the run's
// scale, texture coordinates from the character cell over the texture size,
// the run's colour doubled and clamped; the projection is restored at the
// end), CFont::Render (the text run's count, position and colour are added to
// the packet, then its characters copied in one block; returns 0) and the
// font data accessors GetHeight, SetHeight, SetScale, GetScale, SetDepth (the
// depth's bits stored as a float, as the image does through the stack),
// GetColor and SetColor (the colour word copied, the result byte by byte
// through the copy constructor). The file name is this project's; the
// original record is font.cpp. CFont and its functions are named by the
// mangled symbols; the records, members and viewport view are inferred, and
// the hoisted declarations in Link follow the original's register use. CFont
// declares its destructor (defined elsewhere) first so its table is not
// emitted here.
#include <dolphin/gx.h>
#include <dolphin/mtx.h>



template <class T> T MathFunClamp(T, T, T);

/* Inferred: a render tag: the next tag and the packet data. */
class CDmaTag {
public:
    CDmaTag* m_next;
    void* m_data;
};

/* Inferred: the viewport's normalised edges (the rest is not used here). */
struct ViewportView {
    float left;
    float top;
    float right;
    float bottom;
    unsigned char unknown10[96];
};

extern ViewportView g_viewport;

/* Inferred: a colour with a member-wise copy constructor (as in fader.cpp). */
struct CColor {
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

    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

/* Inferred: the packet's write cursor at +8 and its templated append. */
class CDmaPacket {
public:
    template <class T> void Add(const T& value) {
        *(T*)m_cur = value;
        m_cur += sizeof(T);
    }

    unsigned char unknown00[8];
    unsigned char* m_cur;
};

/* Inferred: the font data's colour, height, depth and scale. */
struct FontDataView {
    unsigned char unknown00[32];
    CColor m_color;
    float m_height;
    float m_depth;
    unsigned char unknown2c[12];
    float m_scaleX;
    float m_scaleY;
};

/* Inferred: a text run's render record: the character count, position,
   colour, then 16 bytes per character. */
struct SFontRender {
    int m_count;
    float m_x;
    float m_y;
    CColor m_color;
    unsigned char m_chars[1];
};

class CRenderList;

/* Inferred: the texture's packed size at +60 (as in sprite_draw.cpp). */
class CTexture {
public:
    void Load(CDmaPacket&, CRenderList*);

    unsigned char unknown00[60];
    int m_width : 10;
    int m_height : 10;
    int m_flags : 12;
};

/* Inferred: a character of a text run as written into the packet. */
struct SFontChar {
    unsigned char m_width;
    unsigned char m_height;
    unsigned short m_u;
    unsigned short m_v;
    unsigned char unknown06[2];
    float m_x;
    float m_y;
};

class CRenderBin {
    unsigned char unknown00[28];

public:
    virtual ~CRenderBin();
    virtual int Render(CDmaPacket&, void*);
};

class CFont : public CRenderBin {
public:
    virtual ~CFont();
    virtual int Render(CDmaPacket&, void*);
    virtual void Link(CDmaTag*, CDmaPacket&);
    float GetHeight();
    void SetHeight(float);
    void SetScale(float, float);
    void GetScale(float&, float&);
    void SetDepth(unsigned long);
    CColor GetColor();
    void SetColor(CColor);

    FontDataView* m_data;
    CTexture* m_texture;
};

void CFont::Link(CDmaTag* tag, CDmaPacket& packet) {
    unsigned char r;
    float scaleX;
    int count;
    unsigned char g;
    float scaleY;
    unsigned char b;
    unsigned char a;
    SFontRender* render;
    CDmaTag* current = tag;
    if (tag) {
        Mtx view;
        Mtx44 ortho;
        float projection[GX_PROJECTION_SZ];
        GXGetProjectionv(projection);
        C_MTXOrtho(ortho, 448.0f * g_viewport.top, 448.0f * g_viewport.bottom, 640.0f * g_viewport.left,
                   640.0f * g_viewport.right, 5.0f, 2000.0f);
        GXSetProjection(ortho, GX_ORTHOGRAPHIC);
        PSMTXIdentity(view);
        view[2][3] = -1200.0f;
        GXLoadPosMtxImm(view, GX_PNMTX0);
        GXClearVtxDesc();
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
        GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
        GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);
        GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
        GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
        GXSetNumChans(1);
        GXSetNumTexGens(1);
        GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
        GXSetNumTevStages(2);
        GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_NOOP);
        GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
        GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
        GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_RASC, GX_CC_ZERO);
        GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_RASA, GX_CA_ZERO);
        GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD_NULL, GX_TEX_DISABLE, GX_COLOR_NULL);
        GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
        GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_2, GX_TRUE, GX_TEVPREV);
        GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
        GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_2, GX_TRUE, GX_TEVPREV);
        m_texture->Load(packet, 0);
        while (current) {
            render = (SFontRender*)current->m_data;
            count = render->m_count;
            if (count == 0) {
                current = current->m_next;
            } else {
                scaleX = render->m_x;
                scaleY = render->m_y;
                r = render->m_color.r;
                g = render->m_color.g;
                b = render->m_color.b;
                a = render->m_color.a;
                GXBegin(GX_QUADS, GX_VTXFMT0, count * 4);
                SFontChar* chars = (SFontChar*)render->m_chars;
                for (int i = 0; i < count; i++) {
                    SFontChar* c = &chars[i];
                    float x0 = scaleX * c->m_x;
                    float y0 = scaleY * c->m_y;
                    float x1 = scaleX * (c->m_x + (float)c->m_width);
                    float y1 = scaleY * (c->m_y + (float)c->m_height);
                    float u0 = (float)c->m_u / (float)m_texture->m_width;
                    float u1 = (float)(c->m_u + c->m_width) / (float)m_texture->m_width;
                    float v0 = (float)c->m_v / (float)m_texture->m_height;
                    float v1 = (float)(c->m_v + c->m_height) / (float)m_texture->m_height;
                    GXPosition3s16(x0, y0, 0);
                    GXColor4u8(MathFunClamp<int>(r * 2, 0, 255), MathFunClamp<int>(g * 2, 0, 255),
                               MathFunClamp<int>(b * 2, 0, 255), MathFunClamp<int>(a * 2, 0, 255));
                    GXTexCoord2f32(u0, v0);
                    GXPosition3s16(x1, y0, 0);
                    GXColor4u8(MathFunClamp<int>(r * 2, 0, 255), MathFunClamp<int>(g * 2, 0, 255),
                               MathFunClamp<int>(b * 2, 0, 255), MathFunClamp<int>(a * 2, 0, 255));
                    GXTexCoord2f32(u1, v0);
                    GXPosition3s16(x1, y1, 0);
                    GXColor4u8(MathFunClamp<int>(r * 2, 0, 255), MathFunClamp<int>(g * 2, 0, 255),
                               MathFunClamp<int>(b * 2, 0, 255), MathFunClamp<int>(a * 2, 0, 255));
                    GXTexCoord2f32(u1, v1);
                    GXPosition3s16(x0, y1, 0);
                    GXColor4u8(MathFunClamp<int>(r * 2, 0, 255), MathFunClamp<int>(g * 2, 0, 255),
                               MathFunClamp<int>(b * 2, 0, 255), MathFunClamp<int>(a * 2, 0, 255));
                    GXTexCoord2f32(u0, v1);
                }
                current = current->m_next;
            }
        }
        GXSetProjectionv(projection);
    }
}

int CFont::Render(CDmaPacket& packet, void* data) {
    SFontRender* render = (SFontRender*)data;
    packet.Add(render->m_count);
    packet.Add(render->m_x);
    packet.Add(render->m_y);
    packet.Add(render->m_color);
    int size = render->m_count * 16;
    memcpy(packet.m_cur, render->m_chars, size);
    packet.m_cur += size;
    return 0;
}

float CFont::GetHeight() {
    return m_data->m_height;
}

void CFont::SetHeight(float height) {
    m_data->m_height = height;
}

void CFont::SetScale(float x, float y) {
    m_data->m_scaleX = x;
    m_data->m_scaleY = y;
}

void CFont::GetScale(float& x, float& y) {
    x = m_data->m_scaleX;
    y = m_data->m_scaleY;
}

void CFont::SetDepth(unsigned long depth) {
    m_data->m_depth = *(float*)&depth;
}

CColor CFont::GetColor() {
    CColor color;
    *(unsigned int*)&color = *(unsigned int*)&m_data->m_color;
    return color;
}

void CFont::SetColor(CColor color) {
    *(unsigned int*)&m_data->m_color = *(unsigned int*)&color;
}
