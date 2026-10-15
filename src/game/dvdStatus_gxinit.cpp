// A fragment of dvdStatus_base.c (0x80109784; compiled as C++): GxAndVIInit
// sets up GX and VI for the disc error screen on first use: the render mode
// by TV format with overscan, the external frame buffer and the FIFO placed
// from 0x80700000 on (gHeapPtr counts the used bytes), the viewport, copy
// and pixel settings, then the first frame. The names come from the symbols;
// the file's other globals are file-local in the original and declared
// extern here (gGxIsInit is global). The rest of the file is in other units.
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <dolphin/os.h>
#include <dolphin/vi.h>

extern GXFifoObj* gDefaultFifoObj;
extern void* gFrameBuffer;
extern u32 gHeapPtr;
extern void* gFifoBuf;
extern GXRenderModeObj gRMode;
extern "C" int gGxIsInit;

extern "C" void GxAndVIInit() {
    GXRenderModeObj* mode;

    if (gGxIsInit)
        return;
    VIInit();
    switch (VIGetTvFormat()) {
    case VI_NTSC:
        mode = &GXNtsc480IntDf;
        break;
    case VI_PAL:
        mode = &GXPal528IntDf;
        break;
    case VI_MPAL:
        mode = &GXMpal480IntDf;
        break;
    }
    GXAdjustForOverscan(mode, &gRMode, 0, 16);
    gFrameBuffer = (void*)(gHeapPtr + 0x80700000);
    gHeapPtr += ((gRMode.fbWidth + 15) & 0xfff0) * gRMode.xfbHeight * 2;
    gFifoBuf = (void*)(gHeapPtr + 0x80700000);
    gHeapPtr += 0x40000;
    gDefaultFifoObj = GXInit(gFifoBuf, 0x40000);
    GXSetViewport(0.0f, 0.0f, gRMode.fbWidth, gRMode.efbHeight, 0.0f, 1.0f);
    GXSetScissor(0, 0, gRMode.fbWidth, gRMode.efbHeight);
    GXSetDispCopySrc(0, 0, gRMode.fbWidth, gRMode.efbHeight);
    GXSetDispCopyDst(gRMode.fbWidth, gRMode.xfbHeight);
    GXSetDispCopyYScale((f32)gRMode.xfbHeight / (f32)gRMode.efbHeight);
    GXSetCopyFilter(gRMode.aa, gRMode.sample_pattern, GX_TRUE, gRMode.vfilter);
    if (gRMode.aa)
        GXSetPixelFmt(GX_PF_RGB565_Z16, GX_ZC_LINEAR);
    else
        GXSetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);
    GXCopyDisp(gFrameBuffer, GX_TRUE);
    GXSetDispCopyGamma(GX_GM_1_0);
    VIConfigure(&gRMode);
    VISetNextFrameBuffer(gFrameBuffer);
    VIFlush();
    VIWaitForRetrace();
    if (gRMode.viTVmode & 1)
        VIWaitForRetrace();
    gGxIsInit = 1;
}
