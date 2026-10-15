// A fragment of hwvideodisplaylist.cpp (0x80102290): GCHW_VD's CompleteDraw
// (empty), ReDraw (draws again through iDraw) and Draw (the frame held at
// +0x6c, if any, released to the decoder at +4, then the new frame held and
// drawn through iDraw). The classes and functions are named by the symbols;
// the members are inferred from the offsets. The rest of the file is in
// other units.
namespace RCMP {
struct FRAME;

class DECODER {
public:
    void ReleaseFrame(FRAME*);
};
}

class GCHW_VD {
public:
    void CompleteDraw();
    void ReDraw();
    void Draw(RCMP::FRAME*);
    void iDraw();

    void* m_vtable;
    RCMP::DECODER* m_decoder;
    unsigned char unknown08[0x64];
    RCMP::FRAME* m_frame;
};

void GCHW_VD::CompleteDraw() {
}

void GCHW_VD::ReDraw() {
    iDraw();
}

void GCHW_VD::Draw(RCMP::FRAME* frame) {
    if (m_frame)
        m_decoder->ReleaseFrame(m_frame);
    m_frame = frame;
    iDraw();
}
