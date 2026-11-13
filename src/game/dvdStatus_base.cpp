// dvdStatus_base.c (compiled as C++): the disc error screen.
// DrawMessage sets up one replacing TEV stage, measures the message (the
// widest of its lines, each character's width scaled by the font size over
// the cell width plus the spacing), centres it on the frame buffer, moves the
// line down by the font's leading at the current size, enables additive
// blending and draws it with RFPuts, the SDK's DEMORFPuts (DEMOPuts.c)
// without the z coordinate: its LoadSheet and DrawFontChar helpers are
// inlined, characters are placed by the font's ascent and descent at the
// current size and lines advanced by the scaled leading. The texture
// coordinate locals' types are inferred from the code (no sign extensions
// for the left, top and right ones). The walk to the message's end before
// centring (its result unused) is kept as the original has it; the rounding
// to 16-pixel units is an inferred inline helper.
// CheckDVDError polls the drive until its state settles - honouring the
// reset button (wait for its release, clear the module area, hot reset) and
// a held Start+B+X on any pad (reset after half a second) - and shows a
// message for the fatal states (no disc, cover open, wrong disc, read error,
// fatal error) through ShowDVDError until the drive recovers; it returns 2
// after an error was shown (the screen is blacked out again), else 0. The pad
// check is an inferred inline helper that owns the two function statics (it
// gives the original's loop registers). ShowDVDError sets up GX and VI on
// first use, loads the ROM font, clears the screen with a black quad and
// draws the message lines for the drive state. The black colour comes from an
// inferred inline (Black); it gives the original's stack slots.
// GxAndVIInit sets up GX and VI for the disc error screen on first use: the
// render mode by TV format with overscan, the external frame buffer and the
// FIFO placed from 0x80700000 on (gHeapPtr counts the used bytes), the
// viewport, copy and pixel settings, then the first frame.
// The names come from the symbols (the two DEMOPuts helpers from the SDK
// source) and strings; parameter and local names are inferred. Every global
// but gGxIsInit is file-local.
extern "C" {
#include <stdio.h>
#include <string.h>
}
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <dolphin/os.h>
#include <dolphin/pad.h>
#include <dolphin/vi.h>
#include <dolphin/dvd.h>

static OSFontHeader* FontData;
static void* LastSheet;
static s16 FontSize;
static s16 FontSpace;
static GXRenderModeObj gRMode;
static GXFifoObj* gDefaultFifoObj;
static void* gFrameBuffer;
static u32 gHeapPtr;
static void* gFifoBuf;
extern "C" int gGxIsInit;
int gGxIsInit;
static int got_reset;

static int RFPuts(s16, s16, char*);
static void ShowDVDError(int error);
static void DrawMessage(int y, char* message);
extern "C" void GxAndVIInit();

/* inferred: the width in 16-pixel units, rounded up */
inline int RoundUpTo16(int value) {
    return (value + 15) / 16;
}

static void DrawMessage(int y, char* message) {
    char* p;
    s32 charWidth;
    int widest;
    int width;
    int length;
    int x;

    GXSetNumTevStages(1);
    GXSetTevOp(GX_TEVSTAGE0, GX_REPLACE);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    p = message;
    width = 0;
    widest = 0;
    while (*p) {
        if (*p == '\n') {
            if (widest < width)
                widest = width;
            width = 0;
        }
        p = OSGetFontWidth(p, &charWidth);
        width += FontSize * charWidth / FontData->cellWidth + FontSpace;
    }
    if (widest < width)
        widest = width;
    length = RoundUpTo16(widest);
    for (p = message; *p; p++)
        ;
    x = (gRMode.fbWidth - length) / 2;
    y += FontData->leading * (FontSize / 16) / FontData->cellWidth;
    GXSetBlendMode(GX_BM_BLEND, GX_BL_ONE, GX_BL_ONE, GX_LO_CLEAR);
    RFPuts(x, y, message);
}

