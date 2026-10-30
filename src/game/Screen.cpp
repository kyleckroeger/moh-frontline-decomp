// Screen.cpp (0x8001b12c..0x8001bfbc): the screen classes. CScreen holds the
// render mode, the two frame buffers and the clear state: SetCurrent sets up
// the graphics FIFO, video and copy state (as DEMOInit does), Flip and Wait
// swap and pace the buffers, VBlankHandler shows the finished buffer on the
// vertical retrace, and GetTMViewToScreen builds the cached view-to-screen
// transform. CScreenFieldMode is the field-rendered mode; its _Capture writes
// the copy buffer to an .ssh image file. The classes and functions are named
// by the mangled symbols; the members, records and helper types are inferred.
//
// The file is compiled with deferred inlining, which emits the functions last
// to first, so the source is in reverse image order.
//
// CScreenFrameMode (a CScreen) and CScreenFieldModePAL (a CScreenFieldMode)
// were defined in this file too, but the original linker removed their code,
// virtual tables and type records; only their type names survive, in the
// file's string block that SetCurrent addresses by offset. They are kept here
// as minimal stubs (empty bodies) solely so that the block holds those names
// in their places. The build discards them again (`linker_stripped` in the
// manifest).
typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned char u8;

struct GXRenderModeObj {
    u32 viTVmode;
    u16 fbWidth;
    u16 efbHeight;
    u16 xfbHeight;
    u16 viXOrigin;
    u16 viYOrigin;
    u16 viWidth;
    u16 viHeight;
    u32 xFBmode;
    u8 field_rendering;
    u8 aa;
    u8 sample_pattern[12][2];
    u8 vfilter[7];
};

struct CRect {
    CRect() {}
    CRect(int l, int t, int r, int b) : left(l), top(t), right(r), bottom(b) {}
    int left;
    int top;
    int right;
    int bottom;
};

struct GXColor {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

struct CColor {
    CColor() {}
    CColor(unsigned char ar, unsigned char ag, unsigned char ab, unsigned char aa) : r(ar), g(ag), b(ab), a(aa) {}
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

extern "C" {
extern GXRenderModeObj GXNtsc480IntDf;
void GXAdjustForOverscan(GXRenderModeObj*, GXRenderModeObj*, u16, u16);
void VIConfigure(GXRenderModeObj*);
void* GXInit(void*, u32);
void GXSetViewport(float, float, float, float, float, float);
void GXSetViewportJitter(float, float, float, float, float, float, unsigned long);
void GXSetScissor(u32, u32, u32, u32);
float GXGetYScaleFactor(u16, u16);
u32 GXSetDispCopyYScale(float);
void GXSetDispCopySrc(u16, u16, u16, u16);
void GXSetDispCopyDst(u16, u16);
void GXSetCopyFilter(u8, u8 (*)[2], u8, u8*);
void GXSetPixelFmt(int, int);
void GXCopyDisp(void*, int);
void GXSetDispCopyGamma(int);
void VISetNextFrameBuffer(void*);
void VIFlush();
void VIWaitForRetrace();
unsigned long VIGetNextField();
void GXInvalidateVtxCache();
void GXSetColorUpdate(int);
void GXSetZMode(int, int, int);
void GXSetCopyClear(GXColor, unsigned long);
void GXDrawDone();
void VISetBlack(int);
void VIInit(void);
void* VISetPreRetraceCallback(void (*)(unsigned long));
int sprintf(char*, const char*, ...);
void MEM_free(void*);
}
void* DWI_allocalign(const char*, int, int, int);
void* DWI_alloc(const char*, int, int);
void DebugMsg(const char*, ...);

enum EOpenMode {};
enum ESeekOrigin {};
int File_Open(const char*, EOpenMode);
int File_Seek(int, int, ESeekOrigin);
int File_Write(int, void*, int);
int File_Close(int);

class CFader {
public:
    static void Update(float);
};

enum EScreenWidth {};
enum EFrameFormat {};
enum EDepthFormat {};

class CVector3 {
public:
    CVector3() {}
    CVector3(float ax, float ay, float az) : x(ax), y(ay), z(az) {}
    float x, y, z, w;
} __attribute__((aligned(8)));

class CMatrix {
public:
    void BuildTrans(CVector3);
    void Scale(CVector3);
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    static bool s_ClassInit;

