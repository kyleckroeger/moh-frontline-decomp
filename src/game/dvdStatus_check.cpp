// A fragment of dvdStatus_base.c (0x80108e18; compiled as C++): CheckDVDError
// polls the drive until its state settles - honouring the reset button (wait
// for its release, clear the module area, hot reset) and a held Start+B+X on
// any pad (reset after half a second) - and shows a message for the fatal
// states (no disc, cover open, wrong disc, read error, fatal error) through
// ShowDVDError until the drive recovers; it returns 2 after an error was shown
// (the screen is blacked out again), else 0. The names come from the symbols
// and strings; parameter and local names are inferred. The pad check is an
// inferred inline helper that owns the two function statics (it gives the
// original's loop registers). ShowDVDError follows: it sets up GX and VI on
// first use, loads the ROM font, clears the screen with a black quad and draws
// the message lines for the drive state. The black colour comes from an
// inferred inline (Black); it gives the original's stack slots. The rest of the
// file is in other units.
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

static int got_reset;
static void ShowDVDError(int error);

/* the file's other globals and functions are in other units; file-local in the original */
extern GXRenderModeObj gRMode;
extern void* gFrameBuffer;
extern u32 gHeapPtr;
extern OSFontHeader* FontData;
extern s16 FontSize;
extern s16 FontSpace;
extern "C" void GxAndVIInit();
void DrawMessage(int y, char* text);

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
