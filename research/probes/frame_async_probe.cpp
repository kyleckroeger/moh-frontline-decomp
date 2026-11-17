// settings: config/GMFE69/animemot.json
// replaces: AnimFileGetFrame
// Test harness (W1, pure image): does anything other than the frame read write the frame-cache slot?
// For the first reads: fill the slot with a marker (flushed), wait ~2 ms with no DMA and look again
// (an asynchronous writer such as the DSP would change it), then do the read as DMAFrame does,
// look right after the wait ends and again ~2 ms later (does the data land late?).
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
extern int g_frseq_on;
static void Spin(unsigned long ms) {
    unsigned long ticks = (*(unsigned long*)0x800000F8 / 4) / 1000 * ms;
    unsigned long start = OSGetTick();
    while (OSGetTick() - start < ticks) {
    }
}
static unsigned long Word(unsigned char* p) {
    DCInvalidateRange(p, 32);
    return *(unsigned long*)p;
}
static unsigned char s_direct[0x800] __attribute__((aligned(32)));
extern "C" unsigned char* AnimFileGetFrame(FrameFile_t* file, int frame) {
    static int s_seq, s_bad;
    unsigned long src = file->frames[frame];
    int length = file->frames[frame + 1] - src;
    unsigned long aligned = src & ~31;
    int skip = src - aligned;
    length += skip;
    int size = (length + 31) & ~31;
    if (size > 0xc800 - g_frame_cache_used)
        g_frame_cache_used = 0;
    unsigned char* cache = g_frame_cache + g_frame_cache_used;
    s_seq++;
    if (s_seq == 1)
        g_frseq_on = 1;
    if (s_seq <= 6)
        OSReport("ARSEQ post %d slot %08x aram %08x size %d tick %u\n", s_seq, (unsigned long)cache, aligned, size, OSGetTick());
    OSGetTick();
    DCFlushRange(cache, size);
    g_bDMAToMRAM = true;
    ARQPostRequest(&g_fromARAM_request, 0, 1, 1, aligned, (GameAddr)cache, size, DMAFromARAMCallback);
    while (g_bDMAToMRAM) {
    }
    if (s_seq <= 6)
    {
        volatile unsigned long* unc = (volatile unsigned long*)(((unsigned long)cache + skip) | 0x40000000);
        unsigned long u = *unc;
        unsigned long c = *(volatile unsigned long*)(cache + skip);
        DCInvalidateRange(cache, size);
        unsigned long ci = *(volatile unsigned long*)(cache + skip);
        OSReport("ARSEQ woke %d tick %u uncached %08x cached %08x after-inval %08x\n", s_seq, OSGetTick(), u, c, ci);
    }
    if (size <= (int)sizeof(s_direct) && s_bad < 40) {
        unsigned long now = *(unsigned long*)(cache + skip);
        DCInvalidateRange(s_direct, size);
        ARStartDMA(1, (unsigned long)s_direct, aligned, size);
        while (ARGetDMAStatus()) {
        }
        DCInvalidateRange(s_direct, size);
        int diff = 0, first = -1;
        for (int i = 0; i < size; i++)
            if (s_direct[i] != cache[i]) {
                if (first < 0)
                    first = i;
                diff++;
            }
        if (diff) {
            s_bad++;
            if (s_bad <= 3)
                for (int i = 0; i < size; i += 16)
                    OSReport("FRDUMP %d +%03x game %08x %08x %08x %08x direct %08x %08x %08x %08x\n", s_seq, i,
                             *(unsigned long*)(cache + i), *(unsigned long*)(cache + i + 4),
                             *(unsigned long*)(cache + i + 8), *(unsigned long*)(cache + i + 12),
                             *(unsigned long*)(s_direct + i), *(unsigned long*)(s_direct + i + 4),
                             *(unsigned long*)(s_direct + i + 8), *(unsigned long*)(s_direct + i + 12));
            Spin(2);
            OSReport("FRDIFF %d slot %08x aram %08x size %d frame %d diff %d first %d game %08x direct %08x later %08x\n",
                     s_seq, (unsigned long)cache, aligned, size, frame, diff, first, now,
                     *(unsigned long*)(s_direct + skip), Word(cache + (skip & ~31)));
        }
    }
    if ((s_seq & 1023) == 0)
        OSReport("FRCOUNT %d bad %d\n", s_seq, s_bad);
    cache += skip;
    g_frame_cache_used += size;
    return cache;
}
