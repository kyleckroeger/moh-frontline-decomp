// A fragment of dvdStatus_base.c (0x80108e18; compiled as C++): DrawMessage
// sets up one replacing TEV stage, measures the message (the widest of
// its lines, each character's width scaled by the font size over the cell
// width plus the spacing), centres it on the frame buffer, moves the line
// down by the font's leading at the current size, enables additive blending
// and draws it with RFPuts. The names come from the symbols; DrawMessage,
// RFPuts and the file's globals are file-local in the original and declared
// extern here. The walk to the message's end before centring (its result
// unused) is kept as the original has it; the rounding to 16-pixel units is
// an inferred inline helper. The rest of the file is in other units.
#include <dolphin/gx.h>
#include <dolphin/os.h>

extern OSFontHeader* FontData;
extern s16 FontSize;
extern s16 FontSpace;
extern GXRenderModeObj gRMode;

void RFPuts(s16, s16, char*); // file-local in the original

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
