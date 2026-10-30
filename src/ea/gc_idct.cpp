// gc_idct.asm: the paired-single 8x8 inverse DCT used by the MPEG decoder.
// The original is an assembly file; it is written here as CodeWarrior
// assembly functions, transcribed from the disassembly. PS_IDCT_row
// transforms the rows of a block and its second entry point (PS_IDCT_col,
// a local label in the original) the columns; GC_IDCT saves f14-f29, loads
// the seven cosine constants, sets GQR7 (16-bit signed quantities, loads
// scaled by 1/16) and runs both passes over the block through a stack buffer.

extern "C" {
static void PS_IDCT_col(void);
}

static float constdata[8] = {
    0.35355338f, 0.46193975f, 0.19134171f, 0.49039263f,
    0.4157348f, 0.27778512f, 0.09754516f, 0.0f,
};

extern "C" asm void PS_IDCT_row(void) {
    nofralloc
    psq_l f0, 0(r3), 0, 7
    psq_l f2, 4(r3), 0, 7
    psq_l f4, 8(r3), 0, 7
    psq_l f6, 12(r3), 0, 7
    psq_l f10, 16(r3), 0, 7
    psq_l f12, 20(r3), 0, 7
    psq_l f14, 24(r3), 0, 7
    psq_l f16, 28(r3), 0, 7
    ps_merge11 f1, f0, f10
    ps_merge00 f0, f0, f10
    ps_merge11 f3, f2, f12
    ps_merge00 f2, f2, f12
    ps_merge11 f5, f4, f14
    ps_merge00 f4, f4, f14
    ps_merge11 f7, f6, f16
    ps_merge00 f6, f6, f16
    ps_mul f18, f22, f0
    ps_madd f10, f23, f2, f18
    ps_madd f11, f24, f2, f18
    ps_madd f12, f23, f6, f18
    ps_madd f13, f22, f4, f18
    ps_madd f10, f22, f4, f10
    ps_madd f10, f24, f6, f10
    ps_mul f18, f23, f6
    ps_mul f19, f22, f4
    ps_mul f20, f24, f6
    ps_nmadd f18, f22, f4, f18
    ps_nmadd f19, f24, f2, f19
    ps_nmadd f20, f23, f2, f20
    ps_add f11, f11, f18
    ps_add f12, f12, f19
    ps_add f13, f13, f20
    ps_mul f18, f25, f1
    ps_mul f19, f26, f1
    ps_mul f20, f27, f1
    ps_mul f21, f28, f1
    ps_madd f18, f26, f3, f18
    ps_madd f20, f28, f5, f20
    ps_madd f21, f26, f5, f21
    ps_madd f18, f27, f5, f18
    ps_madd f20, f26, f7, f20
    ps_madd f18, f28, f7, f18
    ps_mul f14, f27, f7
    ps_nmadd f15, f25, f3, f29
    ps_mul f16, f25, f7
    ps_madd f14, f25, f5, f14
    ps_add f20, f20, f15
    ps_nmadd f16, f27, f3, f16
    ps_nmadd f14, f28, f3, f14
    ps_add f21, f21, f16
    ps_add f19, f19, f14
    ps_sub f17, f10, f18
    ps_sub f16, f11, f19
    ps_sub f15, f12, f20
    ps_sub f14, f13, f21
    ps_add f10, f10, f18
    ps_add f11, f11, f19
    ps_add f12, f12, f20
    ps_add f13, f13, f21
    ps_merge00 f0, f10, f11
    ps_merge11 f10, f10, f11
    ps_merge00 f2, f12, f13
    ps_merge11 f12, f12, f13
    ps_merge00 f4, f14, f15
    ps_merge11 f14, f14, f15
    ps_merge00 f6, f16, f17
    ps_merge11 f16, f16, f17
    psq_st f0, 0(r4), 0, 0
    psq_st f2, 8(r4), 0, 0
    psq_st f4, 16(r4), 0, 0
    psq_st f6, 24(r4), 0, 0
    psq_st f10, 32(r4), 0, 0
    psq_st f12, 40(r4), 0, 0
    psq_st f14, 48(r4), 0, 0
    psq_st f16, 56(r4), 0, 0
    blr
    entry static PS_IDCT_col
    psq_l f0, 0(r4), 0, 0
    psq_l f1, 32(r4), 0, 0
    psq_l f2, 64(r4), 0, 0
    psq_l f3, 96(r4), 0, 0
    psq_l f4, 128(r4), 0, 0
    psq_l f5, 160(r4), 0, 0
    psq_l f6, 192(r4), 0, 0
    psq_l f7, 224(r4), 0, 0
    ps_mul f18, f22, f0
    ps_madd f10, f23, f2, f18
    ps_madd f11, f24, f2, f18
    ps_madd f12, f23, f6, f18
    ps_madd f13, f22, f4, f18
    ps_madd f10, f22, f4, f10
    ps_madd f10, f24, f6, f10
    ps_mul f18, f23, f6
    ps_mul f19, f22, f4
    ps_mul f20, f24, f6
    ps_nmadd f18, f22, f4, f18
    ps_nmadd f19, f24, f2, f19
    ps_nmadd f20, f23, f2, f20
    ps_add f11, f11, f18
    ps_add f12, f12, f19
    ps_add f13, f13, f20
    ps_mul f18, f25, f1
    ps_mul f19, f26, f1
    ps_mul f20, f27, f1
    ps_mul f21, f28, f1
    ps_madd f18, f26, f3, f18
    ps_madd f20, f28, f5, f20
    ps_madd f21, f26, f5, f21
    ps_madd f18, f27, f5, f18
    ps_madd f20, f26, f7, f20
    ps_madd f18, f28, f7, f18
    ps_mul f14, f27, f7
    ps_nmadd f15, f25, f3, f29
    ps_mul f16, f25, f7
    ps_madd f14, f25, f5, f14
    ps_add f20, f20, f15
    ps_nmadd f16, f27, f3, f16
    ps_nmadd f14, f28, f3, f14
    ps_add f21, f21, f16
    ps_add f19, f19, f14
    ps_sub f17, f10, f18
    ps_sub f16, f11, f19
    ps_sub f15, f12, f20
    ps_sub f14, f13, f21
    ps_add f10, f10, f18
    ps_add f11, f11, f19
    ps_add f12, f12, f20
    ps_add f13, f13, f21
    psq_st f10, 0(r3), 0, 7
    psq_st f11, 16(r3), 0, 7
    psq_st f12, 32(r3), 0, 7
    psq_st f13, 48(r3), 0, 7
    psq_st f14, 64(r3), 0, 7
    psq_st f15, 80(r3), 0, 7
    psq_st f16, 96(r3), 0, 7
    psq_st f17, 112(r3), 0, 7
    blr
}

