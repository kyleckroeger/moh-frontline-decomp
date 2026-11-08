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

    void* (*m_alloc)(const char*, unsigned long, int, int, int);
    void (*m_free)(void*);
    int m_allocFlags;
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
    virtual void CompleteDraw() {}
    void* operator new(unsigned long size) { return rcmp_sys.m_alloc("", size, 0, 0, rcmp_sys.m_allocFlags); }

    DECODER* m_decoder;
};

VIDEO_DRIVER* MOHFL_VD_create(DECODER*, FRAME*);

}

struct _GXRenderModeObj {
    unsigned char unknown00[4];
    unsigned short fbWidth;
    unsigned short efbHeight;
};

class CScreen {
public:
    int GetHeight();
    int GetWidth();

    unsigned char unknown000[176];
    _GXRenderModeObj* m_renderMode;
    unsigned char unknown0b4[92];
};

extern CScreen g_screen;

tBigYUVSwizzler* NEW_tBigYUVSwizzlerTexture(GXTexObj*, GXTexObj*, GXTexObj*);

class GCHW_VD : public RCMP::VIDEO_DRIVER {
public:
    GCHW_VD(RCMP::DECODER* decoder, RCMP::FRAME* frame, _GXRenderModeObj* mode);
    virtual ~GCHW_VD();
    void operator delete(void* p) { RCMP::rcmp_sys.m_free(p); }
    virtual void SetPosition(float, float, float);
    virtual void SetScale(float, float);
    virtual void Draw(RCMP::FRAME*);
    virtual void ReDraw();
    virtual void CompleteDraw();

    void iDraw();

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
    _GXRenderModeObj* m_renderMode;
};

GCHW_VD::GCHW_VD(RCMP::DECODER* decoder, RCMP::FRAME* frame, _GXRenderModeObj* mode) : RCMP::VIDEO_DRIVER(decoder) {
    short width = frame->m_data->width;
    short height = frame->m_data->height;

    m_width = width;
    m_height = height;
    m_screenHeight = g_screen.GetHeight();
    m_screenWidth = g_screen.GetWidth();
    m_renderMode = mode;
    GXInitTexObjCI(&m_textureY, 0, width, height, 9, 0, 0, 0, 2);
    GXInitTexObjLOD(&m_textureY, 0, 0, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    GXInitTexObjCI(&m_textureV, 0, width / 2, height / 2, 9, 0, 0, 0, 1);
    GXInitTexObjLOD(&m_textureV, 0, 0, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    GXInitTexObjCI(&m_textureU, 0, width / 2, height / 2, 9, 0, 0, 0, 0);
    GXInitTexObjLOD(&m_textureU, 0, 0, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    m_swizzler = NEW_tBigYUVSwizzlerTexture(&m_textureY, &m_textureV, &m_textureU);
    m_frame = 0;
    int fw = m_renderMode->fbWidth;
    int eh = m_renderMode->efbHeight;
    m_x = (fw - width) / 2;
    m_y = (eh - height) / 2;
    m_right = m_x + width;
    m_bottom = m_y + height;
    m_z = 0.0f;
    m_scaleX = 1.0f;
    m_scaleY = 1.0f;
}





RCMP::VIDEO_DRIVER* RCMP::MOHFL_VD_create(DECODER* decoder, FRAME* frame) {
    return new GCHW_VD(decoder, frame, g_screen.m_renderMode);
}
