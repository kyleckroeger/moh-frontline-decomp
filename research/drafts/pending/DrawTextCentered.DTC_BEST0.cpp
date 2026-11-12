// PENDING: not landable until the maintainer answers State question 8 (per-fragment inline views that
// differ): this fragment's view of the inline DrawText declares scaleY before scaleX; the landed
// DrawTextCenteredF and font_dtor need scaleX first (treated as the declined duplicate-inline form).
// Function: CFont::DrawTextCentered (416 bytes): code exact.
// Source: scratch/loop3/sweep1/DTC_BEST0.cpp (copied 2026-11-12 by loop agent land2 so the work is
// pushed).
// Not in research/drafts/index.tsv; the index draft for the function stays the landable form.
// See research/plans/three-day-loop.md, State, "Waiting on the maintainer".
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

inline float Mul(float a, float b) { float r = a; r *= b; return r; }
class CScreen {
public:
    virtual void vf08();
    virtual void vf0c();
    virtual void vf10();
    virtual void vf14();
    virtual void vf18();
    virtual void vf1c();
    virtual void vf20();
    virtual int GetHeight();
    virtual int GetWidth();
    virtual void vf2c();
    virtual float GetScale();
    static CScreen* GetCurrent();
    float ScaleX(float f) { return Mul((float)GetWidth(), f); }
};

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
        float scaleY;
        float scaleX;
        GetScale(scaleX, scaleY);
        if (strlen(text)) {
            m_font->m_owner = this;
            FONT_drawtexta(m_font, x / scaleX, y / scaleY, (const unsigned char*)text);
        }
    }
    void DrawTextCenteredF(float, const char*, ...);
    void GetScale(float& x, float& y) { x = m_font->m_scaleX; y = m_font->m_scaleY; }
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
    width = screen->ScaleX(g_viewport.m_width);
    viewX = g_viewport.m_x;
    scale = screen->GetScale();
    center = (float)screen->GetWidth();
    center *= viewX;
    center = (width / 2.0f + center) / scale;
    float left;
    float top;
    float right;
    float bottom;
    FONT_getrecta(m_font, (const unsigned char*)text, &left, &top, &right, &bottom);
    left = (center - right / 2.0f) * screen->GetScale();
    DrawText(left, y, text);
}
