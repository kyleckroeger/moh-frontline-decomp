struct CColor {
    CColor() {}
    CColor& operator=(const CColor& c) { r = c.r; g = c.g; b = c.b; a = c.a; return *this; }
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

class CFaderControl {
public:
    CFaderControl(CColor, CColor, float);
    virtual void Update(float);
    void Set(CColor to, CColor from, float duration) {
        m_time = 0.0f;
        m_from = from;
        m_to = to;
        m_duration = duration;
    }

    CColor m_from;
    CColor m_to;
    float m_duration;
    float m_time;
};

class CFader {
public:
    static void SetControl(CFaderControl*);
};

extern CFaderControl g_screenflash;

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

inline CColor BSArgColor(int** stack, int count, int i) {
    CColor c;
    c.r = *(*stack - (count - i));
    c.g = *(*stack - (count - (i + 1)));
    c.b = *(*stack - (count - (i + 2)));
    c.a = *(*stack - (count - (i + 3)));
    return c;
}
void BIFunc_PlayerScreenFlash(int** stack, void*) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    g_screenflash.Set(BSArgColor(stack, count, 5), BSArgColor(stack, count, 1), *(float*)(*stack - (count - 9)));
    CFader::SetControl(&g_screenflash);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
