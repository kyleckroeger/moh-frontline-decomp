// CScreen: the display (render mode, frame buffers, clear state), shared by every file that
// uses it (PortReadiness step 3.3), with CScreenFieldMode, the mode the game runs in.
//
// Evidence:
// - __vt__7CScreen (0x80181dc4): 22 slots in the order declared below. Slots 17 and 20 are 0
//   (pure); CScreenFieldMode's table names them Rebuild and SetDraw.
// - Out of line in the original (src/game/Screen.cpp, 0x8001b12c..0x8001bfbc): every virtual
//   but GetHeightScale/GetWidthScale, and the statics Capture, GetCurrent, Init, VBlankHandler,
//   GetDepthBufferMax, GetVSyncs. GetHeightScale and GetWidthScale are weak (0x8001b12c,
//   0x8001b134): inline here. No CScreen constructor symbol: it is inline (in
//   CScreenFieldMode's constructor).
// - g_screen (.bss, 0x110 bytes) is a CScreenFieldMode: the class has no data of its own.
// - Field offsets: Screen.cpp's matched code and the layout lint (research/layout/classes.tsv:
//   m_widthScale/m_heightScale at +0x28/+0x2c, read by GetWidthScale/GetHeightScale).
//
// Inferred, not in any mangled name: the return types (Screen.cpp's; GetVSyncs returns int:
// mainloop.cpp converts it to float as a signed word), the field names and
// types, the unknown ranges, and SetRect (a helper of Screen.cpp's for the four clear-rect
// words).
#ifndef GAME_CSCREEN_H
#define GAME_CSCREEN_H

#include "game/CMatrix.h"
#include "game/CColor.h"
#include "game/CRect.h"

#ifndef _DOLPHIN_GX_GXSTRUCT_H_
// The SDK's render mode record (dolphin/gx/GXStruct.h), for files that don't include the SDK.
typedef struct _GXRenderModeObj {
    unsigned long viTVmode;
    unsigned short fbWidth;
    unsigned short efbHeight;
    unsigned short xfbHeight;
    unsigned short viXOrigin;
    unsigned short viYOrigin;
    unsigned short viWidth;
    unsigned short viHeight;
    unsigned long xFBmode;
    unsigned char field_rendering;
    unsigned char aa;
    unsigned char sample_pattern[12][2];
    unsigned char vfilter[7];
} GXRenderModeObj;
#endif

// Constructor argument types (__ct__16CScreenFieldModeF12EScreenWidth12EFrameFormat12EDepthFormat);
// enumerators not known.
enum EScreenWidth {};
enum EFrameFormat {};
enum EDepthFormat {};

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
    int GetVSyncs();

    // Inferred: the four clear-rect words set together (arguments evaluated right to left,
    // as Screen.cpp's loads show).
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

// __vt__16CScreenFieldMode: overrides the destructor, SetCurrent, Rebuild, PutDisp, SetDraw
// and _Capture, all out of line in Screen.cpp, as is the constructor.
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

#endif
