/* A fragment of the sound driver (snddrv.c, 0x8016c5f8): filter
   installation (unsupported), memory allocation and freeing through the
   installed allocator or ARAM (sizes rounded up to 32 bytes), and
   SNDPLATFORM_download, which flushes the block and posts its transfer to
   ARAM through ARQ in a new DMA slot, returning the request number. snddrv
   and sndgs are named by their symbols; their views, the DMA slot layout and
   the slot and posting inline helpers are inferred.
   SNDPLATFORM_downloadcomplete after it is drafted in
   scratch/path/sdrv_dlc_wip.cpp. */
struct OSMutex {
    unsigned char unknown00[24];
};

/* inferred: an ARQ request block */
struct ARQRequest {
    unsigned char unknown00[32];
};

struct SNDFILTERDEF;

extern "C" {
extern char sndgs[];
void OSInitMutex(OSMutex*);
void OSLockMutex(OSMutex*);
void OSUnlockMutex(OSMutex*);
int OSDisableInterrupts(void);
int OSRestoreInterrupts(int);
void DCFlushRange(void*, unsigned int);
void ARQPostRequest(ARQRequest*, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int,
                    unsigned int, void (*)(unsigned long));
void ARQRemoveRequest(ARQRequest*);
}
void* SNDARAM_alloc(int);
void SNDARAM_free(unsigned int);
void SNDDRV_dmcallback(unsigned long);

/* inferred: a DMA slot (id -1 when free) */
struct SNDDRVDMA {
    int id;
    ARQRequest request;
    int priority;
    unsigned int source;
    unsigned int dest;
    unsigned int length;
    unsigned int time;
    unsigned char unknown38[8];
};

struct SNDDRVVIEW {
    unsigned char unknown0000[0x9864];
    int requests;
    unsigned char unknown9868[0x98a8 - 0x9868];
    SNDDRVDMA dma[64];
    OSMutex mutex;
    unsigned char unknowna8c0[0x260];
};

extern SNDDRVVIEW snddrv;

/* inferred: claims a free DMA slot for a request */
static inline int allocdma(int id) {
    int level = OSDisableInterrupts();
    int i;

    for (i = 0; i < 64; i++) {
        if (snddrv.dma[i].id == -1)
            break;
    }
    snddrv.dma[i].id = id;
    OSRestoreInterrupts(level);
    return i;
}

/* inferred: posts a transfer, in a new slot when slot is -1 */
static inline int postdma(int slot, int priority, unsigned int source, unsigned int dest, int length) {
    int level = OSDisableInterrupts();
    int id;

    if (slot == -1) {
        slot = allocdma(snddrv.requests);
        id = snddrv.requests;
        snddrv.dma[slot].priority = priority;
        snddrv.dma[slot].source = source;
        snddrv.dma[slot].dest = dest;
        snddrv.dma[slot].length = (length + 31) & ~31;
        snddrv.dma[slot].time = *(unsigned int*)(sndgs + 372);
    } else {
        id = snddrv.dma[slot].id;
    }
    ARQPostRequest(&snddrv.dma[slot].request, id, 0, priority, source, dest, (length + 31) & ~31, SNDDRV_dmcallback);
    OSRestoreInterrupts(level);
    return snddrv.requests++;
}

int SNDPLATFORM_filteradd(int, SNDFILTERDEF*) {
    return -15;
}

void* SNDPLATFORM_memalloc(int, int size) {
    size = (size + 31) & ~31;
    if (*(void* (**)(int))(sndgs + 184))
        return (*(void* (**)(int))(sndgs + 184))(size);
    return SNDARAM_alloc(size);
}

int SNDPLATFORM_memfree(int, unsigned int memory) {
    if (*(void (**)(unsigned int))(sndgs + 188)) {
        (*(void (**)(unsigned int))(sndgs + 188))(memory);
        return 0;
    }
    SNDARAM_free(memory);
    return 0;
}

int SNDPLATFORM_download(int, void* source, void* dest, int length) {
    DCFlushRange(source, length);
    return postdma(-1, 0, (unsigned int)source, (unsigned int)dest, length);
}
