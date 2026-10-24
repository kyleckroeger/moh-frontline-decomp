// A fragment of animemot.cpp (0x80063238): AnimFileGetFrame copies an
// animation frame from ARAM into the frame cache (restarting the cache when
// the 32-byte aligned block does not fit in its 0xc800 bytes), waiting for
// the transfer, and returns the frame's address in the cache. The globals,
// functions and AnimFileFormat_t are named by the symbols; the frame offset
// table at +24 (each frame ends where the next starts) is inferred, the
// transfer request is file-local in the original and declared extern here,
// and the tick read whose result is unused is kept as the code does. The
// transfer is an inferred inline helper taking the source offset and length:
// adding the skipped bytes to the length parameter before rounding gives the
// original's order of the additions, and its locals are declared in the order
// that gives the original's registers. The rest of the file is in other units.
typedef struct ARQRequest ARQRequest;
typedef void (*ARQCallback)(unsigned long);

extern "C" {
unsigned long OSGetTick(void);
void DCFlushRange(void*, unsigned long);
void ARQPostRequest(ARQRequest*, unsigned long, unsigned long, unsigned long, unsigned long, unsigned long,
                    unsigned long, ARQCallback);
}

void DMAFromARAMCallback(unsigned long);

struct AnimFileFormat_t {
    unsigned char field00[24];
    unsigned long* frames;
};

extern unsigned char g_frame_cache[0xc800];
extern int g_frame_cache_used;
extern volatile bool g_bDMAToMRAM;
extern ARQRequest g_fromARAM_request; // file-local in the original

static inline unsigned char* DMAFrame(unsigned long src, int length) {
    unsigned char* cache;
    int skip;
    unsigned long aligned;
    int size;
    aligned = src & ~31;
    skip = src - aligned;
    length += skip;
    size = (length + 31) & ~31;
    if (size > 0xc800 - g_frame_cache_used)
        g_frame_cache_used = 0;
    cache = g_frame_cache + g_frame_cache_used;
    OSGetTick();
    DCFlushRange(cache, size);
    g_bDMAToMRAM = true;
    ARQPostRequest(&g_fromARAM_request, 0, 1, 1, aligned, (unsigned long)cache, size, DMAFromARAMCallback);
    while (g_bDMAToMRAM) {
    }
    cache += skip;
    g_frame_cache_used += size;
    return cache;
}

extern "C" unsigned char* AnimFileGetFrame(AnimFileFormat_t* file, int frame) {
    unsigned long start = file->frames[frame];
    return DMAFrame(start, file->frames[frame + 1] - start);
}
