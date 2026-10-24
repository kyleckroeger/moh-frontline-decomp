// A fragment of sprite.cpp (0x8008192c): CSprite's Link (under a temporary
// orthographic projection of the 640x448 viewport and an identity view
// pushed back to -1200, with direct position/colour/texture vertices and a
// stage modulating the texture by the vertex colour, each queued request
// loads the sprite's texture and draws either the sprite as a quad at its
// screen position - the texture's size, the height scaled by the file's
// height scale, both by the screen scale - or the request's polygon as a
// fan or triangles, each colour channel doubled and clamped), Render (copies
// the request, and a polygon's vertices as raw words, into the packet),
// DrawPolyNoTex (an untextured polygon through the file's untextured bin),
// DrawPoly (a textured polygon through this sprite), Draw (the sprite at a
// screen position in a colour) - each fills g_DrawContext and renders the bin
// through the render list unless the configuration flag at +8 turns sprites
// off - and InitClass (keeps the render list and screen, registers the
// untextured bin, and sets the screen offsets from 2048 less half the
// screen's width and height and the height scale to 1/0.7, then halved). The
// classes, functions and the file's statics are named by the symbols (the
// statics are file-local in the original; this fragment declares them
// extern so it links to the original objects); the configuration, tag,
// packet, viewport, screen and texture views, the draw-context fields, the
// packet layout, the vertex-copy helper, the polygon types and CScreen's
// slots (as in particlesystem_reset.cpp) are inferred. The rest of the file
// is not part of this unit.
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

/* Inferred: the screen's scale factors at +40 (as in TibText.cpp). */
struct ScreenScaleView {
    unsigned char unknown000[40];
    float m_scaleX;
    float m_scaleY;
    unsigned char unknown030[224];
};

extern ScreenScaleView g_screen;


struct CColor {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

struct _spritepolyvert {
    float u;
    float v;
    float x;
    float y;
    float z;
    CColor color;
};

/* inferred: a vertex as raw words */
struct PolyVertWords {
    unsigned long words[5];
    unsigned long color;
};

/* inferred: the packet's write cursor at +8 */
class CDmaPacket {
public:
    template <class T> void Write(const T& value) {
        *(T*)m_cursor = value;
        m_cursor += sizeof(T);
    }

    unsigned char unknown00[8];
    char* m_cursor;
};

class CRenderList;

/* Inferred: the texture's packed size at +60. */
class CTexture {
public:
    void Load(CDmaPacket&, CRenderList*);

    unsigned char unknown00[60];
    int m_width : 10;
    int m_height : 10;
    int m_flags : 12;
};


class CRenderBin;

class CRenderList {
public:
    void Render(CRenderBin&, void*);
    void Register(CRenderBin&);
};

class CScreen {
public:
    virtual ~CScreen();
    virtual void SetCurrent();
    virtual void Flip();
    virtual unsigned long Wait();
    virtual void SetClear(bool);
    virtual void SetClearColor(const CColor&);
    virtual void SetClearRect(const void*);
    virtual int GetHeight();
    virtual int GetWidth();
};

/* inferred: the configuration flag at +8 that turns sprites off */
struct ConfigView {
    unsigned char unknown00[8];
    bool noSprites;
    unsigned char unknown09[11];
};

extern ConfigView g_config;

class CRenderBin {
    unsigned char unknown00[28];

public:
    virtual ~CRenderBin();
};

class CSprite : public CRenderBin {
public:
    enum EPolyPrimType {};
    virtual ~CSprite();
    virtual void Link(CDmaTag*, CDmaPacket&);

    static void DrawPolyNoTex(_spritepolyvert*, int, EPolyPrimType);
    void DrawPoly(_spritepolyvert*, int, EPolyPrimType);
    void Draw(float, float, CColor);
    static void InitClass(CRenderList*, CScreen*);
    virtual int Render(CDmaPacket&, void*);

