// A fragment of Screen.cpp (0x8001b544): CScreenFieldMode::SetCurrent (the rect
// at +8 set to the origin and the screen size through an inferred inline
// setter, the mode rebuilt through its table, then CScreen::SetCurrent); the
// CScreenFieldMode constructor (CScreen's inline constructor with a height of
// 448: flags, words and the clear colour byte reset, the size and the frame
// and depth formats kept, the transform's class initialisation, the cached
// view-to-screen transform marked invalid); CScreen::_Capture (a debug
// message), CScreen::Capture (sets the file-local capture flag) and
// CScreen::GetTMViewToScreen (builds the cached view-to-screen transform once:
// a translation by -1 in z, then a scale by half the width, minus half the
// height and minus half the depth-buffer maximum over 16). The file
// name is this project's; the original record is Screen.cpp, after
// Screen_cscreenfieldmode_dtor.cpp. The classes, enums and functions are named
// by the mangled symbols; CScreen's virtuals are declared in table order
// (Rebuild and SetDraw are pure; result types are not known); the members,
// their names and the constructor's arguments (the 448 field-mode height
// passed to it) are inferred, as is the inline depth-buffer maximum (the
// depth format's switch, also compiled out of line as GetDepthBufferMax). CScreen's destructor is declared out of line
// (it is global in the original), so its table stays elsewhere; so does
// CScreenFieldMode's, which declares Rebuild (defined elsewhere) first. The
// capture flag is file-local in the original; this fragment declares it
// non-static so it links to the original object. The message is an item of
// the file's .rodata pool.
class CColor;
class CRect;
enum EScreenWidth {};
enum EFrameFormat {};
enum EDepthFormat {};
extern bool g_bCapture;
void DebugMsg(const char*, ...);

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

class CScreen {
public:
    CScreen(EScreenWidth width, int height, EFrameFormat frame, EDepthFormat depth)
        : data05(true), data08(0), data0c(0), data10(0), data14(0), data18(0), data19(0), data1a(0), data1b(128),
          data1c(0), m_width(width), m_height(height), m_frameFormat(frame), m_depthFormat(depth), data54(0),
          data58(0), m_tmValid(false), dataa4(0), dataa8(0), dataac(false) {}
    virtual ~CScreen();
    virtual void SetCurrent();
    virtual void Flip();
    virtual void Wait();
    virtual void SetClear(bool);
    virtual void SetClearColor(const CColor&);
    virtual void SetClearRect(const CRect&);
    virtual void GetHeight();
    virtual void GetWidth();
    virtual void GetHeightScale();
    virtual void GetWidthScale();
    virtual void GetFrameFormat();
    virtual void GetDepthFormat();
    virtual void GetClear();
    virtual void GetClearColor();
    virtual void GetClearRect();
    virtual CMatrix* GetTMViewToScreen();
    virtual void Rebuild() = 0;
    virtual void PutDisp();
    virtual void Clear();
    virtual void SetDraw() = 0;
    virtual void _Capture();
    static void Capture();

    /* inferred: the four words at +8 set together (arguments evaluated right
       to left, as the loads show) */
    void SetRect(int x, int y, int width, int height) {
        data08 = x;
        data0c = y;
        data10 = width;
        data14 = height;
    }

    unsigned long DepthMax() const {
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

    unsigned char data04;
    bool data05;
    int data08;
    int data0c;
    int data10;
    int data14;
    unsigned char data18;
    unsigned char data19;
    unsigned char data1a;
    unsigned char data1b;
    int data1c;
    EScreenWidth m_width;
    int m_height;
    unsigned char unknown28[8];
    EFrameFormat m_frameFormat;
    EDepthFormat m_depthFormat;
    unsigned char unknown38[28];
    int data54;
    int data58;
    unsigned char unknown5c[4];
    CMatrix m_tmViewToScreen;
    bool m_tmValid;
    int dataa4;
    int dataa8;
    bool dataac;
};

class CScreenFieldMode : public CScreen {
public:
    virtual void Rebuild();
    virtual ~CScreenFieldMode();
    virtual void SetCurrent();
    virtual void PutDisp();
    virtual void SetDraw();
    virtual void _Capture();
    CScreenFieldMode(EScreenWidth, EFrameFormat, EDepthFormat);
};

void CScreenFieldMode::SetCurrent() {
    SetRect(0, 0, m_width, m_height);
    Rebuild();
    CScreen::SetCurrent();
}

CScreenFieldMode::CScreenFieldMode(EScreenWidth width, EFrameFormat frame, EDepthFormat depth)
    : CScreen(width, 448, frame, depth) {
}

void CScreen::_Capture() {
    DebugMsg("CScreen::_Capture() does nothing...\n");
}

void CScreen::Capture() {
    g_bCapture = true;
}

CMatrix* CScreen::GetTMViewToScreen() {
    if (!m_tmValid) {
        CVector3 scale((float)m_width / 2.0f, (float)-m_height / 2.0f, -0.5f * (float)DepthMax() / 16.0f);
        CVector3 trans(0.0f, 0.0f, -1.0f);
        m_tmViewToScreen.BuildTrans(trans);
        m_tmViewToScreen.Scale(scale);
        m_tmValid = true;
    }
    return &m_tmViewToScreen;
}
