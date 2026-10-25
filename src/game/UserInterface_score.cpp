// A fragment of UserInterface.cpp (0x800be590): UserInterface::DrawScore draws
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
    float scaleX;
    float scaleY;
    float offsetX;
    unsigned char unknown0c[4];
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
    unsigned char unknown0000[4784];
    int m_team;
};

class CScene {
public:
    CPlayerObject* GetPlayer(int) const;

    unsigned char unknown000[328];
};

class UserInterface {
public:
    void DrawScore();

    static CSprite* starSprite;
    static CFont* smallMPFont;

    unsigned char unknown000[2072];
    int m_player;
    float m_timeLeft;
    float m_timeFade;
};

extern CScene g_scene;
extern CShellMenu g_Shell;
extern ViewportView g_viewport;
extern CColor teamColors[4];
extern _spritepolyvert StarSpriteVerts[4];
extern _spritepolyvert TempVerts[24];

void UserInterface::DrawScore() {
    static char buffer[80] = ""; // in .data in the original (explicitly initialised)
    CScreen* screen = CScreen::GetCurrent();
    int width = screen->vf28();
    int height = screen->vf24();
    float viewWidth = g_viewport.width;
    int right = (int)(width * viewWidth);
    int top = (int)(height * g_viewport.scaleY);
    int left = (int)(width * g_viewport.scaleX);
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