    unsigned char unknown20[4];
    CTexture m_texture;
};

/* inferred: the sprite draw request */
struct DrawContextView {
    CSprite* m_sprite;
    int m_polygon;
    _spritepolyvert* m_verts;
    int m_count;
    CSprite::EPolyPrimType m_type;
    float m_x;
    float m_y;
    CColor m_color;
};

/* inferred: a polygon request as written into the packet */
struct SpritePacketView {
    DrawContextView context;
    _spritepolyvert verts[1];
};

extern CRenderBin g_SpriteNoTexture;
extern DrawContextView g_DrawContext;
extern CRenderList* g_pRenderList;
extern CScreen* g_pScreen;
extern float g_ScreenXOffset;
extern float g_ScreenYOffset;
extern float g_HeightScale;

void CSprite::Link(CDmaTag* tag, CDmaPacket& packet) {
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
        GXSetCurrentMtx(GX_PNMTX0);
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
        GXSetNumTevStages(1);
        GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_NOOP);
        GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
        GXSetCullMode(GX_CULL_NONE);
        GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
        GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_RASC, GX_CC_ZERO);
        GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_RASA, GX_CA_ZERO);
        GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
        for (CDmaTag* current = tag; current; current = current->m_next) {
            DrawContextView* context = (DrawContextView*)current->m_data;
            m_texture.Load(packet, 0);
            if (context->m_polygon == 0) {
                float x0 = context->m_x;
                float y0 = context->m_y;
                float x1 = (float)m_texture.m_width * g_screen.m_scaleX + x0;
                unsigned short height = (float)m_texture.m_height * g_HeightScale;
                float y1 = (float)height * g_screen.m_scaleY + y0;
                unsigned char r = context->m_color.r;
                unsigned char g = context->m_color.g;
                unsigned char b = context->m_color.b;
                unsigned char a = context->m_color.a;
                GXBegin(GX_QUADS, GX_VTXFMT0, 4);
                GXPosition3s16(x0, y0, 0);
                GXColor4u8(MathFunClamp<int>(r * 2, 0, 255), MathFunClamp<int>(g * 2, 0, 255),
                           MathFunClamp<int>(b * 2, 0, 255), MathFunClamp<int>(a * 2, 0, 255));
                GXTexCoord2f32(0.0f, 0.0f);
                GXPosition3s16(x1, y0, 0);
                GXColor4u8(MathFunClamp<int>(r * 2, 0, 255), MathFunClamp<int>(g * 2, 0, 255),
                           MathFunClamp<int>(b * 2, 0, 255), MathFunClamp<int>(a * 2, 0, 255));
                GXTexCoord2f32(1.0f, 0.0f);
                GXPosition3s16(x1, y1, 0);
                GXColor4u8(MathFunClamp<int>(r * 2, 0, 255), MathFunClamp<int>(g * 2, 0, 255),
                           MathFunClamp<int>(b * 2, 0, 255), MathFunClamp<int>(a * 2, 0, 255));
                GXTexCoord2f32(1.0f, 1.0f);
                GXPosition3s16(x0, y1, 0);
                GXColor4u8(MathFunClamp<int>(r * 2, 0, 255), MathFunClamp<int>(g * 2, 0, 255),
                           MathFunClamp<int>(b * 2, 0, 255), MathFunClamp<int>(a * 2, 0, 255));
                GXTexCoord2f32(0.0f, 1.0f);
            } else if (context->m_polygon == 1) {
                int count = context->m_count;
                if (context->m_type == 5)
                    GXBegin(GX_TRIANGLEFAN, GX_VTXFMT0, count);
                else if (context->m_type == 3)
                    GXBegin(GX_TRIANGLES, GX_VTXFMT0, count);
                for (int i = 0; i < count; i++) {
                    _spritepolyvert vert = *(_spritepolyvert*)((char*)current->m_data + i * sizeof(_spritepolyvert) + sizeof(DrawContextView));
                    GXPosition3s16(vert.x, vert.y, 10);
                    GXColor4u8(MathFunClamp<int>(vert.color.r * 2, 0, 255), MathFunClamp<int>(vert.color.g * 2, 0, 255),
                               MathFunClamp<int>(vert.color.b * 2, 0, 255), MathFunClamp<int>(vert.color.a * 2, 0, 255));
                    GXTexCoord2f32(vert.u, vert.v);
                }
            }
        }
        GXSetProjectionv(projection);
    }
}

static inline void WriteVerts(CDmaPacket& packet, PolyVertWords* verts, int count) {
    for (int i = 0; i < count; i++)
        packet.Write(verts[i]);
}

int CSprite::Render(CDmaPacket& packet, void* data) {
    DrawContextView* context = (DrawContextView*)data;
    switch (context->m_polygon) {
    case 0:
        packet.Write(*context);
        break;
    case 1: {
        int count = context->m_count;
        PolyVertWords* verts = (PolyVertWords*)context->m_verts;
        packet.Write(*context);
        WriteVerts(packet, verts, count);
        break;
    }
    }
    return 0;
}

void CSprite::DrawPolyNoTex(_spritepolyvert* verts, int count, EPolyPrimType type) {
    if (!g_config.noSprites) {
        g_DrawContext.m_sprite = 0;
        g_DrawContext.m_verts = verts;
        g_DrawContext.m_count = count;
        g_DrawContext.m_type = type;
        g_DrawContext.m_polygon = 1;
        g_pRenderList->Render(g_SpriteNoTexture, &g_DrawContext);
    }
}

void CSprite::DrawPoly(_spritepolyvert* verts, int count, EPolyPrimType type) {
    if (!g_config.noSprites) {
        g_DrawContext.m_verts = verts;
        g_DrawContext.m_count = count;
        g_DrawContext.m_sprite = this;
        g_DrawContext.m_polygon = 1;
        g_DrawContext.m_type = type;
        g_pRenderList->Render(*this, &g_DrawContext);
    }
}

void CSprite::Draw(float x, float y, CColor color) {
    if (!g_config.noSprites) {
        g_DrawContext.m_sprite = this;
        g_DrawContext.m_polygon = 0;
        g_DrawContext.m_x = x;
        g_DrawContext.m_y = y;
        g_DrawContext.m_color = color;
        g_pRenderList->Render(*this, &g_DrawContext);
    }
}

void CSprite::InitClass(CRenderList* list, CScreen* screen) {
    g_pRenderList = list;
    list->Register(g_SpriteNoTexture);
    g_pScreen = screen;
    g_ScreenXOffset = 2048.0f - (float)(g_pScreen->GetWidth() >> 1);
    g_ScreenYOffset = 2048.0f - (float)(g_pScreen->GetHeight() >> 1);
    g_HeightScale = 1.4285715f;
    g_HeightScale *= 0.5f;
}
