// CScreen (0x8001b7e8): the empty PutDisp, Clear (copying the display to the
// buffer at +248), the clear rectangle and colour getters, GetClear (false
// while capturing), the depth buffer's maximum by depth format (16, 24 or 28
// bits), the depth and frame formats (returned by address), the width and
// height, the clear rectangle, colour and flag setters, the vertical sync
// count and Wait (waiting while a swap is pending, updating the fader by the
// frames since the last wait, which it returns, switching the copy buffer
// between the two frame buffers, setting the viewport - with jitter for a
// field render mode - and re-enabling colour updates when not clearing), and
// Flip (depth and colour state, the clear colour, copying the display to the
// copy buffer and showing it, blanking until the first frame, then marking a
// swap pending).
// CScreen, CRect, CColor, CFader and the functions are named by the mangled
// symbols; the members, the format records (their first word the format
// type), the render mode view, the rectangle and colour layouts and the result
// types are inferred; g_bSwap is read as volatile (the wait reloads it).
// The rest of the file is not part of this unit.
struct CRect {
    int left;
    int top;
    int right;
    int bottom;
};

struct CColor {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

extern "C" {
void GXCopyDisp(void*, int);
unsigned long VIGetNextField();
void GXSetViewportJitter(float, float, float, float, float, float, unsigned long);
void GXSetViewport(float, float, float, float, float, float);
void GXInvalidateVtxCache();
void GXSetColorUpdate(int);
void GXSetZMode(int, int, int);
void GXSetCopyClear(CColor, unsigned long);
void GXDrawDone();
void VISetNextFrameBuffer(void*);
void VISetBlack(int);
void VIFlush();
}

class CFader {
public:
    static void Update(float);
};

// Inferred: the render mode record the screen points to.
struct ScreenModeView {
    unsigned char unknown00[4];
    unsigned short width;
    unsigned short height;
    unsigned char unknown08[16];
    unsigned char field;
};

extern volatile bool g_bSwap;
extern unsigned long g_WaitFrameNum;

extern bool g_bCapture;
extern unsigned long g_FrameNum;

struct ScreenFormatView {
    int type;
};

class CScreen {
public:
    void PutDisp();
    void Clear();
    CRect* GetClearRect();
    CColor* GetClearColor();
    bool GetClear();
    unsigned long GetDepthBufferMax();
    void SetClearRect(const CRect&);
    void SetClearColor(const CColor&);
    void SetClear(bool);
    unsigned long GetVSyncs();
    unsigned long Wait();
    bool Flip();
    ScreenFormatView* GetDepthFormat();
    ScreenFormatView* GetFrameFormat();
    int GetWidth();
    int GetHeight();

    unsigned char unknown00[5];
    bool m_clear;
    unsigned char unknown06[2];
    CRect m_clearRect;
    CColor m_clearColor;
    unsigned char unknown1c[4];
    int m_width;
    int m_height;
    unsigned char unknown28[8];
    ScreenFormatView m_frameFormat;
    ScreenFormatView m_depthFormat;
    unsigned char unknown38[120];
    ScreenModeView* m_mode;
    unsigned char unknownb4[60];
    void* m_frameBuffer0;
    void* m_frameBuffer1;
    void* m_copyBuffer;
};

void CScreen::PutDisp() {
}

void CScreen::Clear() {
    GXCopyDisp(m_copyBuffer, 1);
}

CRect* CScreen::GetClearRect() {
    return &m_clearRect;
}

CColor* CScreen::GetClearColor() {
    return &m_clearColor;
}

bool CScreen::GetClear() {
    if (g_bCapture)
        return false;
    return m_clear;
}

unsigned long CScreen::GetDepthBufferMax() {
    switch (m_depthFormat.type) {
    case 0:
        return 0xffff;
    case 1:
        return 0xffffff;
    case 2:
        return 0xfffffff;
    default:
        return 0xfffffff;
    }
}

ScreenFormatView* CScreen::GetDepthFormat() {
    return &m_depthFormat;
}

ScreenFormatView* CScreen::GetFrameFormat() {
    return &m_frameFormat;
}

int CScreen::GetWidth() {
    return m_width;
}

int CScreen::GetHeight() {
    return m_height;
}

void CScreen::SetClearRect(const CRect& rect) {
    m_clearRect = rect;
}

void CScreen::SetClearColor(const CColor& color) {
    m_clearColor = color;
}

void CScreen::SetClear(bool clear) {
    m_clear = clear;
}

unsigned long CScreen::GetVSyncs() {
    return g_FrameNum;
}

unsigned long CScreen::Wait() {
    while (g_bSwap == 1) {
    }
    unsigned long frames = g_FrameNum - g_WaitFrameNum;
    g_WaitFrameNum = g_FrameNum;
    CFader::Update(frames);
    if (m_copyBuffer == m_frameBuffer0)
        m_copyBuffer = m_frameBuffer1;
    else
        m_copyBuffer = m_frameBuffer0;
    if (m_mode->field)
        GXSetViewportJitter(0.0f, 0.0f, m_mode->width, m_mode->height, 0.0f, 1.0f, VIGetNextField());
    else
        GXSetViewport(0.0f, 0.0f, m_mode->width, m_mode->height, 0.0f, 1.0f);
    GXInvalidateVtxCache();
    if (!m_clear)
        GXSetColorUpdate(1);
    return frames;
}

bool CScreen::Flip() {
    GXSetZMode(1, 3, 1);
    GXSetColorUpdate(m_clear != 0);
    CColor color = {m_clearColor.r, m_clearColor.g, m_clearColor.b, 0};
    GXSetCopyClear(color, 0xffffff);
    GXCopyDisp(m_copyBuffer, 1);
    GXDrawDone();
    VISetNextFrameBuffer(m_copyBuffer);
    static bool DemoFirstFrame = true;
    if (DemoFirstFrame) {
        VISetBlack(0);
        DemoFirstFrame = false;
    }
    VIFlush();
    g_bSwap = 1;
    return false;
}
