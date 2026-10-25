// A fragment of fader.cpp (0x8007bd00); the file name is this project's. Screen fades: CFaderControl, a colour pair with a duration and a virtual
// Update, and its fading test. CFaderControl, CFader and CColor are named by the
// mangled symbols and RTTI; members are inferred from offsets. This unit
// covers Update (the key function: the unit owns the class's virtual table
// and RTTI record), IsFading and the constructor; the fader bin before them
// and the CFader functions after them are not part of it (draft in scratch).
// Update blends the start colour towards the end colour by the elapsed part
// of the duration and makes it the fade colour; CFader::SetColor is defined
// later in fader.cpp and inlined here (inferred: a plain member-wise store of
// the file's static fade colour). The hoisted float declarations follow the
// original's register use.
struct CColor {
    CColor() {}
    CColor(float ar, float ag, float ab, float aa) : r(ar), g(ag), b(ab), a(aa) {}
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

/* the static fade colour of fader.cpp (fader.json) */
extern CColor g_color;

class CFader {
public:
    static void SetColor(CColor color) { g_color = color; }
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

bool CFaderControl::IsFading() const {
    return m_time <= m_duration;
}

CFaderControl::CFaderControl(CColor from, CColor to, float duration) {
    m_from = from;
    m_to = to;
    m_duration = duration;
    m_time = 0.0f;
}
