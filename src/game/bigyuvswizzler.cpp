// The start of bigyuvswizzler.cpp (0x800fc5ac): tBigYUVSwizzler_DrawSetup,
// the GX state for drawing a YUV movie frame through two "big swizzler"
// indirect lookups: the three plane textures (nearest filtering, no LOD) and
// the swizzler's three colour lookup tables loaded, the swizzlers' tile and
// pixel maps bound as indirect textures (the first matrix scales by the
// second swizzler's tile size / 16, the second by 1/16), and five TEV stages
// that sum the planes' lookups with two constant colours (yellow, cyan) and
// swap tables. The names come from the symbols; tBigSwizzler's and
// tBigYUVSwizzler's layouts (three 256-entry IA8 tables, the two swizzlers,
// three TLUT objects; 0x640 bytes) are inferred from this file and
// bigswizzler.cpp, as are the parameter names. Then NEW_tBigYUVSwizzlerTexture:
// the structure allocated with the movie allocation flags, both swizzlers
// created, and the three IA8 colour tables filled (YUV to RGB weights; the
// (unsigned char) conversions give the original's twofold loop unrolling),
// flushed and bound to the TLUT objects. DELETE_tBigYUVSwizzler is not part
// of this unit.
#include <dolphin/gx.h>

extern "C" {
void* MEM_alloc(const char*, int, int);
void DCFlushRange(void*, unsigned long);
}

namespace RCMP {
class RCMP_SYSTEM {
public:
    virtual ~RCMP_SYSTEM();

    void* (*m_alloc)(const char*, unsigned long, int, int, int);
    void (*m_free)(void*);
    int m_allocFlags;
};

extern RCMP_SYSTEM rcmp_sys;
}

struct tBigSwizzler {
    unsigned int tileWidth;
    unsigned int tileHeight;
    unsigned int mapWidth;
    unsigned int mapHeight;
    void* tileMap;
    GXTexObj tileTexture;
    unsigned int pixWidth;
    unsigned int pixHeight;
    void* pixMap;
    GXTexObj pixTexture;
};

struct tBigYUVSwizzler {
    unsigned char lut[3][256][2];
    tBigSwizzler* swizzler0;
    tBigSwizzler* swizzler1;
    GXTlutObj tlut[3];
    unsigned char unknown62c[20];
};

tBigSwizzler* NEW_tBigSwizzlerTexture(GXTexObj*);

