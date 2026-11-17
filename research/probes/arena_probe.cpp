// settings: config/GMFE69/MallocInit.json
// replaces: mallocinit
//
// Test harness, not game code. Link it into every image that is compared
// frame by frame (with clock_probe.cpp): it keeps the game's main heap below
// 32 MB.
//
// Dolphin runs these images with 64 MB of MEM1 (dolphin_boot.py: the drafts'
// code segment takes heap, and with 24 MB Mission 1 runs out). The main heap
// then reaches 0x827fce00 (mallocinit keeps the top 24 MB of an arena larger
// than 24 MB out of it), and the shell fonts, allocated near its top, sit
// above 0x82000000. A GameCube TLUT load ignores the upper bits of its source
// address, and Dolphin does the same (LOADTLUT1: addr & 0x01FFFFFF), so a
// palette there is read from 32 MB lower: ordinary heap whose contents depend
// on the image's layout. The original and the pure-source image then drew the
// menu's button help text differently (2026-11-17: invisible in one, grey
// boxes in the other) and every menu frame hash differed.
//
// This mallocinit is the game's with the heap's end capped at 0x82000000: every
// palette is read where it lies, and the heap still has about 4 MB more than
// on a retail console. (Capping the OS arena at 32 MB instead leaves the main
// heap 3.7 MB, because of the 24 MB mallocinit holds back; the game hangs.)
extern "C" {
void* OSGetArenaLo(void);
void* OSGetArenaHi(void);
void OSReport(const char*, ...);
int MEM_initadr(void*, int);

extern bool g_bMallocInitialized;

void mallocinit(void) {
    void* low = OSGetArenaLo();
    int size = (char*)OSGetArenaHi() - (char*)low;
    if (size > 0x1800000)
        size -= 0x1800000;
    if ((char*)low + size > (char*)0x82000000)
        size = (char*)0x82000000 - (char*)low;
    int result = MEM_initadr(low, size);
    g_bMallocInitialized = true;
    OSReport("\n");
    OSReport("\n");
    OSReport("MEM_init() return %d\n", result);
    OSReport("\n");
}
}
