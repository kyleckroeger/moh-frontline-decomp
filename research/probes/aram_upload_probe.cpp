// settings: config/GMFE69/animemot.json
// replaces: UploadFramesToARAM__FP16AnimFileFormat_tPUcPUc
// Test harness (W1, pure image): animemot.cpp's UploadFramesToARAM, reporting each upload's ARAM
// address, length, source and lead-in, to compare the pure image's ARAM layout with the original's.
#include "game/GameAddr.h"
#include <dolphin/os.h>
#include <dolphin/ar.h>
#include <dolphin/vi.h>
struct AnimFileFormat_t {
    int field00;
    int field04;
    unsigned short field08;
    unsigned short field0A;
    unsigned short numFrames;
    unsigned short field0E;
    void* states;
    void* animations;
    unsigned int* frames;
};
extern "C" unsigned long ARAM_alloc(void*, const char*, unsigned long, int);
extern void* g_ARAM_poolman;
extern ARQRequest g_toARAM_request;
extern volatile bool g_bDMAToARAM;
void DMAToARAMCallback(unsigned long);
void UploadFramesToARAM(AnimFileFormat_t* file, unsigned char* start, unsigned char* finish) {
    unsigned char* aligned = (unsigned char*)((unsigned long)start & ~31);
    unsigned long size = finish - start;
    int lead = start - aligned;
    unsigned long length = ((finish - start) + lead + 31) & ~31;
    unsigned long aram = ARAM_alloc(g_ARAM_poolman, "Animation", length, 0);
    OSReport("ARAMUP aram %08x len %d start %08x lead %d frames %d first %08x\n", aram, length,
             (unsigned long)start, lead, file->numFrames, *(unsigned long*)start);
    for (unsigned long c = 0; c < length; c += 0x40000) {
        unsigned long sum = 0, n = length - c < 0x40000 ? length - c : 0x40000;
        for (unsigned long k = 0; k < n; k += 4)
            sum = sum * 31 + *(unsigned long*)(aligned + c + k);
        OSReport("ARAMSUM %08x %08x\n", c, sum);
    }
    DCFlushRange(aligned, length);
    g_bDMAToARAM = true;
    ARQPostRequest(&g_toARAM_request, 0, 0, 1, (GameAddr)aligned, aram, length, DMAToARAMCallback);
    while (g_bDMAToARAM)
        VIWaitForRetrace();
    {
        static unsigned long back[8] __attribute__((aligned(32)));
        DCInvalidateRange(back, 32);
        ARStartDMA(1, (unsigned long)back, aram, 32);
        while (ARGetDMAStatus()) {
        }
        DCInvalidateRange(back, 32);
        OSReport("ARAMBACK %08x %08x %08x %08x src %08x %08x\n", back[0], back[1], back[2], back[3],
                 *(unsigned long*)aligned, *(unsigned long*)(aligned + 4));
    }
    for (int i = 0; i < file->numFrames; i++)
        file->frames[i] = aram + (file->frames[i] - (unsigned long)start) + lead;
    file->frames[file->numFrames] = aram + size + lead;
}