    CVector3 right;
    CVector3 forward;
    CVector3 up;
    CVector3 position;
};

struct SSHHeader {
    char id[4];
    int size;
    int count;
    char dir[4];
};

struct SSHDirEntry {
    int name;
    int offset;
    int pad0;
    int pad1;
};

struct SSHShape {
    unsigned long type : 7;
    unsigned long flag : 1;
    unsigned long next : 24;
    unsigned short width;
    unsigned short height;
    unsigned short cx;
    unsigned short cy;
    short x : 12;
    unsigned short f0 : 1;
    unsigned short f1 : 1;
    unsigned short f2 : 1;
    unsigned short f3 : 1;
    short y : 12;
    unsigned short mips : 4;
};

class CScreen;
static CScreen* g_prevScreen;
static CScreen* g_pScreen;
static unsigned long g_FrameNum;
static unsigned long g_WaitFrameNum;
static volatile bool g_bSwap;
static bool g_bReady;
static bool g_bCapture;

class CScreen {
public:
    CScreen(EScreenWidth width, int height, EFrameFormat frame, EDepthFormat depth)
        : m_clear(true), m_clearRect(0, 0, 0, 0), m_clearColor(0, 0, 0, 128), data1c(0), m_width(width), m_height(height),
          m_frameFormat(frame), m_depthFormat(depth), data54(0), data58(0), m_tmValid(false), m_buffer(0), m_retraces(0),
          m_late(false) {}
    virtual ~CScreen();
    virtual CScreen* SetCurrent();
    virtual bool Flip();
    virtual unsigned long Wait();
    virtual void SetClear(bool);
    virtual void SetClearColor(const CColor&);
    virtual void SetClearRect(const CRect&);
    virtual int GetHeight();
    virtual int GetWidth();
    virtual float GetHeightScale() { return m_heightScale; }
    virtual float GetWidthScale() { return m_widthScale; }
    virtual EFrameFormat* GetFrameFormat();
    virtual EDepthFormat* GetDepthFormat();
    virtual bool GetClear();
    virtual CColor* GetClearColor();
    virtual CRect* GetClearRect();
    virtual CMatrix* GetTMViewToScreen();
    virtual void Rebuild() = 0;
    virtual void PutDisp();
    virtual void Clear();
    virtual void SetDraw() = 0;
    virtual void _Capture();
    static void Capture();
    static CScreen* GetCurrent();
    static void Init();
    static void VBlankHandler(unsigned long);
    unsigned long GetDepthBufferMax();
    unsigned long GetVSyncs();

    /* inferred: the four words at +8 set together (arguments evaluated right
       to left, as the loads show) */
    void SetRect(int x, int y, int width, int height) {
        m_clearRect.left = x;
        m_clearRect.top = y;
        m_clearRect.right = width;
        m_clearRect.bottom = height;
    }