void tBigYUVSwizzler_DrawSetup(tBigYUVSwizzler* swizzler, GXTexObj* tex0, GXTexObj* tex1, GXTexObj* tex2) {
    tBigSwizzler* swizzler0 = swizzler->swizzler0;
    GXColor k0 = { 0xff, 0xff, 0x00, 0x00 };
    tBigSwizzler* swizzler1 = swizzler->swizzler1;
    GXColor k1 = { 0x00, 0xff, 0xff, 0x00 };
    float mtx[2][3];

    GXSetIndTexCoordScale(GX_INDTEXSTAGE0, GX_ITS_1, GX_ITS_1);
    GXSetIndTexCoordScale(GX_INDTEXSTAGE1, GX_ITS_1, GX_ITS_1);
    GXSetIndTexCoordScale(GX_INDTEXSTAGE2, GX_ITS_1, GX_ITS_1);
    GXSetIndTexCoordScale(GX_INDTEXSTAGE3, GX_ITS_1, GX_ITS_1);
    GXInitTexObjLOD(tex2, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    GXInitTexObjLOD(tex1, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    GXInitTexObjLOD(tex0, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    GXLoadTlut(&swizzler->tlut[2], GX_TLUT0);
    GXLoadTlut(&swizzler->tlut[1], GX_TLUT1);
    GXLoadTlut(&swizzler->tlut[0], GX_TLUT2);
    GXLoadTexObj(tex2, GX_TEXMAP0);
    GXLoadTexObj(tex1, GX_TEXMAP1);
    GXLoadTexObj(tex0, GX_TEXMAP2);
    GXLoadTexObj(&swizzler1->tileTexture, GX_TEXMAP3);
    GXLoadTexObj(&swizzler1->pixTexture, GX_TEXMAP4);
    GXLoadTexObj(&swizzler0->tileTexture, GX_TEXMAP5);
    GXLoadTexObj(&swizzler0->pixTexture, GX_TEXMAP6);
    GXSetTevKColor(GX_KCOLOR0, k0);
    GXSetTevKColor(GX_KCOLOR1, k1);
    GXSetTevSwapModeTable(GX_TEV_SWAP1, GX_CH_ALPHA, GX_CH_RED, GX_CH_RED, GX_CH_RED);
    GXSetTevSwapModeTable(GX_TEV_SWAP2, GX_CH_GREEN, GX_CH_ALPHA, GX_CH_GREEN, GX_CH_GREEN);
    GXSetNumTexGens(2);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEXCOORD0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
    GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEXCOORD0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
    mtx[0][0] = swizzler1->tileWidth / 16.0f;
    mtx[0][1] = 0.0f;
    mtx[1][1] = swizzler1->tileHeight / 16.0f;
    mtx[1][0] = 0.0f;
    mtx[0][2] = 0.0f;
    mtx[1][2] = 0.0f;
    GXSetIndTexMtx(GX_ITM_0, mtx, 4);
    mtx[0][0] = 0.0625f;
    mtx[0][1] = 0.0f;
    mtx[1][1] = 0.0625f;
    mtx[1][0] = 0.0f;
    mtx[0][2] = 0.0f;
    mtx[1][2] = 0.0f;
    GXSetIndTexMtx(GX_ITM_1, mtx, 4);
    GXSetNumIndStages(4);
    GXSetIndTexOrder(GX_INDTEXSTAGE0, GX_TEXCOORD0, GX_TEXMAP3);
    GXSetTevIndirect(GX_TEVSTAGE0, GX_INDTEXSTAGE0, GX_ITF_8, GX_ITB_NONE, GX_ITM_0, GX_ITW_0, GX_ITW_0, GX_FALSE, GX_FALSE, GX_ITBA_OFF);
    GXSetIndTexOrder(GX_INDTEXSTAGE1, GX_TEXCOORD0, GX_TEXMAP4);
    GXSetTevIndirect(GX_TEVSTAGE1, GX_INDTEXSTAGE1, GX_ITF_8, GX_ITB_T, GX_ITM_1, GX_ITW_0, GX_ITW_OFF, GX_TRUE, GX_FALSE, GX_ITBA_OFF);
    GXSetIndTexOrder(GX_INDTEXSTAGE2, GX_TEXCOORD1, GX_TEXMAP5);
    GXSetTevIndirect(GX_TEVSTAGE3, GX_INDTEXSTAGE2, GX_ITF_8, GX_ITB_NONE, GX_ITM_0, GX_ITW_0, GX_ITW_0, GX_FALSE, GX_FALSE, GX_ITBA_OFF);
    GXSetIndTexOrder(GX_INDTEXSTAGE3, GX_TEXCOORD1, GX_TEXMAP6);
    GXSetTevIndirect(GX_TEVSTAGE4, GX_INDTEXSTAGE3, GX_ITF_8, GX_ITB_T, GX_ITM_1, GX_ITW_0, GX_ITW_OFF, GX_TRUE, GX_FALSE, GX_ITBA_OFF);
    GXSetNumTevStages(5);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevIndRepeat(GX_TEVSTAGE2);
    GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD0, GX_TEXMAP1, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD1, GX_TEXMAP2, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE4, GX_TEXCOORD1, GX_TEXMAP2, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
    GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_TEXC, GX_CC_KONST, GX_CC_ZERO);
    GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
    GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP1, GX_TEV_SWAP2);
    GXSetTevKColorSel(GX_TEVSTAGE1, GX_TEV_KCSEL_K1);
    GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_SUBHALF, GX_CS_SCALE_2, GX_FALSE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_TEXC, GX_CC_KONST, GX_CC_CPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
    GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP1, GX_TEV_SWAP1);
    GXSetTevKColorSel(GX_TEVSTAGE2, GX_TEV_KCSEL_K0);
    GXSetTevColorOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_FALSE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE3, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
    GXSetTevColorOp(GX_TEVSTAGE4, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE4, GX_CC_TEXC, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE4, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE4, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
}

tBigYUVSwizzler* NEW_tBigYUVSwizzlerTexture(GXTexObj* tex0, GXTexObj* tex1, GXTexObj*) {
    tBigYUVSwizzler* swizzler = (tBigYUVSwizzler*)MEM_alloc("tBigYUVSwizzler", sizeof(tBigYUVSwizzler), RCMP::rcmp_sys.m_allocFlags);
    swizzler->swizzler0 = NEW_tBigSwizzlerTexture(tex0);
    swizzler->swizzler1 = NEW_tBigSwizzlerTexture(tex1);
    for (int i = 0; i < 256; i++) {
        float value = i - 128.0f;
        swizzler->lut[0][i][0] = 0xff;
        swizzler->lut[0][i][1] = i;
        swizzler->lut[1][i][0] = (unsigned char)(0.701f * value + 128.0f);
        swizzler->lut[1][i][1] = (unsigned char)(-0.35705f * value + 85.333336f);
        swizzler->lut[2][i][0] = (unsigned char)(-0.17205f * value + 42.666668f);
        swizzler->lut[2][i][1] = (unsigned char)(0.886f * value + 128.0f);
    }
    DCFlushRange(swizzler->lut[0], sizeof(swizzler->lut[0]));
    DCFlushRange(swizzler->lut[1], sizeof(swizzler->lut[1]));
    DCFlushRange(swizzler->lut[2], sizeof(swizzler->lut[2]));
    GXInitTlutObj(&swizzler->tlut[0], swizzler->lut[0], GX_TL_IA8, 256);
    GXInitTlutObj(&swizzler->tlut[1], swizzler->lut[1], GX_TL_IA8, 256);
    GXInitTlutObj(&swizzler->tlut[2], swizzler->lut[2], GX_TL_IA8, 256);
    return swizzler;
}
