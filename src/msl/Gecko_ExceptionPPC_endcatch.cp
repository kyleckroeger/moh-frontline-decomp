// Runtime Gecko_ExceptionPPC.cp: ExPPC_LongJump, __throw and __end__catch.
// ExPPC_LongJump and __throw are assembly functions in the runtime, written
// here from the disassembly: LongJump restores the callee-saved registers
// (GPRs, FPR doubles and paired-single halves) and the condition register
// from a ThrowContext and jumps to the catch handler on the thrower's stack;
// __throw saves them into a ThrowContext on its own frame and calls
// ExPPC_ThrowHandler. __end__catch destroys the caught exception object (the
// catch information's location, with its destructor called with -1) when it
// has one; the CatchInfo layout (location first, destructor at +8) is
// inferred from the offsets. The rest of the file is in other units.

typedef struct FPRSave {
    double d;
    float ps[2];
} FPRSave;

typedef struct ThrowContext {
    FPRSave FPR[32];
    long GPR[32];
    long CR;
    char* SP;
    char* FP;
    char* throwSP;
    char* returnaddr;
    char* throwtype;
    void* location;
    void* dtor;
    void* catchinfo;
} ThrowContext;

/* local to the file; ExPPC_ThrowHandler is in another unit */
void ExPPC_ThrowHandler(ThrowContext* context);

static asm void ExPPC_LongJump(register ThrowContext* context, register void* newRTOC, register void* newPC) {
    nofralloc
    mr r8, newPC
    mr r2, newRTOC
    lwz r0, context->CR
    mtcrf 255, r0
    lmw r13, context->GPR[13]
    addi r7, r3, 232
    psq_lx f14, r0, r7, 0, 0
    lfd f14, 224(r3)
    addi r7, r3, 248
    psq_lx f15, r0, r7, 0, 0
    lfd f15, 240(r3)
    addi r7, r3, 264
    psq_lx f16, r0, r7, 0, 0
    lfd f16, 256(r3)
    addi r7, r3, 280
    psq_lx f17, r0, r7, 0, 0
    lfd f17, 272(r3)
    addi r7, r3, 296
    psq_lx f18, r0, r7, 0, 0
    lfd f18, 288(r3)
    addi r7, r3, 312
    psq_lx f19, r0, r7, 0, 0
    lfd f19, 304(r3)
    addi r7, r3, 328
    psq_lx f20, r0, r7, 0, 0
    lfd f20, 320(r3)
    addi r7, r3, 344
    psq_lx f21, r0, r7, 0, 0
    lfd f21, 336(r3)
    addi r7, r3, 360
    psq_lx f22, r0, r7, 0, 0
    lfd f22, 352(r3)
    addi r7, r3, 376
    psq_lx f23, r0, r7, 0, 0
    lfd f23, 368(r3)
    addi r7, r3, 392
    psq_lx f24, r0, r7, 0, 0
    lfd f24, 384(r3)
    addi r7, r3, 408
    psq_lx f25, r0, r7, 0, 0
    lfd f25, 400(r3)
    addi r7, r3, 424
    psq_lx f26, r0, r7, 0, 0
    lfd f26, 416(r3)
    addi r7, r3, 440
    psq_lx f27, r0, r7, 0, 0
    lfd f27, 432(r3)
    addi r7, r3, 456
    psq_lx f28, r0, r7, 0, 0
    lfd f28, 448(r3)
    addi r7, r3, 472
    psq_lx f29, r0, r7, 0, 0
    lfd f29, 464(r3)
    addi r7, r3, 488
    psq_lx f30, r0, r7, 0, 0
    lfd f30, 480(r3)
    addi r7, r3, 504
    psq_lx f31, r0, r7, 0, 0
    lfd f31, 496(r3)
    mtlr r8
    lwz r1, context->throwSP
    lwz r3, context->SP
    lwz r3, 0(r3)
    stw r3, 0(r1)
    blr
}

extern "C" asm void __throw(char* throwtype, void* location, void* dtor) {
    ThrowContext throwcontext;
    fralloc
    stmw r13, throwcontext.GPR[13]
    stfd f14, throwcontext.FPR[14].d
    addi r3, r1, 256
    psq_stx f14, r0, r3, 0, 0
    stfd f15, throwcontext.FPR[15].d
    addi r3, r1, 272
    psq_stx f15, r0, r3, 0, 0
    stfd f16, throwcontext.FPR[16].d
    addi r3, r1, 288
    psq_stx f16, r0, r3, 0, 0
    stfd f17, throwcontext.FPR[17].d
    addi r3, r1, 304
    psq_stx f17, r0, r3, 0, 0
    stfd f18, throwcontext.FPR[18].d
    addi r3, r1, 320
    psq_stx f18, r0, r3, 0, 0
    stfd f19, throwcontext.FPR[19].d
    addi r3, r1, 336
    psq_stx f19, r0, r3, 0, 0
    stfd f20, throwcontext.FPR[20].d
    addi r3, r1, 352
    psq_stx f20, r0, r3, 0, 0
    stfd f21, throwcontext.FPR[21].d
    addi r3, r1, 368
    psq_stx f21, r0, r3, 0, 0
    stfd f22, throwcontext.FPR[22].d
    addi r3, r1, 384
    psq_stx f22, r0, r3, 0, 0
    stfd f23, throwcontext.FPR[23].d
    addi r3, r1, 400
    psq_stx f23, r0, r3, 0, 0
    stfd f24, throwcontext.FPR[24].d
    addi r3, r1, 416
    psq_stx f24, r0, r3, 0, 0
    stfd f25, throwcontext.FPR[25].d
    addi r3, r1, 432
    psq_stx f25, r0, r3, 0, 0
    stfd f26, throwcontext.FPR[26].d
    addi r3, r1, 448
    psq_stx f26, r0, r3, 0, 0
    stfd f27, throwcontext.FPR[27].d
    addi r3, r1, 464
    psq_stx f27, r0, r3, 0, 0
    stfd f28, throwcontext.FPR[28].d
    addi r3, r1, 480
    psq_stx f28, r0, r3, 0, 0
    stfd f29, throwcontext.FPR[29].d
    addi r3, r1, 496
    psq_stx f29, r0, r3, 0, 0
    stfd f30, throwcontext.FPR[30].d
    addi r3, r1, 512
    psq_stx f30, r0, r3, 0, 0
    stfd f31, throwcontext.FPR[31].d
    addi r3, r1, 528
    psq_stx f31, r0, r3, 0, 0
    mfcr r3
    stw r3, throwcontext.CR
    lwz r3, 0(r1)
    lwz r4, 4(r3)
    stw r3, throwcontext.SP
    stw r3, throwcontext.throwSP
    stw r4, throwcontext.returnaddr
    lwz r3, throwtype
    stw r3, throwcontext.throwtype
    lwz r3, location
    stw r3, throwcontext.location
    lwz r3, dtor
    stw r3, throwcontext.dtor
    la r3, throwcontext
    bl ExPPC_ThrowHandler
    nop
    frfree
    blr
}

typedef struct CatchInfo {
    void* location;
    void* typeinfo;
    void* dtor;
} CatchInfo;

extern "C" void __end__catch(CatchInfo* catchinfo) {
    if (catchinfo->location && catchinfo->dtor)
        ((void (*)(void*, short))catchinfo->dtor)(catchinfo->location, -1);
}
