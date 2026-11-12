// A fragment of pumhandler.cpp (0x800b9a00): PopUpMessageHandler_Loop.
extern "C" void FONT_getrecta(void*, const char*, float*, float*, float*, float*);

struct CColor {
    CColor(unsigned char ar, unsigned char ag, unsigned char ab, unsigned char aa) : r(ar), g(ag), b(ab), a(aa) {}

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
    unsigned long color;
};

class CSprite {
public:
    enum EPolyPrimType {};
    void DrawPoly(_spritepolyvert*, int, EPolyPrimType);
};

typedef CSprite::EPolyPrimType EPolyPrimType;

class CFont {
public:
    void SetScale(float, float);
    void GetScale(float&, float&);
    void SetColor(CColor);
    void DrawText(float, float, const char*);
    void DrawTextCentered(float, const char*);

    unsigned char unknown00[32];
    void* m_font;
};

class CScreen {
public:
    static CScreen* GetCurrent();
    int GetWidth();
    int GetHeight();
    virtual void unknown08();
    virtual void unknown0c();
    virtual void unknown10();
    virtual void unknown14();
    virtual void unknown18();
    virtual void unknown1c();
    virtual void unknown20();
    virtual int GetY();
    virtual int GetX();

    unsigned char unknown04[40];
    float m_scale;
    unsigned char unknown30[224];
};

extern CScreen g_screen;

/* inferred: the viewport's scale and offsets */
struct ViewportView {
    float m_x;
    float m_y;
    unsigned char unknown08[8];
    float m_scaleX;
    float m_scaleY;
    unsigned char unknown18[88];
};

extern ViewportView g_viewport;

class Pause {
public:
    bool bIsGamePaused();

    unsigned char unknown00[152];
};

extern Pause g_Pause;

class CPlayerObject {
public:
    unsigned char unknown0000[4720];
    bool m_hideMessages;
};

class CScene {
public:
    CPlayerObject* GetPlayer(int) const;

    unsigned char unknown000[328];
};

extern CScene g_scene;
extern bool g_bInMultiplayerMode;
extern int g_NumPlayers;
extern int POPUPS_IN_SECTION;
extern float MP_PUM_TEXT_SCALE_X_2P;
extern float MP_PUM_TEXT_SCALE_Y_2P;
extern float MP_PUM_TEXT_SCALE_X_4P;
extern float MP_PUM_TEXT_SCALE_Y_4P;

class PopUpMessage {
public:
    char* GetString();
    short Get_top();
    unsigned char Get_a();
    unsigned char Get_r();
    float Get_onScreenTime();
    unsigned char Get_property();
    unsigned short Get_screenLocY();
    unsigned short Get_screenLocX();
    bool Available();
    void Scroll();
    void UpdateDisplayTime(float);
    void Reset();

    unsigned char unknown00[76];
};

class PopUpMessageHandler {
public:
    void PopUpMessageHandler_Loop();

    PopUpMessage m_messages[5][4];
    unsigned char unknown5f0[4];
    CFont* m_font;
    CFont* m_subtitleFont;
    CSprite* m_background;
    CSprite* m_background2;
    float m_speed;
    bool m_showBackground;
};

