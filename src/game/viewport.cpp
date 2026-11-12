// CViewport: a rectangle in normalised screen coordinates. It converts to a
// GX viewport and scissor for a screen, and caches the clip-to-view matrix.
// CViewport, CScreen, CMatrix and CVector3 are named by the mangled symbols;
// members, CScreen's virtual functions and its render-mode view are inferred
// from offsets and are not original.
extern "C" {
void GXSetViewport(float left, float top, float wd, float ht, float nearz, float farz);
void GXSetViewportJitter(float left, float top, float wd, float ht, float nearz, float farz, unsigned long field);
void GXSetScissor(unsigned long left, unsigned long top, unsigned long wd, unsigned long ht);
unsigned long VIGetNextField(void);
}

// The by-value copies are made with lfd/stfd pairs, so this view is 8-byte
// aligned (16 bytes with padding); the original declaration is not known.
#include "game/CVector3.h"


class CMatrix {
public:
    void BuildScale(CVector3);
    void SetPos(CVector3);

    float m[4][4];
};

struct CScreenMode {
    char field0[24];
    unsigned char field18;
};

class CScreen {
public:
    virtual void vf08();
    virtual void vf0C();
    virtual void vf10();
    virtual void vf14();
    virtual void vf18();
    virtual void vf1C();
    virtual void vf20();
    virtual int vf24();
    virtual int vf28();

    char field4[0xAC];
    CScreenMode* m_mode;
};

class CViewport {
public:
    const CMatrix& GetTMClipToView() const;
    void SetViewport(CScreen&) const;
    void SetRect(float, float, float, float);

    float m_left;
    float m_top;
    float m_right;
    float m_bottom;
    float m_width;
    float m_height;
    float field18[2];
    mutable CMatrix m_TMClipToView;
    mutable bool m_TMClipToViewValid;
};

const CMatrix& CViewport::GetTMClipToView() const {
    if (!m_TMClipToViewValid) {
        CVector3 scale(m_right - m_left, m_bottom - m_top, 1.0f);
        CVector3 pos(m_right + m_left - 1.0f, -(m_bottom + m_top - 1.0f), 0.0f);

        m_TMClipToView.BuildScale(scale);
        m_TMClipToView.SetPos(pos);
        m_TMClipToViewValid = true;
    }
    return m_TMClipToView;
}

void CViewport::SetViewport(CScreen& screen) const {
    int width = screen.vf28();
    int height = screen.vf24();
    float left = width * m_left;
    float top = height * m_top;
    float wd = width * m_width;
    float ht = height * m_height;

    if (screen.m_mode->field18)
        GXSetViewportJitter(left, top, wd, ht, 0.0f, 1.0f, VIGetNextField());
    else
        GXSetViewport(left, top, wd, ht, 0.0f, 1.0f);
    GXSetScissor(left, top, wd, ht);
}

void CViewport::SetRect(float left, float top, float right, float bottom) {
    m_left = left;
    m_top = top;
    m_bottom = bottom;
    m_right = right;
    m_width = right - left;
    m_height = bottom - top;
    m_TMClipToViewValid = false;
}
