// A fragment of UserInterface.cpp (0x800be108): UserInterface::DrawViewportEdge
// outlines the player's viewport in split screen: LetterBoxVerts copied into
// TempVerts in the player's team colour, then a grey quad along each edge that
// borders another viewport (top/bottom only with two players).
// UserInterface::DrawScore draws
// the multiplayer score in the viewport's top-right corner: the player's score
// (with the score limit, "%d / %d", when the game uses one) in the player's
// team colour at unit scale, right-aligned 54 viewport-width units from the
// viewport's right edge, a star (StarSpriteVerts scaled by 0.75 into
// TempVerts) beside it when the player leads, and, while the time-left fade
// is positive, the remaining time as minutes and seconds centred below it
// (fading in over the first two units); then it restores the font's scale.
// The text is formatted into a function-static buffer. The classes,
// functions and globals are named by the mangled symbols; the screen's
// virtual slots (+36, +40, +48) are unnamed placeholders, and the members,
// the colour view (as in UserInterface_weaponname.cpp), the viewport, stats,
// shell and player views and the star loop are inferred from offsets. The
// rest of the file is in other units.
struct CColor {
    CColor() {}
    CColor(unsigned char ar, unsigned char ag, unsigned char ab, unsigned char aa) : r(ar), g(ag), b(ab), a(aa) {}
    CColor& operator=(const CColor& c) {
        r = c.r;
        g = c.g;
        b = c.b;
        a = c.a;
        return *this;
    }

    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

extern "C" void* memcpy(void*, const void*, unsigned long);
extern "C" int sprintf(char*, const char*, ...);
extern "C" double floor(double);
extern "C" void FONT_getrecta(int, const char*, float*, float*, float*, float*);

class CFont {
public:
    void SetColor(CColor);
    void GetScale(float&, float&);
    void SetScale(float, float);
    void DrawText(float, float, const char*);
    void DrawTextCentered(float, const char*);

    unsigned char unknown00[32];
    int m_font;
};

struct _spritepolyvert {
    unsigned char unknown00[8];
    float x;
    float y;
    unsigned char unknown10[4];
    CColor color;
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
    virtual void vf2c();
    virtual float vf30();

    unsigned char unknown04[36];
    float m_scaleX;
    float m_scaleY;
};

struct ViewportView {
    float left;
    float top;
    float right;
    float bottom;
    float width;
    float height;
};

struct MPStatsView {
    unsigned char unknown000[240];
    int score;
    unsigned char unknown0f4[8];
    unsigned char leader;
    unsigned char unknown0fd[3];
};

class CShellMenu {
public:
    MPStatsView* GetMPPlayerStats(int);

    unsigned char unknown0000[5160];
    int m_scoreLimit;
    unsigned char unknown142c[4];
    int m_useScoreLimit;
    unsigned char unknown1434[1448];
};

class CPlayerObject {
public:
    unsigned char unknown0000[1896];
    int m_index;
    unsigned char unknown076c[2884];
    int m_team;
};

class CScene {
public:
    CPlayerObject* GetPlayer(int) const;

    unsigned char unknown000[328];
};

class UserInterface {
public:
    void DrawViewportEdge();
    void DrawScore();

    static CSprite* greySprite;
    static CSprite* starSprite;
    static CFont* smallMPFont;