void PopUpMessageHandler::PopUpMessageHandler_Loop() {
    PopUpMessageHandler* const self = this;

    float scaleX;
    float top;
    float bottom;
    CScreen* screen;
    int screenX;
    int screenY;
    float left;
    float right;
    float scaleY;
    float x;
    float y;
    float w;
    float h;
    float fontScaleX;
    float fontScaleY;
    int count;
    int i;
    float sy0;
    float sy1;
    float sy2;
    float sy3;
    float elapsed;
    short minTop;
    short topSlot;
    short slot;
    int section;
    left = 0.0f;
    right = (float)g_screen.GetWidth();
    top = 322.0f * g_screen.m_scale;
    bottom = 448.0f * g_screen.m_scale;
    screen = CScreen::GetCurrent();
    screenX = screen->GetX();
    screenY = screen->GetY();
    scaleX = g_viewport.m_scaleX;
    elapsed = (1.0f / 59.94f) * m_speed;
    scaleY = g_viewport.m_scaleY;
    if (g_bInMultiplayerMode) {
        float offsetX = (float)screenX * g_viewport.m_x;
        left = 10.0f * scaleX + offsetX;
        right = (right - 10.0f) * scaleX + offsetX;
        float offsetY = (float)screenY * g_viewport.m_y;
        top = top * scaleY + offsetY;
        bottom = bottom * scaleY + offsetY;
    }
    switch (g_NumPlayers) {
    case 1:
        m_font->SetScale(1.0f, 1.0f);
        break;
    case 2:
        m_font->SetScale(MP_PUM_TEXT_SCALE_X_2P, MP_PUM_TEXT_SCALE_Y_2P);
        break;
    case 3:
    case 4:
        m_font->SetScale(MP_PUM_TEXT_SCALE_X_4P, MP_PUM_TEXT_SCALE_Y_4P);
        break;
    }
    if (m_showBackground) {
        count = 0;
        for (i = 0; i < POPUPS_IN_SECTION; i++) {
            if (!m_messages[4][i].Available())
                count++;
        }
        if (count > 0) {
            int shrink = (4 - count) * 22;
            _spritepolyvert verts[4] = {
                { 0.0f, 0.0f, left, 0.0f, 0.0f, 0x80808040 },
                { 1.0f, 0.0f, right, 0.0f, 0.0f, 0x80808040 },
                { 1.0f, 1.0f, right, 110.0f * g_screen.m_scale, 0.0f, 0x80808040 },
                { 0.0f, 1.0f, left, 110.0f * g_screen.m_scale, 0.0f, 0x80808040 },
            };
            sy0 = g_viewport.m_y;
            verts[0].y += sy0 * (float)g_screen.GetHeight();
            sy1 = g_viewport.m_y;
            verts[1].y += sy1 * (float)g_screen.GetHeight();
            sy2 = g_viewport.m_y;
            verts[2].y += sy2 * (float)g_screen.GetHeight() - (float)shrink;
            sy3 = g_viewport.m_y;
            verts[3].y += sy3 * (float)g_screen.GetHeight() - (float)shrink;
            m_background->DrawPoly(verts, 4, (EPolyPrimType)5);
        }
        count = 0;
        for (int j = 0; j < POPUPS_IN_SECTION; j++) {
            if (!m_messages[1][j].Available())
                count++;
        }
        if (count > 0 && !g_scene.GetPlayer(0)->m_hideMessages && !g_bInMultiplayerMode) {
            _spritepolyvert verts[4] = {
                { 0.0f, 0.0f, left, top, 0.0f, 0x80808040 },
                { 1.0f, 0.0f, right, top, 0.0f, 0x80808040 },
                { 1.0f, 1.0f, right, bottom, 0.0f, 0x80808040 },
                { 0.0f, 1.0f, left, bottom, 0.0f, 0x80808040 },
            };
            m_background2->DrawPoly(verts, 4, (EPolyPrimType)5);
        }
    }
    topSlot = -1;
    minTop = 2345;
    PopUpMessage* topMessage = m_messages[4];
    for (slot = 0; slot < POPUPS_IN_SECTION; topMessage++, slot++) {
        if (!topMessage->Available()) {
            int messageTop = topMessage->Get_top();
            if (messageTop < minTop) {
                topSlot = slot;
                minTop = messageTop;
            }
        }
    }
    for (section = 0; section < 5; section++) {
        for (int i = 0; i < POPUPS_IN_SECTION; i++) {
            PopUpMessage* message = &m_messages[section][i];
            if (!message->Available()) {
                switch (message->Get_property()) {
                case 1:
                    if (!m_showBackground)
                        break;
                    if (section != 1) {
                        if (!g_scene.GetPlayer(0)->m_hideMessages) {
                            if (i != topSlot || g_bInMultiplayerMode) {
                                if (message->Get_r() != 60)
                                    m_font->SetColor(CColor(35, 35, 35, 128));
                                else
                                    m_font->SetColor(CColor(60, 60, 40, 128));
                            } else {
                                if (message->Get_r() != 60)
                                    m_font->SetColor(CColor(55, 55, 55, 128));
                                else
                                    m_font->SetColor(CColor(60, 60, 40, 128));
                            }
                            void* font = m_font->m_font;
                            FONT_getrecta(font, message->GetString(), &x, &y, &w, &h);
                            m_font->GetScale(fontScaleX, fontScaleY);
                            if (g_bInMultiplayerMode) {
                                static int MP_PUM_LEFT_JUSTIFY = 30;
                                float textX = scaleX * MP_PUM_LEFT_JUSTIFY + (float)screenX * g_viewport.m_x;
                                unsigned short locY = message->Get_screenLocY();
                                float textY = fontScaleY * locY + (float)screenY * g_viewport.m_y;
                                textY += 8.0f;
                                m_font->DrawText(textX, textY, message->GetString());
                            } else {
                                m_font->DrawTextCentered((float)message->Get_screenLocY(), message->GetString());
                            }
                        }
                    } else {
                        x = (float)message->Get_screenLocX();
                        m_subtitleFont->SetColor(CColor(255, 255, 255, message->Get_a()));
                        m_subtitleFont->DrawText(x, (float)message->Get_screenLocY(), message->GetString());
                    }
                    break;
                case 4:
                    message->Scroll();
                    break;
                case 7:
                    break;
                case 8:
                    message = (PopUpMessage*)(void*)message;
                }
                if (!g_Pause.bIsGamePaused()) {
                    if (message->Get_onScreenTime() < 0.0f)
                        message->Reset();
                    message->UpdateDisplayTime(elapsed);
                }
            }
        }
    }
}