/* the SDK's DEMOPuts LoadSheet: load a font sheet unless it is current */
inline void LoadSheet(void* image, GXTexMapID texMapID) {
    Mtx mtx;
    GXTexObj texObj;

    if (LastSheet != image) {
        LastSheet = image;
        GXInitTexObj(&texObj, image, FontData->sheetWidth, FontData->sheetHeight, (GXTexFmt)FontData->sheetFormat,
                     GX_CLAMP, GX_CLAMP, GX_FALSE);
        GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
        GXLoadTexObj(&texObj, texMapID);
        MTXScale(mtx, 1.0f / FontData->sheetWidth, 1.0f / FontData->sheetHeight, 1.0f);
        GXLoadTexMtxImm(mtx, GX_TEXMTX0, GX_MTX2x4);
        GXSetNumTexGens(1);
        GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
    }
}

/* DEMOPuts' DrawFontChar, placed by the font's ascent and descent */
inline void DrawFontChar(int x, int y, int z, int xChar, int yChar) {
    s16 posLeft = x;
    s16 posRight = posLeft + FontSize;
    s16 posTop = y - (FontData->ascent * FontSize) / FontData->cellWidth;
    s16 posBottom = y + (FontData->descent * FontSize) / FontData->cellWidth;
    int texLeft = xChar;
    int texTop = yChar;
    u16 texRight = xChar + FontData->cellWidth;
    s16 texBottom = yChar + FontData->cellHeight;

    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    GXPosition3s16(posLeft, posTop, z);
    GXTexCoord2s16(texLeft, texTop);
    GXPosition3s16(posRight, posTop, z);
    GXTexCoord2s16(texRight, texTop);
    GXPosition3s16(posRight, posBottom, z);
    GXTexCoord2s16(texRight, texBottom);
    GXPosition3s16(posLeft, posBottom, z);
    GXTexCoord2s16(texLeft, texBottom);
    GXEnd();
}

/* the SDK's DEMORFPuts without z, lines advanced by the scaled leading */
static int RFPuts(s16 x, s16 y, char* string) {
    s32 cx;
    void* image;
    s32 xChar;
    s32 yChar;
    s32 width;

    LastSheet = NULL;
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 4);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 0);

    x *= 16;
    y *= 16;

    width = 0;
    while (*string != 0) {
        if (*string == '\n') {
            width = 0;
            y += (FontData->leading * FontSize) / FontData->cellWidth;
            string++;
        } else {
            string = OSGetFontTexture(string, &image, &xChar, &yChar, &cx);
            LoadSheet(image, GX_TEXMAP0);
            DrawFontChar(x + width, y, 0, xChar, yChar);
            width = FontSpace + ((FontSize * cx) / FontData->cellWidth) + width;
        }
    }
    return (width + 15) / 16;
}

static inline void CheckPadReset(PADStatus* pads) {
    static u8 got_reset[4] = { 0 };
    static u32 reset_time[4] = { 0 };
    int i;

    for (i = 0; i < 4; i++) {
        if (pads[i].err == PAD_ERR_NONE) {
            if ((pads[i].button & 0x1600) == 0x1600) {
                u32 now = OSTicksToMilliseconds(OSGetTime());
                if (!got_reset[i])
                    reset_time[i] = now;
                if (now - reset_time[i] > 500)
                    OSResetSystem(OS_RESET_HOTRESET, 1, FALSE);
                got_reset[i] = 1;
            } else {
                got_reset[i] = 0;
            }
        }
    }
}

