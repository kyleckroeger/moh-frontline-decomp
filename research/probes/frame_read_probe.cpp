// settings: config/GMFE69/animemot.json
// replaces: AnimFileGetFrame
// Test harness (W1, pure image): AnimFileGetFrame with animemot.cpp's DMAFrame inline, then a check:
// invalidate the data cache over the frame and compare. Bytes that change were stale cache lines
// (the CPU read them, not the DMA'd data). Also reads the same ARAM range a second time.
#include "game/GameAddr.h"
#include <dolphin/os.h>
#include <dolphin/ar.h>
struct FrameFile_t {
    unsigned char field00[24];
    unsigned long* frames;
};
extern unsigned char g_frame_cache[0xc800];
extern int g_frame_cache_used;
extern volatile bool g_bDMAToMRAM;
extern ARQRequest g_fromARAM_request;
void DMAFromARAMCallback(unsigned long);
static int s_reports;
static unsigned char s_copy[0x800] __attribute__((aligned(32)));
extern "C" unsigned char* AnimFileGetFrame(FrameFile_t* file, int frame) {
    unsigned long src = file->frames[frame];
    int length = file->frames[frame + 1] - src;
    unsigned long aligned = src & ~31;
    int skip = src - aligned;
    length += skip;
    int size = (length + 31) & ~31;
    if (size > 0xc800 - g_frame_cache_used)
        g_frame_cache_used = 0;
    unsigned char* cache = g_frame_cache + g_frame_cache_used;
    OSGetTick();
    DCFlushRange(cache, size);
    g_bDMAToMRAM = true;
    ARQPostRequest(&g_fromARAM_request, 0, 1, 1, aligned, (GameAddr)cache, size, DMAFromARAMCallback);
    while (g_bDMAToMRAM) {
    }
    static int s_seq;
    if (s_seq < 300) {
        s_seq++;
        if (s_seq <= 300)
            OSReport("FRSEQ %d aram %08x size %d frame %d first %08x\n", s_seq, aligned, size, frame, *(unsigned long*)(cache + skip));
    }
    if (size <= (int)sizeof(s_copy) && s_reports < 30) {
        int i, before = 0;
        for (i = 0; i < size; i++)
            s_copy[i] = cache[i];
        DCInvalidateRange(cache, size);
        for (i = 0; i < size; i++)
            if (s_copy[i] != cache[i])
                before++;
        if (before) {
            s_reports++;
            OSReport("FRAMEREAD stale %d of %d at off %d aram %08x\n", before, size, (int)(cache - g_frame_cache), aligned);
        }
    }
    cache += skip;
    g_frame_cache_used += size;
    return cache;
}
