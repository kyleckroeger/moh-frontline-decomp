// A fragment of UserInterface.cpp (0x800bd590): UserInterface::DrawWeaponName
// draws a non-empty weapon name with the player's pop-up message font (the
// handler of the player's index): it saves the font's colour and scale, sets a
// grey colour whose alpha is 96 times the name fade (+2000) and the HUD alpha
// (+1984) and a unit scale, measures the text (its width scaled by the
// screen's virtual at +48), and draws it right-aligned 56 units from the
// screen's edge at height 356 (both scaled by the screen's scale), or in a
// multiplayer viewport placed by its scale, offset and size (15 units
// higher); then it restores the scale and the colour. The classes, functions
// and globals are named by the mangled symbols; the screen's virtual slots
// (+36, +40, +48) are unnamed placeholders, and the members, the colour view
// (as in fader.cpp: member-wise assignment, block copies), the viewport,
// pop-up handler and player views are inferred from offsets. The constants
// are entries of the file's .sdata2 pool. The rest of the file is in other
// units.
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

extern "C" void FONT_getrecta(int, const char*, float*, float*, float*, float*);

class CFont {
public:
    CColor GetColor();
    void SetColor(CColor);
    void GetScale(float&, float&);
    void SetScale(float, float);
    void DrawText(float, float, const char*);

    unsigned char unknown00[32];
    int m_font;
};

class CScreen {
public:
    static CScreen* GetCurrent();
    int GetWidth();
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

struct PopUpMessageHandlerView {
    unsigned char unknown000[1528];
    CFont* font;
};

class CPlayerObject {
public:
    unsigned char unknown000[1896];
    int m_index;
};

class CScene {
public:
    CPlayerObject* GetPlayer(int) const;

    unsigned char unknown000[328];
};

class UserInterface {
public:
    void DrawWeaponName(const char*);

    unsigned char unknown000[1984];
    float m_alpha;
    unsigned char unknown7c4[12];
    float m_nameFade;
    unsigned char unknown7d4[68];
    int m_player;
};

extern bool g_bInMultiplayerMode;
extern CScene g_scene;
extern CScreen g_screen;
extern ViewportView g_viewport;
extern PopUpMessageHandlerView* g_pPopUpMessageHandler[];

void UserInterface::DrawWeaponName(const char* name) {
    if (*name) {
        CFont* font = g_pPopUpMessageHandler[g_scene.GetPlayer(m_player)->m_index]->font;
        CColor saved;
        saved = font->GetColor();
        font->SetColor(CColor(128, 128, 128, (unsigned char)(96.0f * m_nameFade * m_alpha)));
        CScreen* screen = CScreen::GetCurrent();
        int width = screen->vf28();
        int height = screen->vf24();
        float viewWidth = g_viewport.width;
        float viewHeight = g_viewport.height;
        float scaleX;
        float scaleY;
        font->GetScale(scaleX, scaleY);
        if (!g_bInMultiplayerMode)
            font->SetScale(1.0f, 1.0f);
        else
            font->SetScale(1.0f, 1.0f);
        float x;
        float y;
        float w;
        float h;
        FONT_getrecta(font->m_font, name, &x, &y, &w, &h);
        w *= screen->vf30();
        if (!g_bInMultiplayerMode) {
            float sx = g_screen.m_scaleX;
            x = g_screen.GetWidth() - w - 56.0f * sx;
            font->DrawText(x, 356.0f * g_screen.m_scaleY, name);
        } else {
            x = width * g_viewport.offsetX - w - viewWidth * (56.0f * g_screen.m_scaleX);
            y = viewHeight * (356.0f * g_screen.m_scaleY) + height * g_viewport.scaleY - 15.0f;
            font->DrawText(x, y, name);
        }
        font->SetScale(scaleX, scaleY);
        font->SetColor(saved);
    }
}
