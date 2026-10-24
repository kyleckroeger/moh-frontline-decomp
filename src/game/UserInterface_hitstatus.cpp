// A fragment of UserInterface.cpp (0x800bbbc0): UserInterface::DrawHitStatus
// draws the four hit-direction sprites whose hit level (+1960) is positive:
// each sprite's three vertices are copied to the shared temporary vertices,
// given an alpha of 96 times the HUD fade (+1992), the HUD alpha (+1984) and
// the hit level clamped to 1, and scaled by the screen's scale, or, in
// multiplayer, placed in the viewport (scaled by 0.75 with more than two
// players, offset by the screen size times the viewport's scale and moved 16
// units up); then they are drawn as one triangle. The classes, functions and
// globals are named by the mangled symbols; the screen's virtual slots (+36,
// +40) are unnamed placeholders, and the members, the vertex, viewport and
// screen views are inferred from offsets. The multiplayer scale is a
// conditional expression whose first operand is the default (the compiler
// keeps the branch to its empty copy). The constants are entries of the
// file's .sdata2 pool. The rest of the file is in other units.
extern "C" void* memcpy(void*, const void*, unsigned long);

struct _spritepolyvert {
    unsigned char unknown00[8];
    float x;
    float y;
    unsigned char unknown10[7];
    unsigned char alpha;
};

class CSprite {
public:
    enum EPolyPrimType {};
    void DrawPoly(_spritepolyvert*, int, EPolyPrimType);
};

class CScreen {
public:
    static CScreen* GetCurrent();
    virtual void vf08();
    virtual void vf0c();
    virtual void vf10();
    virtual void vf14();
    virtual void vf18();
    virtual void vf1c();
    virtual void vf20();
    virtual int vf24();
    virtual int vf28();

    unsigned char unknown04[36];
    float m_scaleX;
    float m_scaleY;
};

struct ViewportView {
    float scaleX;
    float scaleY;
    unsigned char unknown08[8];
    float width;
    float height;
};

class UserInterface {
public:
    void DrawHitStatus();
    static CSprite* hitMeterSprite;

    unsigned char unknown000[1960];
    float m_hits[4];
    unsigned char unknown7b8[8];
    float m_alpha;
    unsigned char unknown7c4[4];
    float m_fade;
};

extern bool g_bInMultiplayerMode;
extern int g_NumPlayers;
extern CScreen g_screen;
extern ViewportView g_viewport;
extern _spritepolyvert HitSpriteVerts[4][3];
extern _spritepolyvert TempVerts[3];

void UserInterface::DrawHitStatus() {
    CScreen* screen = CScreen::GetCurrent();
    int width = screen->vf28();
    int height = screen->vf24();
    float viewWidth = g_viewport.width;
    float viewHeight = g_viewport.height;
    float scale = 1.0f;
    if (g_bInMultiplayerMode)
        scale = (g_NumPlayers <= 2) ? 1.0f : 0.75f;
    for (int i = 0; i < 4; i++) {
        if (m_hits[i] > 0.0f) {
            memcpy(TempVerts, HitSpriteVerts[i], sizeof(TempVerts));
            unsigned char alpha = 96.0f * (m_fade * m_alpha * (m_hits[i] > 1.0f ? 1.0f : m_hits[i]));
            for (int j = 0; j < 3; j++) {
                TempVerts[j].alpha = alpha;
                if (!g_bInMultiplayerMode) {
                    TempVerts[j].x *= g_screen.m_scaleX;
                    TempVerts[j].y *= g_screen.m_scaleY;
                } else {
                    TempVerts[j].x = 92.0f * viewWidth + (scale * (TempVerts[j].x - 92.0f) + width * g_viewport.scaleX);
                    TempVerts[j].y = 384.0f * viewHeight + (scale * (TempVerts[j].y - 384.0f) + height * g_viewport.scaleY) - 16.0f;
                }
            }
            hitMeterSprite->DrawPoly(TempVerts, 3, (CSprite::EPolyPrimType)3);
        }
    }
}
