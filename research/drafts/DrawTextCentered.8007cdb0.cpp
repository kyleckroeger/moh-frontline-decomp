namespace std {
struct __tag_va_List {
    char gpr;
    char fpr;
    char reserved[2];
    char* input_arg_area;
    char* reg_save_area;
};
typedef __tag_va_List va_list[1];
}

extern "C" int vsprintf(char*, const char*, std::va_list);
extern "C" unsigned long strlen(const char*);
struct FONT;
extern "C" int FONT_drawtexta(FONT*, float, float, const unsigned char*);
extern "C" int FONT_getrecta(FONT*, const unsigned char*, float*, float*, float*, float*);

class CFont;

struct FONT {
    unsigned char unknown00[56];
    float m_scaleX;
    float m_scaleY;
    unsigned char unknown40[48];
    CFont* m_owner;
};

#include "game/CScreen.h"


struct ViewportView {
    float m_x;
    unsigned char unknown04[12];
    float m_width;
    unsigned char unknown14[92];
};

extern ViewportView g_viewport;

class CRenderBin {
    unsigned char unknown00[28];

public:
    virtual ~CRenderBin();
};

class CFont : public CRenderBin {
public:
    virtual ~CFont();
    void DrawText(float x, float y, const char* text) {
        float scaleX = m_font->m_scaleX;
        float scaleY = m_font->m_scaleY;
        if (strlen(text)) {
            m_font->m_owner = this;
            FONT_drawtexta(m_font, x / scaleX, y / scaleY, (const unsigned char*)text);
        }
    }
    void DrawTextCenteredF(float, const char*, ...);
    void DrawTextCentered(float, const char*);

    FONT* m_font;
};

inline CScreen* CurrentScreen() {
    return CScreen::GetCurrent();
}

void CFont::DrawTextCentered(float y, const char* text) {
    CScreen* screen = CurrentScreen();
    float center;
    float scale;
    float viewWidth;
    float viewX;
    float width;
    viewWidth = g_viewport.m_width;
    width = (float)screen->GetWidth() * viewWidth;
    viewX = g_viewport.m_x;
    scale = screen->GetWidthScale();
    center = (width / 2.0f + (float)screen->GetWidth() * viewX) / scale;
    float left;
    float top;
    float right;
    float bottom;
    FONT_getrecta(m_font, (const unsigned char*)text, &left, &top, &right, &bottom);
    left = (center - right / 2.0f) * screen->GetWidthScale();
    DrawText(left, y, text);
}
