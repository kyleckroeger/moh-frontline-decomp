// GCHW_VD, the hardware video display (0x80102308): iDraw draws the current
// frame - its Y, U and V planes (after the frame header, or at the header's
// offset when its flag is set) are flushed and bound as C8 textures (TLUTs
// 2, 1, 0), the swizzler sets up the YUV combine, and one screen-space quad
// (orthographic over the screen size, depth -1200) is drawn at the display's
// rectangle; the TEV swap tables and direct stages are then reset and the
// vertex format 0 attributes it changed are restored. Then the destructor (the YUV
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
// of the file is not part of this unit. The GX calls are declared as the
// Dolphin SDK has them (the vertex writes are the SDK's inline write-gather
// helpers); the frame header and texture-object members are inferred, and
// the frame variables are declared in the order that gives the original
// registers.
struct tBigYUVSwizzler;
void DELETE_tBigYUVSwizzler(tBigYUVSwizzler*);

/* the Dolphin SDK GX interface as used here */
typedef struct _GXTexObj {
    unsigned int dummy[8];
} GXTexObj;
typedef float Mtx[3][4];
typedef float Mtx44[4][4];

extern "C" {
void GXSetCurrentMtx(unsigned int);
void GXClearVtxDesc();
void GXSetVtxDesc(int, int);
void GXSetCullMode(int);
void GXSetVtxAttrFmt(int, int, int, int, unsigned char);
void GXGetVtxAttrFmt(int, int, int*, int*, unsigned char*);
void C_MTXOrtho(Mtx44, float, float, float, float, float, float);
void GXSetProjection(Mtx44, int);
void PSMTXIdentity(Mtx);
void GXLoadPosMtxImm(Mtx, unsigned int);
void GXSetNumTevStages(unsigned char);
void GXSetTexCoordGen2(int, int, int, unsigned int, unsigned char, unsigned int);
void GXSetNumTexGens(unsigned char);
void GXSetNumChans(unsigned char);
void GXSetChanCtrl(int, unsigned char, int, int, unsigned int, int, int);
void GXSetTevOrder(int, int, int, int);
void GXSetTevOp(int, int);
void DCFlushRangeNoSync(void*, unsigned int);
void GXInitTexObjCI(GXTexObj*, void*, unsigned short, unsigned short, int, int, int, unsigned char, unsigned int);
void GXInitTexObjLOD(GXTexObj*, int, int, float, float, float, unsigned char, unsigned char, int);
void GXSetZMode(unsigned char, int, unsigned char);
void PPCSync();
void GXBegin(int, int, unsigned short);
void GXSetNumIndStages(unsigned char);
void GXSetTevSwapModeTable(int, int, int, int, int);
void GXSetTevDirect(int);
void GXSetTevSwapMode(int, int, int);
}

typedef union {
    unsigned char u8;
    unsigned short u16;
    unsigned int u32;
    float f32;
} PPCWGPipe;

#define GXWGFifo (*(volatile PPCWGPipe*)0xCC008000)

inline void GXPosition3f32(float x, float y, float z) {
    GXWGFifo.f32 = x;
    GXWGFifo.f32 = y;
    GXWGFifo.f32 = z;
}

inline void GXColor1u32(unsigned int color) {
    GXWGFifo.u32 = color;
}

inline void GXTexCoord2f32(float s, float t) {
    GXWGFifo.f32 = s;
    GXWGFifo.f32 = t;
}

inline void GXEnd() {
}

void tBigYUVSwizzler_DrawSetup(tBigYUVSwizzler*, GXTexObj*, GXTexObj*, GXTexObj*);

namespace RCMP {

/* inferred: a decoded frame's header (planes after it or at an offset) */
struct FrameDataView {
    unsigned char unknown00[4];
    short width;
    short height;
    unsigned char unknown08[4];
    unsigned char flags0 : 3;
    unsigned char m_planesAtOffset : 1;
    unsigned char flags1 : 4;
    unsigned char unknown0d[3];
    int offset;
};

class FRAME {
public:
    unsigned char unknown00[4];
    FrameDataView* m_data;
};

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

    void iDraw();

    RCMP::DECODER* m_decoder;
    tBigYUVSwizzler* m_swizzler;
    GXTexObj m_textureY;
    GXTexObj m_textureU;
    GXTexObj m_textureV;
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
    unsigned int m_screenHeight;
    unsigned int m_screenWidth;
};

