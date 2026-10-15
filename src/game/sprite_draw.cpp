// A fragment of sprite.cpp (0x800823a0): CSprite's DrawPolyNoTex (an
// untextured polygon through the file's untextured bin), DrawPoly (a
// textured polygon through this sprite), Draw (the sprite at a screen
// position in a colour) - each fills g_DrawContext and renders the bin
// through the render list unless the configuration flag at +8 turns sprites
// off - and InitClass (keeps the render list and screen, registers the
// untextured bin, and sets the screen offsets from 2048 less half the
// screen's width and height and the height scale to 1/0.7, then halved). The
// classes, functions and the file's statics are named by the symbols (the
// statics are file-local in the original; this fragment declares them
// extern so it links to the original objects); the configuration view, the
// draw-context fields, the polygon types and CScreen's slots (as in
// particlesystem_reset.cpp) are inferred. The rest of the file is not part
// of this unit.
class CDmaTag;
class CDmaPacket;
struct _spritepolyvert;

struct CColor {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
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

    static void DrawPolyNoTex(_spritepolyvert*, int, EPolyPrimType);
    void DrawPoly(_spritepolyvert*, int, EPolyPrimType);
    void Draw(float, float, CColor);
    static void InitClass(CRenderList*, CScreen*);
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

extern CRenderBin g_SpriteNoTexture;
extern DrawContextView g_DrawContext;
extern CRenderList* g_pRenderList;
extern CScreen* g_pScreen;
extern float g_ScreenXOffset;
extern float g_ScreenYOffset;
extern float g_HeightScale;

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
