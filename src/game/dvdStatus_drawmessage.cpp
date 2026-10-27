// A fragment of dvdStatus_base.c (0x80108e18; compiled as C++): DrawMessage
// sets up one replacing TEV stage, measures the message (the widest of
// its lines, each character's width scaled by the font size over the cell
// width plus the spacing), centres it on the frame buffer, moves the line
// down by the font's leading at the current size, enables additive blending
// and draws it with RFPuts, the SDK's DEMORFPuts (DEMOPuts.c) without the z
// coordinate: its LoadSheet and DrawFontChar helpers are inlined, characters
// are placed by the font's ascent and descent at the current size and lines
// advanced by the scaled leading. The names come from the symbols (the two
// helpers from the SDK source); DrawMessage, RFPuts and the file's globals
// are file-local in the original and the globals are declared extern here.
// The texture coordinate locals' types are inferred from the code (no sign
// extensions for the left, top and right ones). The walk to the message's
// end before centring (its result unused) is kept as the original has it;
// the rounding to 16-pixel units is an inferred inline helper. The rest of
// the file is in other units.
#include <dolphin/gx.h>
#include <dolphin/os.h>
#include <dolphin/mtx.h>

extern OSFontHeader* FontData;
extern s16 FontSize;
extern s16 FontSpace;
extern GXRenderModeObj gRMode;

static int RFPuts(s16, s16, char*);
extern void* LastSheet;

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