extern "C" asm void GC_IDCT(void) {
    nofralloc
    mflr r0
    stw r0, 4(r1)
    stwu r1, -392(r1)
    stfd f14, 8(r1)
    stfd f15, 16(r1)
    stfd f16, 24(r1)
    stfd f17, 32(r1)
    stfd f18, 40(r1)
    stfd f19, 48(r1)
    stfd f20, 56(r1)
    stfd f21, 64(r1)
    stfd f22, 72(r1)
    stfd f23, 80(r1)
    stfd f24, 88(r1)
    stfd f25, 96(r1)
    stfd f26, 104(r1)
    stfd f27, 112(r1)
    stfd f28, 120(r1)
    stfd f29, 128(r1)
    lis r5, constdata@ha
    addi r5, r5, constdata@l
    lis r6, 1031
    addi r6, r6, 7
    mtspr GQR7, r6
    lfs f22, 0(r5)
    lfs f23, 4(r5)
    lfs f24, 8(r5)
    lfs f25, 12(r5)
    lfs f26, 16(r5)
    lfs f27, 20(r5)
    lfs f28, 24(r5)
    lfs f29, 28(r5)
    lis r7, 0
    addi r8, r3, 0
    addi r4, r1, 136
L_800fd10c:
    addi r7, r7, 1
    bl PS_IDCT_row
    cmpwi r7, 4
    addi r3, r3, 32
    addi r4, r4, 64
    blt L_800fd10c
    lis r7, 0
    addi r3, r8, 0
    addi r4, r1, 136
L_800fd130:
    addi r7, r7, 1
    bl PS_IDCT_col
    cmpwi r7, 4
    addi r3, r3, 4
    addi r4, r4, 8
    blt L_800fd130
    lwz r0, 396(r1)
    mtlr r0
    lfd f14, 8(r1)
    lfd f15, 16(r1)
    lfd f16, 24(r1)
    lfd f17, 32(r1)
    lfd f18, 40(r1)
    lfd f19, 48(r1)
    lfd f20, 56(r1)
    lfd f21, 64(r1)
    lfd f22, 72(r1)
    lfd f23, 80(r1)
    lfd f24, 88(r1)
    lfd f25, 96(r1)
    lfd f26, 104(r1)
    lfd f27, 112(r1)
    lfd f28, 120(r1)
    lfd f29, 128(r1)
    addi r1, r1, 392
    blr
}

