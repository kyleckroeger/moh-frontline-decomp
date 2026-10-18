// GCHW_VD, the hardware video display (0x801028d4): the destructor (the YUV
// swizzler deleted, the frame still held returned to the decoder, then the
// VIDEO_DRIVER base's inline destructor and the RCMP system's free for the
// class delete), then setting the scale or the position, which recomputes
// the drawn extent as size * scale + position on each axis (the position
// setter also stores the depth). The destructor is GCHW_VD's key function,
// so this unit is compiled with RTTI on and holds the class's hierarchy
// record, virtual table and type information (the class names are entries
// of the file's .rodata and .sdata2 pools). VIDEO_DRIVER has no key function
// (its CompleteDraw is inline), so its weak table and destructor are emitted
// here too: the destructor's weak copy follows SetPosition (its class delete
// is the global one; the RCMP system's free belongs to GCHW_VD), while the
// weak table and CompleteDraw are discarded as duplicates (CompleteDraw's
// copy is in hwvideodisplaylist_delete.cpp; the table follows this unit's
// data, compiled first here). GCHW_VD, VIDEO_DRIVER, DECODER, FRAME, rcmp_sys and the
// swizzler function are named by the mangled symbols; the members, the
// system's free slot (as in avplayer.cpp) and the result types are
// inferred, and the virtual functions are declared in table order. The rest
// of the file is not part of this unit.
struct tBigYUVSwizzler;
void DELETE_tBigYUVSwizzler(tBigYUVSwizzler*);

namespace RCMP {

class FRAME;

class DECODER {
public:
    void ReleaseFrame(FRAME*);
};

class RCMP_SYSTEM {
public:
    virtual ~RCMP_SYSTEM();

    void* (*m_alloc)(unsigned long);
    void (*m_free)(void*);
};

extern RCMP_SYSTEM rcmp_sys;

class VIDEO_DRIVER {
public:
    virtual ~VIDEO_DRIVER() {}
    virtual void SetPosition(float, float, float) = 0;
    virtual void SetScale(float, float) = 0;
    virtual void Draw(FRAME*) = 0;
    virtual void ReDraw() = 0;
    virtual void CompleteDraw() {}
};

}

class GCHW_VD : public RCMP::VIDEO_DRIVER {
public:
    virtual ~GCHW_VD();
    void operator delete(void* p) { RCMP::rcmp_sys.m_free(p); }
    virtual void SetPosition(float, float, float);
    virtual void SetScale(float, float);
    virtual void Draw(RCMP::FRAME*);
    virtual void ReDraw();
    virtual void CompleteDraw();

    RCMP::DECODER* m_decoder;
    tBigYUVSwizzler* m_swizzler;
    unsigned char unknown0c[96];
    RCMP::FRAME* m_frame;
    float m_width;
    float m_height;
    float m_z;
    float m_x;
    float m_y;
    float m_right;
    float m_bottom;
    float m_scaleX;
    float m_scaleY;
};

GCHW_VD::~GCHW_VD() {
    DELETE_tBigYUVSwizzler(m_swizzler);
    if (m_frame)
        m_decoder->ReleaseFrame(m_frame);
}

void GCHW_VD::SetScale(float x, float y) {
    m_scaleX = x;
    m_scaleY = y;
    m_right = m_width * m_scaleX + m_x;
    m_bottom = m_height * m_scaleY + m_y;
}

void GCHW_VD::SetPosition(float x, float y, float z) {
    m_x = x;
    m_y = y;
    m_right = m_width * m_scaleX + m_x;
    m_bottom = m_height * m_scaleY + m_y;
    m_z = z;
}
