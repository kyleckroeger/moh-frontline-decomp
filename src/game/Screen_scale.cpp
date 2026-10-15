// A fragment of Screen.cpp (0x8001b12c): CScreen's GetHeightScale and
// GetWidthScale return the screen's scale factors (+44, +40). They are weak
// copies of header inlines emitted in this file, so they are defined
// __declspec(weak). CScreen is named by the symbols; the members are
// inferred. The rest of the file is in other units.
class CScreen {
public:
    float GetHeightScale();
    float GetWidthScale();

    unsigned char unknown00[40];
    float m_widthScale;
    float m_heightScale;
};

__declspec(weak) float CScreen::GetHeightScale() {
    return m_heightScale;
}

__declspec(weak) float CScreen::GetWidthScale() {
    return m_widthScale;
}
