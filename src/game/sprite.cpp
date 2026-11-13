// sprite.cpp (0x80081894): 2D sprites. CSprite, a render bin holding a shape
// file's texture: Init (weak: clears two counters and initialises every
// render bin in the list it holds), Unload (closes the loaded file), Link
// (under a temporary orthographic projection of the 640x448 viewport and an
// identity view pushed back to -1200, with direct position/colour/texture
// vertices and a stage modulating the texture by the vertex colour, each
// queued request loads the sprite's texture and draws either the sprite as a
// quad at its screen position - the texture's size, the height scaled by the
// file's height scale, both by the screen scale - or the request's polygon as
// a fan or triangles, each colour channel doubled and clamped), Render (copies
// the request, and a polygon's vertices as raw words, into the packet),
// DrawPolyNoTex, DrawPoly and Draw (each fills g_DrawContext and renders a bin
// through the render list unless the configuration flag at +8 turns sprites
// off), InitClass (keeps the render list and screen, registers the untextured
// bin, sets the screen offsets from 2048 less half the screen's width and
// height and the height scale to 1/0.7, then halved), the destructor
// (unregisters, closes the file) and the constructor (loads the named file
// from the big file chosen by the load type; a failed load or a file without
// the "SHPG" shape tag is reported and leaves the sprite empty; otherwise it
// registers with the render list and sets its texture from the shape file);
// CSpriteNoTextureBin, the untextured bin (empty Link, Render returning 0);
// and the static initialisation of that bin (priority 10) and of
// g_DrawContext (colour (0, 0, 0, 128)). The file owns __vt__7CSprite,
// __vt__19CSpriteNoTextureBin and the RTTI records of CRenderBinData,
// CRenderBin, CSprite and CSpriteNoTextureBin. Classes, functions and the
// file's statics are named by the symbols; the configuration, tag, packet,
// viewport, screen and texture views, the draw-context fields (its type name,
// DrawContextView, is this project's), the packet layout, the vertex-copy
// helper, the polygon and load types, the priorities and CScreen's slots (as
// in particlesystem_reset.cpp) are inferred. CColor is an aggregate here (a
// constructor changes Link's frame). Compiled with deferred inlining,
// so the functions are written in reverse image order.
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

struct ShapeFile;

void TLT_CloseFile(void*);
void* TLT_LoadFileFromShellBriefingLogBigFile(const char*, int*);
void* TLT_LoadFileFromShellBigFile(const char*, int*);
void* TLT_LoadFileFromSlidesBigFile(const char*, int*);
void* TLT_LoadFileFromLevelBigFile(const char*, int*);
void DebugMsg(const char*, ...);

template <class T> T MathFunClamp(T, T, T);

enum ERenderPriority {};

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
    void Set(ShapeFile*, bool);
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
    void UnRegister(CRenderBin&);
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

class CRenderBinData {
public:
    CRenderBinData() : m_prev(0), m_next(0), m_bins(0), m_count0c(0), m_count10(0), m_priority(3) {}

    CRenderBin* m_prev;
    CRenderBin* m_next;
    CRenderBin* m_bins;
    int m_count0c;
    int m_count10;
    int m_priority;
    int m_field18;
};

class CRenderBin : public CRenderBinData {
public:
    virtual ~CRenderBin() {}
    virtual int Render(CDmaPacket&, void*);
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();
};

class CSprite : public CRenderBin {
public:
    enum ESpriteLoadType {};
    enum EPolyPrimType {};

    CSprite(const char*, ESpriteLoadType, ERenderPriority);
    virtual ~CSprite();
    virtual int Render(CDmaPacket&, void*);
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&);
    void Unload();

    static void DrawPolyNoTex(_spritepolyvert*, int, EPolyPrimType);
    void DrawPoly(_spritepolyvert*, int, EPolyPrimType);
    void Draw(float, float, CColor);
    static void InitClass(CRenderList*, CScreen*);

    void* m_file;
    CTexture m_texture;
};

class CSpriteNoTextureBin : public CRenderBin {
public:
    virtual int Render(CDmaPacket&, void*);
    CSpriteNoTextureBin() { m_priority = 10; }
    virtual ~CSpriteNoTextureBin();
    virtual void Link(CDmaTag*, CDmaPacket&);
};

/* inferred: the sprite draw request */
struct DrawContextView {
    DrawContextView() { m_color.r = 0; m_color.g = 0; m_color.b = 0; m_color.a = 128; }

    CSprite* m_sprite;
    int m_polygon;
    _spritepolyvert* m_verts;
    int m_count;
    CSprite::EPolyPrimType m_type;
    float m_x;
    float m_y;
    CColor m_color;
};

static CRenderList* g_pRenderList;
static CScreen* g_pScreen;
static float g_HeightScale = 1.0f;
static float g_ScreenXOffset;
static float g_ScreenYOffset;
static CSpriteNoTextureBin g_SpriteNoTexture;
static DrawContextView g_DrawContext;

__declspec(weak) CSpriteNoTextureBin::~CSpriteNoTextureBin() {
}

int CSpriteNoTextureBin::Render(CDmaPacket&, void*) {
    return 0;
}

void CSpriteNoTextureBin::Link(CDmaTag*, CDmaPacket&) {
}

CSprite::CSprite(const char* name, ESpriteLoadType type, ERenderPriority priority) {
    m_priority = priority;
    m_file = 0;
    if (!name)
        return;

    int size;
    char* data;
    if (type == 3)
        data = (char*)TLT_LoadFileFromShellBriefingLogBigFile(name, &size);
    else if (type == 2)
        data = (char*)TLT_LoadFileFromShellBigFile(name, &size);
    else if (type == 4)
        data = (char*)TLT_LoadFileFromSlidesBigFile(name, &size);
    else
        data = (char*)TLT_LoadFileFromLevelBigFile(name, &size);

    if (!data) {
        DebugMsg("CSprite::CSprite(%s):  TLT_LoadFile... FAILED!!!\n", name);
        return;
    }
    if (data[0] != 'S' || data[1] != 'H' || data[2] != 'P' || data[3] != 'G') {
        DebugMsg("CCSprite::CCSprite():  File is not a GAMECUBE Shape, type = %s\n", data);
        return;
    }
    m_file = data;
    ShapeFile* shape = (ShapeFile*)m_file;
    g_pRenderList->Register(*this);
    m_texture.Set(shape, false);
}

CSprite::~CSprite() {
    g_pRenderList->UnRegister(*this);
    if (m_file) {
        TLT_CloseFile(m_file);
        m_file = 0;
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

void CSprite::Unload() {
    if (m_file) {
        TLT_CloseFile(m_file);
        m_file = 0;
    }
}

__declspec(weak) void CSprite::Init() {
    m_count0c = 0;
    m_count10 = 0;
    for (CRenderBin* bin = m_bins; bin; bin = bin->m_next)
        bin->Init();
}