    unsigned char data04;
    bool m_clear;
    CRect m_clearRect;
    CColor m_clearColor;
    int data1c;
    EScreenWidth m_width;
    int m_height;
    float m_widthScale;
    float m_heightScale;
    EFrameFormat m_frameFormat;
    EDepthFormat m_depthFormat;
    unsigned char unknown38[28];
    int data54;
    int data58;
    unsigned char unknown5c[4];
    CMatrix m_tmViewToScreen;
    bool m_tmValid;
    int m_buffer;
    int m_retraces;
    bool m_late;
    GXRenderModeObj* m_mode;
    GXRenderModeObj m_modeCopy;
    void* m_frameBuffer0;
    void* m_frameBuffer1;
    void* m_copyBuffer;
    void* m_fifoBuffer;
    void* m_fifo;
};

class CScreenFieldMode : public CScreen {
public:
    virtual ~CScreenFieldMode();
    virtual CScreen* SetCurrent();
    virtual void Rebuild();
    virtual void PutDisp();
    virtual void SetDraw();
    virtual void _Capture();
    CScreenFieldMode(EScreenWidth, EFrameFormat, EDepthFormat);
};


/* Stubs of classes the original linker removed (see the top of the file). */
class CScreenFrameMode : public CScreen {
public:
    virtual ~CScreenFrameMode();
    virtual void Rebuild();
    virtual void SetDraw();
};

class CScreenFieldModePAL : public CScreenFieldMode {
public:
    virtual ~CScreenFieldModePAL();
};

void CScreen::VBlankHandler(unsigned long) {
    g_FrameNum++;
    if (g_pScreen && g_bSwap) {
        CScreen* screen = g_pScreen;
        if (screen->m_retraces++ > 1)
            screen->m_late = true;
        screen->PutDisp();
        screen->SetDraw();
        screen->m_buffer ^= 1;
        g_bSwap = false;
    }
}

CScreen::~CScreen() {
    if (g_pScreen == this)
        g_pScreen = g_prevScreen;
    else if (g_prevScreen == this)
        g_prevScreen = 0;
}

void CScreen::Init() {
    g_bReady = true;
    VIInit();
    VISetPreRetraceCallback(VBlankHandler);
}

CScreen* CScreen::GetCurrent() {
    return g_pScreen;
}

CScreen* CScreen::SetCurrent() {
    m_widthScale = (float)m_width / 640.0f;
    m_heightScale = (float)m_height / 448.0f;
    if (g_pScreen == this)
        return this;
    g_prevScreen = g_pScreen;
    g_pScreen = this;
    m_mode = &GXNtsc480IntDf;
    DebugMsg("INTERLACED MODE");
    GXAdjustForOverscan(m_mode, &m_modeCopy, 0, 16);
    m_mode = &m_modeCopy;
    u32 size = (u16)((u16)(m_mode->fbWidth + 15) & ~15) * m_mode->xfbHeight * 2;
    m_frameBuffer0 = DWI_allocalign("FrameBuffer 1", size, 32, 1024);
    m_frameBuffer1 = DWI_allocalign("FrameBuffer 2", size, 32, 1024);
    m_copyBuffer = m_frameBuffer1;
    VIConfigure(m_mode);
    m_fifoBuffer = DWI_allocalign("Fifo", 0x40000, 32, 1024);
    m_fifo = GXInit(m_fifoBuffer, 0x40000);
    GXSetViewport(0.0f, 0.0f, m_mode->fbWidth, m_mode->efbHeight, 0.0f, 1.0f);
    GXSetScissor(0, 0, m_mode->fbWidth, m_mode->efbHeight);
    u16 lines = GXSetDispCopyYScale(GXGetYScaleFactor(m_mode->efbHeight, m_mode->xfbHeight));
    GXSetDispCopySrc(0, 0, m_mode->fbWidth, m_mode->efbHeight);
    GXSetDispCopyDst(m_mode->fbWidth, lines);
    GXSetCopyFilter(m_mode->aa, m_mode->sample_pattern, 1, m_mode->vfilter);
    if (m_mode->aa)
        GXSetPixelFmt(2, 0);
    else
        GXSetPixelFmt(0, 0);
    GXCopyDisp(m_copyBuffer, 1);
    GXSetDispCopyGamma(0);
    VISetNextFrameBuffer(m_frameBuffer0);
    m_copyBuffer = m_frameBuffer1;
    VIFlush();
    VIWaitForRetrace();
    if (m_mode->viTVmode & 1)
        VIWaitForRetrace();
    if (m_mode->field_rendering)
        GXSetViewportJitter(0.0f, 0.0f, m_mode->fbWidth, m_mode->efbHeight, 0.0f, 1.0f, VIGetNextField());
    else
        GXSetViewport(0.0f, 0.0f, m_mode->fbWidth, m_mode->efbHeight, 0.0f, 1.0f);
    GXInvalidateVtxCache();
    return g_prevScreen;
}

bool CScreen::Flip() {
    GXSetZMode(1, 3, 1);
    GXSetColorUpdate(m_clear != 0);
    GXColor color = {m_clearColor.r, m_clearColor.g, m_clearColor.b, 0};
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
    if (m_mode->field_rendering)
        GXSetViewportJitter(0.0f, 0.0f, m_mode->fbWidth, m_mode->efbHeight, 0.0f, 1.0f, VIGetNextField());
    else
        GXSetViewport(0.0f, 0.0f, m_mode->fbWidth, m_mode->efbHeight, 0.0f, 1.0f);
    GXInvalidateVtxCache();
    if (!m_clear)
        GXSetColorUpdate(1);
    return frames;
}

unsigned long CScreen::GetVSyncs() {
    return g_FrameNum;
}

void CScreen::SetClear(bool clear) {
    m_clear = clear;
}

void CScreen::SetClearColor(const CColor& color) {
    m_clearColor = color;
}

void CScreen::SetClearRect(const CRect& rect) {
    m_clearRect = rect;
}

int CScreen::GetHeight() {
    return m_height;
}

int CScreen::GetWidth() {
    return m_width;
}

EFrameFormat* CScreen::GetFrameFormat() {
    return &m_frameFormat;
}

EDepthFormat* CScreen::GetDepthFormat() {
    return &m_depthFormat;
}

unsigned long CScreen::GetDepthBufferMax() {
    switch (m_depthFormat) {
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

bool CScreen::GetClear() {
    if (g_bCapture)
        return false;
    return m_clear;
}

CColor* CScreen::GetClearColor() {
    return &m_clearColor;
}

CRect* CScreen::GetClearRect() {
    return &m_clearRect;
}

void CScreen::Clear() {
    GXCopyDisp(m_copyBuffer, 1);
}

void CScreen::PutDisp() {
}

CMatrix* CScreen::GetTMViewToScreen() {
    if (!m_tmValid) {
        CVector3 scale((float)m_width / 2.0f, (float)-m_height / 2.0f, -0.5f * (float)GetDepthBufferMax() / 16.0f);
        CVector3 trans(0.0f, 0.0f, -1.0f);
        m_tmViewToScreen.BuildTrans(trans);
        m_tmViewToScreen.Scale(scale);
        m_tmValid = true;
    }
    return &m_tmViewToScreen;
}

void CScreen::Capture() {
    g_bCapture = true;
}

void CScreen::_Capture() {
    DebugMsg("CScreen::_Capture() does nothing...\n");
}

CScreenFieldMode::CScreenFieldMode(EScreenWidth width, EFrameFormat frame, EDepthFormat depth)
    : CScreen(width, 448, frame, depth) {
}

CScreen* CScreenFieldMode::SetCurrent() {
    SetRect(0, 0, m_width, m_height);
    Rebuild();
    return CScreen::SetCurrent();
}

CScreenFieldMode::~CScreenFieldMode() {
}

void CScreenFieldMode::Rebuild() {
}

void CScreenFieldMode::SetDraw() {
}

void CScreenFieldMode::PutDisp() {
}

void CScreenFieldMode::_Capture() {
    static int _captureNum = 0;
    char name[256];
    sprintf(name, "host0:screen_%04d.ssh", _captureNum++);
    int file = File_Open(name, (EOpenMode)98);
    if (file < 0)
        return;
    int bpp;
    switch (m_frameFormat) {
    case 0:
        bpp = 2;
        break;
    case 1:
        bpp = 3;
        break;
    case 2:
    default:
        bpp = 4;
        break;
    }
    int rowBytes = m_width * bpp;
    int size = rowBytes * m_height;
    unsigned short* buffer = (unsigned short*)DWI_alloc(0, size, 1024);
    int height = m_height;
    int lines = 0x7fff0 / rowBytes;
    for (int y = 0; y < height;) {
        int next = y + lines;
        if (next > height) {
            next = height;
            lines = height - y;
        }
        y = next;
    }
    SSHDirEntry dir;
    SSHShape shape;
    SSHHeader header;
    header.id[0] = 'S';
    header.id[1] = 'H';
    header.id[2] = 'P';
    header.id[3] = 'S';
    header.size = size + 48;
    header.count = 1;
    header.dir[0] = 'G';
    header.dir[1] = 'R';
    header.dir[2] = 'A';
    header.dir[3] = 'B';
    dir.name = 0;
    dir.offset = 32;
    dir.pad1 = 0;
    dir.pad0 = 0;
    char type;
    if (m_frameFormat == 0)
        type = 3;
    else {
        type = 5;
        if (m_frameFormat == 1)
            type = 4;
    }
    shape.type = type;
    shape.flag = 0;
    shape.next = 0;
    shape.width = m_width;
    shape.height = m_height;
    shape.cx = 0;
    shape.cy = 0;
    shape.x = 0;
    shape.f0 = 0;
    shape.f1 = 0;
    shape.f2 = 0;
    shape.f3 = 0;
    shape.y = 0;
    shape.mips = 0;
    if (m_frameFormat == 0) {
        int count = m_width * m_height;
        for (int i = 0; i < count; i++)
            buffer[i] |= 0x8000;
    }
    File_Seek(file, 0, (ESeekOrigin)0);
    File_Write(file, &header, 16);
    File_Write(file, &dir, 16);
    File_Write(file, &shape, 16);
    File_Write(file, buffer, size);
    File_Close(file);
    MEM_free(buffer);
}

/* Stubs (see the top of the file): their type names must follow the
   functions' strings, so they are defined last. */
CScreenFieldModePAL::~CScreenFieldModePAL() {
}

CScreenFrameMode::~CScreenFrameMode() {
}

void CScreenFrameMode::Rebuild() {
}

void CScreenFrameMode::SetDraw() {
}

