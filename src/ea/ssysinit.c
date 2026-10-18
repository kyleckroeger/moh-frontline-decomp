// Sound system shutdown and state query. sndgs's layout is not known; its
// fields (the module shutdown hooks, the voice and bank tables and the
// initialised flag) are accessed through inferred offsets.
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

// SNDSYS_getopts and SNDSYS_setopts (in ssysinit_opts.c) and SNDSYSI_init
// come first in the original file, so this unit starts at SNDSYS_restore.

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