void GCHW_VD::iDraw() {
    RCMP::FrameDataView* header = m_frame->m_data;
    unsigned int size;
    int halfHeight;
    int halfWidth;
    unsigned char* u;
    unsigned char* y;
    int width;
    int height;
    unsigned char* v;
    if (header->m_planesAtOffset)
        y = (unsigned char*)header + header->offset;
    else
        y = (unsigned char*)header + 16;
    width = header->width;
    height = header->height;
    halfWidth = width / 2;
    size = width * height;
    halfHeight = height / 2;
    u = y + size;
    v = u + halfWidth * halfHeight;
    GXSetCurrentMtx(60);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(11, 1);
    GXSetVtxDesc(13, 1);
    GXSetCullMode(0);
    GXSetVtxAttrFmt(7, 9, 1, 4, 0);
    GXSetVtxAttrFmt(7, 11, 1, 5, 0);
    GXSetVtxAttrFmt(7, 13, 1, 4, 0);
    Mtx view;
    Mtx44 projection;
    C_MTXOrtho(projection, 0.0f, m_screenHeight, 0.0f, m_screenWidth, 5.0f, 2000.0f);
    GXSetProjection(projection, 1);
    PSMTXIdentity(view);
    view[2][3] = -1200.0f;
    GXLoadPosMtxImm(view, 0);
    GXSetCurrentMtx(0);
    GXSetNumTevStages(1);
    GXSetTexCoordGen2(0, 1, 4, 60, 0, 125);
    GXSetNumTexGens(1);
    GXSetNumChans(1);
    GXSetChanCtrl(4, 0, 0, 1, 0, 2, 2);
    GXSetTevOrder(0, 0, 0, 4);
    GXSetTevOp(0, 0);
    DCFlushRangeNoSync(y, size);
    size >>= 2;
    DCFlushRangeNoSync(v, size);
    DCFlushRangeNoSync(u, size);
    GXInitTexObjCI(&m_textureY, y, width, height, 9, 0, 0, 0, 2);
    GXInitTexObjLOD(&m_textureY, 0, 0, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    GXInitTexObjCI(&m_textureV, v, halfWidth, halfHeight, 9, 0, 0, 0, 1);
    GXInitTexObjLOD(&m_textureV, 0, 0, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    GXInitTexObjCI(&m_textureU, u, halfWidth, halfHeight, 9, 0, 0, 0, 0);
    GXInitTexObjLOD(&m_textureU, 0, 0, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    tBigYUVSwizzler_DrawSetup(m_swizzler, &m_textureY, &m_textureV, &m_textureU);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(11, 1);
    GXSetVtxDesc(13, 1);
    int attributes[3] = {9, 11, 13};
    int count[3];
    int type[3];
    unsigned char frac[3];
    unsigned int i;
    for (i = 0; i < 3; i++)
        GXGetVtxAttrFmt(0, attributes[i], &count[i], &type[i], &frac[i]);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 11, 1, 5, 0);
    GXSetVtxAttrFmt(0, 13, 1, 4, 0);
    GXSetZMode(0, 7, 0);
    PPCSync();
    GXBegin(0x80, 0, 4);
    GXPosition3f32(m_x, m_y, m_z);
    GXColor1u32(0xFFFFFFFF);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(m_right, m_y, m_z);
    GXColor1u32(0xFFFFFFFF);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(m_right, m_bottom, m_z);
    GXColor1u32(0xFFFFFFFF);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(m_x, m_bottom, m_z);
    GXColor1u32(0xFFFFFFFF);
    GXTexCoord2f32(0.0f, 1.0f);
    GXEnd();
    GXSetNumIndStages(0);
    GXSetTevSwapModeTable(0, 0, 1, 2, 3);
    GXSetTevSwapModeTable(1, 0, 0, 0, 3);
    GXSetTevSwapModeTable(2, 1, 1, 1, 3);
    GXSetTevSwapModeTable(3, 2, 2, 2, 3);
    GXSetTevDirect(0);
    GXSetTevDirect(1);
    GXSetTevDirect(2);
    GXSetTevDirect(3);
    GXSetTevDirect(4);
    GXSetTevSwapMode(1, 0, 0);
    GXSetTevSwapMode(2, 0, 0);
    for (i = 0; i < 3; i++)
        GXSetVtxAttrFmt(0, attributes[i], count[i], type[i], frac[i]);
}

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