extern "C" int CheckDVDError() {
    int shown = 0;
    s32 last = -9999;
    s32 status;
    PADStatus pads[4];

    for (;;) {
        do {
            status = DVDGetDriveStatus();
            if (OSGetResetButtonState())
                ::got_reset = 1;
            if (::got_reset) {
                while (OSGetResetButtonState())
                    ;
                printf("Reset button released. Reset NOW.\n");
                memset((void*)0x80003100, 0, 0x4000);
                OSResetSystem(OS_RESET_HOTRESET, 1, FALSE);
            }
            PADRead(pads);
            CheckPadReset(pads);
        } while (status == last);
        if (status != DVD_STATE_END && status != DVD_STATE_BUSY)
            printf("CheckDVDError: Trapped error %d\n", status);
        last = status;
        switch (status) {
        case DVD_STATE_FATAL_ERROR:
        case DVD_STATE_NO_DISK:
        case DVD_STATE_COVER_OPEN:
        case DVD_STATE_WRONG_DISK:
        case DVD_STATE_RETRY:
            ShowDVDError(status);
            shown = 2;
            continue;
        case DVD_STATE_WAITING:
        case DVD_STATE_COVER_CLOSED:
        case DVD_STATE_MOTOR_STOPPED:
        case DVD_STATE_PAUSING:
        case DVD_STATE_CANCELED:
        case DVD_STATE_IGNORED:
            printf("CheckDVDError: Ignoring %d\n", status);
            continue;
        case DVD_STATE_BUSY:
            if (!shown)
                break;
            printf("CheckDVDError: Trapped error %d. Previous error condition, \n", status);
            printf("\tso we're waiting for everything to chill out.\n");
            continue;
        }
        break;
    }
    if (shown) {
        VISetBlack(TRUE);
        VIFlush();
    }
    return shown;
}

static inline void SetMatColor(GXColor c) {
    GXSetChanMatColor(GX_COLOR0, c);
}
static inline GXColor Black() {
    GXColor c = { 0, 0, 0, 255 };
    return c;
}

static void ShowDVDError(int error) {
    Mtx identity;
    Mtx44 projection;

    GxAndVIInit();
    FontData = (OSFontHeader*)(gHeapPtr + 0x80700000);
    OSInitFont(FontData);
    FontSize = FontData->cellWidth * 16;
    FontSpace = -16;
    int width;
    int height = gRMode.efbHeight;
    GXColor black = Black();
    width = gRMode.fbWidth;
    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    SetMatColor(black);
    GXSetBlendMode(GX_BM_BLEND, GX_BL_ONE, GX_BL_ZERO, GX_LO_CLEAR);
    GXSetNumTexGens(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0);
    GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
    GXSetCullMode(GX_CULL_NONE);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);
    MTXOrtho(projection, 0.0f, height, 0.0f, width, 0.0f, 1.0f);
    GXSetProjection(projection, GX_ORTHOGRAPHIC);
    MTXIdentity(identity);
    GXLoadPosMtxImm(identity, GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    GXPosition3s16(0, 0, 0);
    GXPosition3s16(width, 0, 0);
    GXPosition3s16(width, height, 0);
    GXPosition3s16(0, height, 0);
    GXEnd();
    switch (error) {
    case DVD_STATE_COVER_OPEN:
        DrawMessage(196, "The Disc Cover is open.");
        DrawMessage(232, "If you want to continue the game, ");
        DrawMessage(260, "please close the Disc Cover.");
        break;
    case DVD_STATE_NO_DISK:
        DrawMessage(196, "Please insert the");
        DrawMessage(232, "Medal of Honor Frontline");
        DrawMessage(260, "Game Disc.");
        break;
    case DVD_STATE_WRONG_DISK:
        DrawMessage(196, "This is not the Medal of Honor Frontline Game Disc.");
        DrawMessage(232, "Please insert the Medal of Honor Frontline");
        DrawMessage(260, "Game Disc.");
        break;
    case DVD_STATE_RETRY:
        DrawMessage(186, "The Game Disc could not be read.");
        DrawMessage(232, "Please read the");
        DrawMessage(260, "Nintendo GameCube\x99 Instruction Booklet");
        DrawMessage(288, "for more information.");
        break;
    case DVD_STATE_FATAL_ERROR:
        DrawMessage(186, "An error has occurred.");
        DrawMessage(232, "Turn off the power and refer to the");
        DrawMessage(260, "Nintendo GameCube\x99 Instruction Booklet");
        DrawMessage(288, "for further instructions.");
        break;
    }
    VISetBlack(FALSE);
    u8* frameBuffer = (u8*)gFrameBuffer;
    GXCopyDisp(frameBuffer, GX_TRUE);
    GXDrawDone();
    VISetNextFrameBuffer(frameBuffer);
    VIFlush();
    VIWaitForRetrace();
}

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
