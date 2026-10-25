// GCSW_VD, the software video display: completing a draw releases the frame to
// the decoder, redrawing waits for the retrace and converts the frame's Y, U
// and V planes (stored after the header or at an offset it gives) into the
// display buffer at the position, Draw stores the frame and redraws, the
// scale setter is empty and the position setter stores integer coordinates.
// GCSW_VD, RCMP::FRAME and RCMP::DECODER are named by the mangled symbols;
// the virtual functions are declared in the order of __vt__7GCSW_VD; the
// members and the frame-header and display-target views are inferred from
// offsets. YUV_convert (file-local) clips the destination rectangle to the
// display, skipping clipped source rows and columns in the three planes, and
// writes each source row twice through YUV_WriteRow into the uncached
// (0x40000000) mirror of the buffer. The rest of the file is not part of this
// unit.

/* inferred view of the decoded frame's header */
struct FrameHeaderView {
    unsigned char unknown00[4];
    short width;
    short height;
    unsigned char unknown08[4];
    unsigned char unknown0c_0 : 3;
    unsigned char external : 1;
    unsigned char unknown0c_1 : 4;
    unsigned char unknown0d[3];
    int dataOffset;
};

namespace RCMP {
class FRAME {
public:
    unsigned char unknown00[4];
    FrameHeaderView* header;
};

class DECODER {
public:
    void ReleaseFrame(FRAME*);
};
}

/* inferred view of the display target */
struct DisplayTargetView {
    unsigned char unknown00[4];
    unsigned short width;
    unsigned short height;
};

extern "C" void VIWaitForRetrace(void);
static int YUV_convert(unsigned char**, int, int, int, int, int, void*, int, int, int);
void YUV_WriteRow(unsigned char*, unsigned char*, unsigned char*, char*, int);

class GCSW_VD {
public:
    virtual ~GCSW_VD();
    virtual void SetPosition(float, float, float);
    virtual void SetScale(float, float);
    virtual void Draw(RCMP::FRAME*);
    virtual void ReDraw();
    virtual void CompleteDraw();

    RCMP::DECODER* m_decoder;
    RCMP::FRAME* m_frame;
    DisplayTargetView* m_target;
    void* m_buffer;
    int m_x;
    int m_y;
};

void GCSW_VD::CompleteDraw() {
    m_decoder->ReleaseFrame(m_frame);
    m_frame = 0;
}

void GCSW_VD::ReDraw() {
    unsigned short stride = (m_target->width + 15) & ~15;
    FrameHeaderView* header = m_frame->header;
    void* buffer = m_buffer;
    unsigned char* data;
    unsigned char* planes[3];
    int width;
    int height;

    if (header->external)
        data = (unsigned char*)header + header->dataOffset;
    else
        data = (unsigned char*)header + 16;
    width = header->width;
    height = header->height;
    planes[0] = data;
    planes[1] = data + width * height;
    planes[2] = planes[1] + (width / 2) * (height / 2);
    VIWaitForRetrace();
    YUV_convert(planes, m_x, m_y, width, width, height, buffer, m_target->width, m_target->height / 2, stride);
}

void GCSW_VD::Draw(RCMP::FRAME* frame) {
    m_frame = frame;
    ReDraw();
}

void GCSW_VD::SetScale(float, float) {
}

void GCSW_VD::SetPosition(float x, float y, float) {
    m_x = x;
    m_y = y;
}

static int YUV_convert(unsigned char** planes, int x, int y, int stride, int width, int height, void* dst, int maxw, int maxh,
                       int pitch) {
    char* uncached = (char*)dst + 0x40000000;
    int halfwidth = width >> 1;
    int i;

    if (x < 0) {
        int d = -x;
        width -= d;
        planes[0] += d;
        planes[1] += d / 2;
        planes[2] += d / 2;
        x = 0;
    }
    if (y < 0) {
        int d = -y;
        int offset = d;
        offset *= stride;
        height -= d;
        planes[0] += offset;
        planes[1] += (d / 2) * (stride / 2);
        planes[2] += (d / 2) * (stride / 2);
        y = 0;
    }
    if (x + width > maxw)
        width -= x + width - maxw;
    if (y + height > maxh)
        height -= y + height - maxh;
    for (i = 0; i < height; i++) {
        unsigned char* yrow = planes[0] + i * width;
        unsigned char* urow = planes[1] + halfwidth * (i >> 1);
        unsigned char* vrow = planes[2] + halfwidth * (i >> 1);

        YUV_WriteRow(yrow, urow, vrow, uncached + x * 2 + pitch * 2 * (y + i * 2), width);
        YUV_WriteRow(yrow, urow, vrow, uncached + x * 2 + pitch * 2 * (y + (i * 2 + 1)), width);
    }
    return 0;
}
