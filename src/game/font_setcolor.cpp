// A fragment of font.cpp (0x8007cba0): CFont::SetColor stores the colour, as a word, in the font record (+32) at
// its +32 (the views are inferred; the colour is passed by value). The classes
// and functions are named by the symbols. The rest of the file is in other
// units.
struct CColor {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

/* inferred: the font record's colour word at +32 */
struct FontRecordView {
    unsigned char unknown00[32];
    unsigned long m_color;
};

class CFont {
public:
    void SetColor(CColor);

    unsigned char unknown00[32];
    FontRecordView* m_font;
};

void CFont::SetColor(CColor color) {
    m_font->m_color = *(unsigned long*)&color;
}
