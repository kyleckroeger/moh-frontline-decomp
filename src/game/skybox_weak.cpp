// A fragment of skybox.cpp; the file name is this project's. CSkyBox's weak visibility and draw-enabled defaults and its vertex
// constructor. CSkyBox and SVertex are named by the mangled symbols; the
// draw-enabled member is inferred from its offset and the class is a
// non-virtual view. They are inline in the original (weak symbols), so they
// are defined __declspec(weak). The rest of the file is not part of this unit.
class CDrawContext;

class CSkyBox {
public:
    struct SVertex {
        SVertex();
    };

    bool IsVisible(CDrawContext&) const;
    bool IsDrawEnabled() const;

    unsigned char unknown000[448];
    bool m_drawEnabled;
};

__declspec(weak) bool CSkyBox::IsVisible(CDrawContext&) const {
    return true;
}

__declspec(weak) bool CSkyBox::IsDrawEnabled() const {
    return m_drawEnabled;
}

__declspec(weak) CSkyBox::SVertex::SVertex() {
}
