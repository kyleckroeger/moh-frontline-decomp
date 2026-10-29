// A fragment of swvideodisplaylist.cpp (0x80108648): the GCSW_VD destructor
// (VIDEO_DRIVER's inline destructor, then the RCMP system's free as the class
// delete) and RCMP::GCSW_VD_create, which allocates a 28-byte GCSW_VD through
// the system's allocator hook and constructs it inline: the decoder kept by
// the VIDEO_DRIVER base, the frame, display target and buffer kept, and the
// picture centred on the target (half the target height, as the target is
// interlaced). The file name is this project's; the original record is
// swvideodisplaylist.cpp. The destructor is GCSW_VD's key function, so this
// unit is compiled with RTTI on and holds the class's virtual table and the
// type information of VIDEO_DRIVER and GCSW_VD (class names and the empty
// allocation name are entries of the file's .rodata and .sdata2 pools).
// GCSW_VD, VIDEO_DRIVER, DECODER, FRAME, rcmp_sys and _GXRenderModeObj are
// named by the mangled symbols; the members, the allocator hook (as in
// audioplayer.cpp), the constructors and the frame-header and display-target
// views are inferred, and the virtual functions are declared in table order.
struct _GXRenderModeObj {
    unsigned char unknown00[4];
    unsigned short fbWidth;
    unsigned short efbHeight;
};

struct FrameHeaderView {
    unsigned char unknown00[4];
    short width;
    short height;
};

namespace RCMP {

class FRAME {
public:
    unsigned char unknown00[4];
    FrameHeaderView* header;
};

class DECODER;

class RCMP_SYSTEM {
public:
    virtual ~RCMP_SYSTEM();

    void* (*m_alloc)(const char*, int, int, int, int);
    void (*m_free)(void*);
    int m_userValue;
};

extern RCMP_SYSTEM rcmp_sys;

class VIDEO_DRIVER {
public:
    VIDEO_DRIVER(DECODER* decoder) : m_decoder(decoder) {}
    virtual ~VIDEO_DRIVER() {}
    virtual void SetPosition(float, float, float) = 0;
    virtual void SetScale(float, float) = 0;
    virtual void Draw(FRAME*) = 0;
    virtual void ReDraw() = 0;
    virtual void CompleteDraw();
    static void* operator new(unsigned long size) { return rcmp_sys.m_alloc("", size, 0, 0, rcmp_sys.m_userValue); }
    static void operator delete(void* p) { rcmp_sys.m_free(p); }

    DECODER* m_decoder;
};

VIDEO_DRIVER* GCSW_VD_create(DECODER*, FRAME*, _GXRenderModeObj*, void*);

}

class GCSW_VD : public RCMP::VIDEO_DRIVER {
public:
    GCSW_VD(RCMP::DECODER* decoder, RCMP::FRAME* frame, _GXRenderModeObj* target, void* buffer)
        : RCMP::VIDEO_DRIVER(decoder), m_target(target), m_buffer(buffer) {
        m_frame = frame;
        FrameHeaderView* header = frame->header;
        _GXRenderModeObj* t = m_target;
        int w = header->width;
        int fw = t->fbWidth;
        int eh = t->efbHeight / 2;
        int h = header->height;
        m_x = (fw - w) / 2;
        m_y = (eh - h) / 2;
    }
    virtual ~GCSW_VD();
    virtual void SetPosition(float, float, float);
    virtual void SetScale(float, float);
    virtual void Draw(RCMP::FRAME*);
    virtual void ReDraw();
    virtual void CompleteDraw();

    RCMP::FRAME* m_frame;
    _GXRenderModeObj* m_target;
    void* m_buffer;
    int m_x;
    int m_y;
};

GCSW_VD::~GCSW_VD() {
}

RCMP::VIDEO_DRIVER* RCMP::GCSW_VD_create(DECODER* decoder, FRAME* frame, _GXRenderModeObj* target, void* buffer) {
    return new GCSW_VD(decoder, frame, target, buffer);
}
