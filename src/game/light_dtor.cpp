// A fragment of light.cpp (0x80078304): the CLight destructor (its virtual
// table pointer, then the inline IMovingSceneNode and ISceneNode destructors
// and IObserver's, and the object freed when asked), then
// CLight::SetDefaultLight, which converts the colour to luma and chroma,
// brightens the luma by half, converts it back to RGB in 0..1 and stores it
// with the normalised direction and the intensity in g_DefaultLights. The
// file name is this project's; the original record is light.cpp, after
// light_visible.cpp. The classes and functions are named by the mangled
// symbols; only the virtuals the destructor needs are declared. CLight
// declares MarkForDestruction (its own override, defined elsewhere) first so
// its global virtual table is not emitted here. IMovingSceneNode's and
// ISceneNode's destructors are inline and their tables weak in the original,
// so the compiler's copies are weak duplicates, linked to the original
// copies.
extern inline float sqrtf(float x)
{
    const double _half = .5;
    const double _three = 3.0;
    volatile float y;
    if (x > 0.0f)
    {
        double guess = __frsqrte((double)x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        y = (float)(x*guess);
        return y;
    }
    return x;
}

class CVector3 {
public:
    float Length() const { return sqrtf(d.v[0] * d.v[0] + d.v[1] * d.v[1] + d.v[2] * d.v[2]); }
    void Normalize() {
        float length = Length();
        if (length != 0.0f) {
            d.v[0] *= 1.0f / length;
            d.v[1] *= 1.0f / length;
            d.v[2] *= 1.0f / length;
        }
    }

    union {
        double pair[2];
        float v[4];
    } d;
} __attribute__((aligned(8)));

struct CColor {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

/* inferred: a default light's direction, colour and intensity */
struct DefaultLightView {
    unsigned char unknown00[16];
    float m_direction[3];
    float m_color[3];
    float m_intensity;
    unsigned char unknown2c[4];
};

extern DefaultLightView g_DefaultLights[];

class IDestructible {
public:
    IDestructible() {}
    virtual void MarkForDestruction(int);
    virtual ~IDestructible();
};

class ISubject : public IDestructible {
public:
    ISubject() {}
    virtual ~ISubject();
};

class IObserver : public ISubject {
public:
    IObserver() {}
    virtual ~IObserver();
};

class ISceneNode : public IObserver {
public:
    ISceneNode() : data04(0), data08(0), data0c(0), data10(0), data14(0), data18(0), data1c(0), data20(0) {}
    virtual ~ISceneNode() {}

    int data04;
    int data08;
    int data0c;
    int data10;
    int data14;
    int data18;
    int data1c;
    int data20;
};

class IMovingSceneNode : public ISceneNode {
public:
    IMovingSceneNode() {}
    virtual ~IMovingSceneNode() {}
};

class CLight : public IMovingSceneNode {
public:
    virtual void MarkForDestruction(int);
    virtual ~CLight();
    enum EDefaultLights {};
    static void SetDefaultLight(EDefaultLights, CVector3, CColor, float);
};

CLight::~CLight() {
}

void CLight::SetDefaultLight(EDefaultLights light, CVector3 direction, CColor color, float intensity) {
    float red;
    float green;
    float blue;
    float luma;
    float cb;
    float cr;
    float r;
    float g;
    float b;
    CVector3 dir;
    red = color.r;
    green = color.g;
    blue = color.b;
    luma = 0.299 * red + 0.587 * green + 0.114 * blue;
    cb = 128.0 + (-0.1687 * red - 0.3313 * green + 0.5 * blue);
    cr = 128.0 + (0.5 * red - 0.4187 * green - 0.0813 * blue);
    luma *= 1.5f;
    r = luma + 1.402 * (cr - 128.0f);
    g = luma - 0.34414 * (cb - 128.0f) - 0.71414 * (cr - 128.0f);
    b = luma + 1.772 * (cb - 128.0f);
    r /= 255.0f;
    g /= 255.0f;
    b /= 255.0f;
    dir = direction;
    dir.Normalize();
    g_DefaultLights[light].m_direction[0] = dir.d.v[0];
    g_DefaultLights[light].m_direction[1] = dir.d.v[1];
    g_DefaultLights[light].m_direction[2] = dir.d.v[2];
    g_DefaultLights[light].m_color[0] = r;
    g_DefaultLights[light].m_color[1] = g;
    g_DefaultLights[light].m_color[2] = b;
    g_DefaultLights[light].m_intensity = intensity;
}
