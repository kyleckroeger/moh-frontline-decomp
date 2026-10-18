// Sound system start-up, shutdown and state query. SNDSYSI_init marks the
// library author string, does nothing when the system is already initialised,
// clears and hands the memory block to the sound allocator, fetches and
// applies the default options when no voice count is set, seeds the random
// generator, allocates the voice (128 bytes each) and bank (12 bytes each)
// tables under the critical section, resets the state, starts the platform
// (undoing it on failure), marks the system initialised and sets the default
// output volume pairs. sndgs's layout is not known; its fields (the options,
// the module shutdown hooks, the voice and bank tables, the initialised flag
// and the volume tables) are accessed through inferred offsets.
extern "C" char sndgs[];

typedef void (*SNDHOOK)(void);
typedef void (*SNDHOOKARG)(int);

extern "C" {
int SNDfxinitbus(int, int, int, int, int);
void SNDstopall(void);
void SNDSYS_entercritical(void);
void SNDSYS_leavecritical(void);
}
void SNDPLATFORM_restore(void);
void SNDMEMI_free(void*);
int SNDMEMI_restore(void);
void SNDI_mutexfree(void);
int SNDPLATFORM_init(void);
void SNDMEMI_init(void*, int);
void* SNDMEMI_allocz(int);
void SNDI_randomseed(unsigned int);
void SNDI_mutexalloc(void);
void SNDI_precalcaztospkrvol(void);
extern "C" {
void* memset(void*, int, unsigned long);
int SNDSYS_getopts(void*);
int SNDSYS_setopts(void*);
}
extern "C" char sndlibauthor[];

// SNDSYS_getopts and SNDSYS_setopts come first in the original file (they are
// in ssysinit_opts.c), so this unit starts at SNDSYSI_init.

extern "C" int SNDSYSI_init(void* memory, int size) {
    int result;

    sndlibauthor[0] = 'S';
    if (*(signed char*)(sndgs + 360))
        return 0;
    memset(memory, 0, size);
    SNDMEMI_init(memory, size);
    if (*(short*)(sndgs + 368) == 0) {
        result = SNDSYS_getopts(sndgs);
        if (result < 0)
            return result;
        SNDSYS_setopts(sndgs);
    }
    SNDI_randomseed(*(unsigned int*)(sndgs + 36));
    SNDI_mutexalloc();
    SNDSYS_entercritical();
    *(void**)(sndgs + 468) = SNDMEMI_allocz(*(short*)(sndgs + 368) << 7);
    *(void**)(sndgs + 472) = SNDMEMI_allocz(*(unsigned short*)(sndgs + 40) * 12);
    SNDSYS_leavecritical();
    *(int*)(sndgs + 372) = 0;
    sndgs[361] = 127;
    sndgs[366] = 0;
    sndgs[364] = 0;
    sndgs[365] = 0;
    result = SNDPLATFORM_init();
    if (result < 0) {
        SNDPLATFORM_restore();
        SNDI_mutexfree();
        return result;
    }
    sndgs[360] = 1;
    if ((unsigned char)sndgs[72] == 2) {
        *(unsigned short*)(sndgs + 528) = 0x10000 - 0x4000;
        *(short*)(sndgs + 530) = 0x4000;
    } else {
        *(unsigned short*)(sndgs + 528) = 0x10000 - 0x2000;
        *(short*)(sndgs + 530) = 0x2000;
    }
    *(unsigned short*)(sndgs + 536) = *(unsigned short*)(sndgs + 140);
    *(unsigned short*)(sndgs + 538) = *(unsigned short*)(sndgs + 136);
    *(unsigned short*)(sndgs + 540) = *(unsigned short*)(sndgs + 138);
    *(unsigned short*)(sndgs + 544) = 0x10000 - 0x2000;
    *(short*)(sndgs + 546) = 0x2000;
    *(unsigned short*)(sndgs + 548) = 0x10000 - 0x6000;
    *(short*)(sndgs + 550) = 0x6000;
    SNDI_precalcaztospkrvol();
    return 0;
}

extern "C" int SNDSYS_inited(void);

extern "C" int SNDSYS_restore(void) {
    int result;

    if (!SNDSYS_inited())
        return -14;
    SNDfxinitbus(0, 0, 0, -1, -1);
    if (*(SNDHOOK*)(sndgs + 440))
        (*(SNDHOOK*)(sndgs + 440))();
    if (*(SNDHOOK*)(sndgs + 444))
        (*(SNDHOOK*)(sndgs + 444))();
    if (*(SNDHOOK*)(sndgs + 448))
        (*(SNDHOOK*)(sndgs + 448))();
    if (*(SNDHOOK*)(sndgs + 460))
        (*(SNDHOOK*)(sndgs + 460))();
    if (*(SNDHOOK*)(sndgs + 464))
        (*(SNDHOOK*)(sndgs + 464))();
    if (*(SNDHOOK*)(sndgs + 456))
        (*(SNDHOOK*)(sndgs + 456))();
    SNDstopall();
    if (*(SNDHOOKARG*)(sndgs + 452))
        (*(SNDHOOKARG*)(sndgs + 452))(-1);
    SNDPLATFORM_restore();
    SNDSYS_entercritical();
    SNDMEMI_free(*(void**)(sndgs + 468));
    SNDMEMI_free(*(void**)(sndgs + 472));
    SNDSYS_leavecritical();
    result = SNDMEMI_restore();
    sndgs[360] = 0;
    SNDI_mutexfree();
    return result;
}

extern "C" int SNDSYS_inited(void) {
    return *(signed char*)(sndgs + 360);
}
