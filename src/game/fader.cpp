// fader.cpp (0x8007b914): screen fades. The fader's render bin (CFaderBin: its
// destructor, IsUsed (the fade colour's alpha is not zero) and Link (under a
// temporary orthographic projection of the 640x448 screen and an identity view
// pushed back to -1200, one untextured quad covering the screen in the fade
// colour, each channel doubled and clamped; the previous projection is
// restored)); CFaderControl, a colour pair with a duration and a virtual Update
// that blends the start colour towards the end colour by the elapsed part of
// the duration and makes it the fade colour, its fading test and constructor;
// CFader's SetControl, SetColor, Update and Init; the weak MathFunClamp<int>
// instance and the static initialisation of the fade colour and the render
// bin. The file owns CFaderBin's and CFaderControl's virtual tables and the
// RTTI records of CRenderBinData, CRenderBin, CFaderBin and CFaderControl.
// The classes and functions are named by the mangled symbols and RTTI;
// members, the base layout, the 114 bin priority and the colour constructor
// and assignment (bytewise) are inferred. The file is compiled with deferred
// inlining (SetColor and IsFading are inlined into functions the image places
// before them), so the functions are written in reverse image order. The
// hoisted float declarations in CFaderControl::Update follow the original's
// register use.
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

class CDmaTag;
class CDmaPacket;

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

class CRenderBinData {
public:
    CRenderBinData() : m_field0(0), m_field4(0), m_field8(0), m_fieldC(0), m_field10(0), m_priority(3) {}

    int m_field0;
    int m_field4;
    int m_field8;
    int m_fieldC;
    int m_field10;
    int m_priority;
    int m_field18;
};

class CRenderBin : public CRenderBinData {
public:
    virtual ~CRenderBin() {}
    virtual void Render(CDmaPacket&, void*);
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();
};

class CRenderList {
public:
    void Register(CRenderBin&);
};

class CFaderBin : public CRenderBin {
public:
    CFaderBin() { m_priority = 114; }
    virtual bool IsUsed();
    virtual void Link(CDmaTag*, CDmaPacket&);
};

class CFaderControl {
public:
    CFaderControl(CColor, CColor, float);
    virtual void Update(float);
    bool IsFading() const;

    CColor m_from;
    CColor m_to;
    float m_duration;
    float m_time;
};

class CFader {
public:
    static void Init(CRenderList*);
    static void Update(float);
    static void SetControl(CFaderControl*);
    static void SetColor(CColor);
};

static CFaderControl* g_pFaderControl;
static CRenderList* g_pRenderList;
static CColor g_color(0, 0, 0, 0);
static CFaderBin g_FaderBin;

#pragma auto_inline off
template <class T> __declspec(weak) T MathFunClamp(T value, T low, T high) {
    if (value < low)
        return low;
    if (value > high)
        return high;
    return value;
}

template int MathFunClamp<int>(int, int, int);
#pragma auto_inline reset

void CFader::Init(CRenderList* list) {
    g_pRenderList = list;
    list->Register(g_FaderBin);
}

void CFader::Update(float elapsed) {
    if (g_pFaderControl) {
        g_pFaderControl->Update(elapsed);
        if (!g_pFaderControl->IsFading())
            g_pFaderControl = 0;
    }
}

void CFader::SetColor(CColor color) {
    g_color = color;
}

void CFader::SetControl(CFaderControl* control) {
    if (control) {
        control->m_time = 0.0f;
        SetColor(CColor(control->m_from.r, control->m_from.g, control->m_from.b, control->m_from.a));
    }
    g_pFaderControl = control;
}

CFaderControl::CFaderControl(CColor from, CColor to, float duration) {
    m_from = from;
    m_to = to;
    m_duration = duration;
    m_time = 0.0f;
}

bool CFaderControl::IsFading() const {
    return m_time <= m_duration;
}

void CFaderControl::Update(float elapsed) {
    if (m_time < m_duration) {
        float fg;
        float a;
        float fb;
        float fr;
        float fa;
        float r;
        float b;
        float g;
        float t;
        t = m_time / m_duration;
        fa = m_from.a;
        fb = m_from.b;
        fg = m_from.g;
        fr = m_from.r;
        r = m_to.r - fr;
        r *= t;
        r += fr;
        g = m_to.g - fg;
        g *= t;
        g += fg;
        b = m_to.b - fb;
        b *= t;
        b += fb;
        a = m_to.a - fa;
        a *= t;
        a += fa;
        CFader::SetColor(CColor(r, g, b, a));
        m_time += elapsed;
        if (m_time > m_duration)
            m_time = m_duration;
    } else if (m_time == m_duration) {
        CFader::SetColor(m_to);
        m_time += elapsed;
    }
}

void CFaderBin::Link(CDmaTag*, CDmaPacket&) {
    float projection[GX_PROJECTION_SZ];
    Mtx view;
    Mtx44 ortho;
    GXGetProjectionv(projection);
    MTXOrtho(ortho, 224.0f, -224.0f, -320.0f, 320.0f, 5.0f, 2000.0f);
    GXSetProjection(ortho, GX_ORTHOGRAPHIC);
    MTXIdentity(view);
    view[2][3] = -1200.0f;
    GXLoadPosMtxImm(view, GX_PNMTX0);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_NONE);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GXSetNumChans(1);
    GXSetNumTexGens(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_NOOP);
    GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
    GXSetCullMode(GX_CULL_NONE);
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    GXPosition3s16(-320, 224, 0);
    GXColor4u8(MathFunClamp<int>(g_color.r * 2, 0, 255), MathFunClamp<int>(g_color.g * 2, 0, 255),
               MathFunClamp<int>(g_color.b * 2, 0, 255), MathFunClamp<int>(g_color.a * 2, 0, 255));
    GXPosition3s16(320, 224, 0);
    GXColor4u8(MathFunClamp<int>(g_color.r * 2, 0, 255), MathFunClamp<int>(g_color.g * 2, 0, 255),
               MathFunClamp<int>(g_color.b * 2, 0, 255), MathFunClamp<int>(g_color.a * 2, 0, 255));
    GXPosition3s16(320, -224, 0);
    GXColor4u8(MathFunClamp<int>(g_color.r * 2, 0, 255), MathFunClamp<int>(g_color.g * 2, 0, 255),
               MathFunClamp<int>(g_color.b * 2, 0, 255), MathFunClamp<int>(g_color.a * 2, 0, 255));
    GXPosition3s16(-320, -224, 0);
    GXColor4u8(MathFunClamp<int>(g_color.r * 2, 0, 255), MathFunClamp<int>(g_color.g * 2, 0, 255),
               MathFunClamp<int>(g_color.b * 2, 0, 255), MathFunClamp<int>(g_color.a * 2, 0, 255));
    GXSetProjectionv(projection);
}

bool CFaderBin::IsUsed() {
    return g_color.a != 0;
}
