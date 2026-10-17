// A fragment of font.cpp (0x8007ca34): CFont::Render (the text run's count,
// position and colour are added to the packet, then its characters copied in
// one block; returns 0) and the font data accessors GetHeight, SetHeight,
// SetScale, GetScale, SetDepth (the depth's bits stored as a float, as
// the image does through the stack), GetColor and SetColor (the colour word
// copied, the result byte by byte through the copy constructor). The file name is this project's; the
// original record is font.cpp. CFont and its functions are named by the
// mangled symbols; the records and members are inferred. CFont declares its
// destructor (defined elsewhere) first so its table is not emitted here.
extern "C" void* memcpy(void*, const void*, unsigned long);

/* Inferred: a colour with a member-wise copy constructor (as in fader.cpp). */
struct CColor {
    CColor() {}
    CColor(const CColor& c) {
        unsigned char cr = c.r;
        unsigned char cg = c.g;
        unsigned char ca = c.a;
        unsigned char cb = c.b;
        r = cr;
        g = cg;
        b = cb;
        a = ca;
    }

    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

/* Inferred: the packet's write cursor at +8 and its templated append. */
class CDmaPacket {
public:
    template <class T> void Add(const T& value) {
        *(T*)m_cur = value;
        m_cur += sizeof(T);
    }

    unsigned char unknown00[8];
    unsigned char* m_cur;
};

/* Inferred: the font data's colour, height, depth and scale. */
struct FontDataView {
    unsigned char unknown00[32];
    CColor m_color;
    float m_height;
    float m_depth;
    unsigned char unknown2c[12];
    float m_scaleX;
    float m_scaleY;
};

/* Inferred: a text run's render record: the character count, position,
   colour, then 16 bytes per character. */
struct SFontRender {
    int m_count;
    float m_x;
    float m_y;
    CColor m_color;
    unsigned char m_chars[1];
};

class CRenderBin {
    unsigned char unknown00[28];

public:
    virtual ~CRenderBin();
    virtual int Render(CDmaPacket&, void*);
};

class CFont : public CRenderBin {
public:
    virtual ~CFont();
    virtual int Render(CDmaPacket&, void*);
    float GetHeight();
    void SetHeight(float);
    void SetScale(float, float);
    void GetScale(float&, float&);
    void SetDepth(unsigned long);
    CColor GetColor();
    void SetColor(CColor);

    FontDataView* m_data;
};

int CFont::Render(CDmaPacket& packet, void* data) {
    SFontRender* render = (SFontRender*)data;
    packet.Add(render->m_count);
    packet.Add(render->m_x);
    packet.Add(render->m_y);
    packet.Add(render->m_color);
    int size = render->m_count * 16;
    memcpy(packet.m_cur, render->m_chars, size);
    packet.m_cur += size;
    return 0;
}

float CFont::GetHeight() {
    return m_data->m_height;
}

void CFont::SetHeight(float height) {
    m_data->m_height = height;
}

void CFont::SetScale(float x, float y) {
    m_data->m_scaleX = x;
    m_data->m_scaleY = y;
}

void CFont::GetScale(float& x, float& y) {
    x = m_data->m_scaleX;
    y = m_data->m_scaleY;
}

void CFont::SetDepth(unsigned long depth) {
    m_data->m_depth = *(float*)&depth;
}

CColor CFont::GetColor() {
    CColor color;
    *(unsigned int*)&color = *(unsigned int*)&m_data->m_color;
    return color;
}

void CFont::SetColor(CColor color) {
    *(unsigned int*)&m_data->m_color = *(unsigned int*)&color;
}