    unsigned char unknown000[2072];
    int m_player;
    float m_timeLeft;
    float m_timeFade;
};

extern int g_NumPlayers;
extern CScene g_scene;
extern CShellMenu g_Shell;
extern ViewportView g_viewport;
extern CColor teamColors[4];
extern _spritepolyvert LetterBoxVerts[4];
extern _spritepolyvert StarSpriteVerts[4];
extern _spritepolyvert TempVerts[24];

static inline CColor TeamColor(int team) {
    unsigned char b;
    unsigned char g;
    unsigned char r;
    r = 0;
    g = 0;
    b = 0;
    switch (team) {
    case 0:
        r = teamColors[0].r;
        g = teamColors[0].g;
        b = teamColors[0].b;
        break;
    case 1:
        r = teamColors[1].r;
        g = teamColors[1].g;
        b = teamColors[1].b;
        break;
    case 2:
        r = teamColors[2].r;
        g = teamColors[2].g;
        b = teamColors[2].b;
        break;
    case 3:
        r = teamColors[3].r;
        g = teamColors[3].g;
        b = teamColors[3].b;
        break;
    }
    return CColor(r, g, b, 255);
}

/* inferred: which edges of a player's viewport border another viewport */
static inline bool IsTop(int index) {
    return g_NumPlayers == 2 ? index == 1 ? 1 : 0 : (bool)(index == 2 || index == 3) != 0;
}

static inline bool IsBottom(int index) {
    return g_NumPlayers == 2 ? index == 0 ? 1 : 0 : (bool)(index == 0 || index == 1) != 0;
}

static inline bool IsLeft(int index) {
    return g_NumPlayers == 2 ? false ? 1 : 0 : (bool)(index == 1 || index == 3) != 0;
}

static inline bool IsRight(int index) {
    return g_NumPlayers == 2 ? false ? 1 : 0 : (bool)(index == 2 || index == 0) != 0;
}

void UserInterface::DrawViewportEdge() {
    memcpy(TempVerts, LetterBoxVerts, sizeof(_spritepolyvert) * 4);
    CPlayerObject* player = g_scene.GetPlayer(m_player);
    CColor color = TeamColor(player->m_team);
    for (int i = 0; i < 4; i++)
        TempVerts[i].color = color;
    CScreen* screen = CScreen::GetCurrent();
    int width = screen->vf28();
    int height = screen->vf24();
    bool top = IsTop(player->m_index);
    bool bottom = IsBottom(player->m_index);
    bool left = IsLeft(player->m_index);
    bool right = IsRight(player->m_index);
    float x0 = width * g_viewport.left;
    float x1 = width * g_viewport.right - 1.0f;
    float y0 = height * g_viewport.top;
    float y1 = height * g_viewport.bottom - 1.0f;
    if (top) {
        TempVerts[3].x = x0;
        TempVerts[0].x = x0;
        TempVerts[2].x = x1;
        TempVerts[1].x = x1;
        TempVerts[1].y = y0;
        TempVerts[0].y = y0;
        TempVerts[3].y = y0 + 2.0f;
        TempVerts[2].y = y0 + 2.0f;
        greySprite->DrawPoly(TempVerts, 4, (CSprite::EPolyPrimType)5);
    }
    if (bottom) {
        TempVerts[3].x = x0;
        TempVerts[0].x = x0;
        TempVerts[2].x = x1;
        TempVerts[1].x = x1;
        TempVerts[1].y = y1 - 2.0f;
        TempVerts[0].y = y1 - 2.0f;
        TempVerts[3].y = y1;
        TempVerts[2].y = y1;
        greySprite->DrawPoly(TempVerts, 4, (CSprite::EPolyPrimType)5);
    }
    if (left) {
        TempVerts[1].y = y0;
        TempVerts[0].y = y0;
        TempVerts[3].x = x0 + 1.0f;
        TempVerts[0].x = x0 + 1.0f;
        TempVerts[2].x = x0 + 5.0f;
        TempVerts[1].x = x0 + 5.0f;
        TempVerts[3].y = y1;
        TempVerts[2].y = y1;
        greySprite->DrawPoly(TempVerts, 4, (CSprite::EPolyPrimType)5);
    }
    if (right) {
        TempVerts[2].x = x1;
        TempVerts[1].x = x1;
        TempVerts[3].x = x1 - 4.0f;
        TempVerts[0].x = x1 - 4.0f;
        TempVerts[1].y = y0;
        TempVerts[0].y = y0;
        TempVerts[3].y = y1;
        TempVerts[2].y = y1;
        greySprite->DrawPoly(TempVerts, 4, (CSprite::EPolyPrimType)5);
    }
}

void UserInterface::DrawScore() {
    static char buffer[80] = ""; // in .data in the original (explicitly initialised)
    CScreen* screen = CScreen::GetCurrent();
    int width = screen->vf28();
    int height = screen->vf24();
    float viewWidth = g_viewport.width;
    int right = (int)(width * viewWidth);
    int top = (int)(height * g_viewport.top);
    int left = (int)(width * g_viewport.left);
    MPStatsView* stats = g_Shell.GetMPPlayerStats(m_player);
    if (g_Shell.m_useScoreLimit)
        sprintf(buffer, "%d / %d", stats->score, g_Shell.m_scoreLimit);
    else
        sprintf(buffer, "%d", stats->score);
    float scaleX;
    float scaleY;
    smallMPFont->GetScale(scaleX, scaleY);
    CPlayerObject* player = g_scene.GetPlayer(m_player);
    smallMPFont->SetScale(1.0f, 1.0f);
    float x;
    float y;
    float w;
    float h;
    FONT_getrecta(smallMPFont->m_font, buffer, &x, &y, &w, &h);
    w *= screen->vf30();
    float margin = 54.0f * viewWidth;
    int textX = (int)((float)(left + right) - margin - (float)(int)w);
    int textY = top + 16;
    smallMPFont->SetColor(teamColors[player->m_team]);
    smallMPFont->DrawText(textX, textY, buffer);
    if (stats->leader) {
        memcpy(TempVerts, StarSpriteVerts, sizeof(StarSpriteVerts));
        for (int i = 0; i < 4; i++) {
            TempVerts[i].x = (float)right + (0.75f * TempVerts[i].x + (float)left) - margin - 12.0f;
            TempVerts[i].y = 12.0f + (12.0f + (h + (0.75f * TempVerts[i].y + (float)textY)));
        }
        starSprite->DrawPoly(TempVerts, 4, (CSprite::EPolyPrimType)5);
    }
    if (m_timeFade > 0.0f) {
        float minutes = floor(m_timeLeft);
        float seconds = m_timeLeft - minutes;
        if (m_timeFade < 2.0f)
            smallMPFont->SetColor(CColor(255, 255, 255, (unsigned char)(0.5f * (128.0f * m_timeFade))));
        else
            smallMPFont->SetColor(CColor(255, 255, 255, 128));
        sprintf(buffer, "%d:%02d remaining", (int)minutes, (int)(60.0f * seconds));
        smallMPFont->DrawTextCentered(top + 72, buffer);
    }
    smallMPFont->SetScale(scaleX, scaleY);
}
