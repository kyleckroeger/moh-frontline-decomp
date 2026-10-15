// A fragment of hwvideodisplaylist.cpp (0x80102290): GCHW_VD's CompleteDraw (empty) and ReDraw (draws again through iDraw). The classes
// and functions are named by the symbols. The rest of the file is in other
// units.
class GCHW_VD {
public:
    void CompleteDraw();
    void ReDraw();
    void iDraw();
};

void GCHW_VD::CompleteDraw() {
}

void GCHW_VD::ReDraw() {
    iDraw();
}
