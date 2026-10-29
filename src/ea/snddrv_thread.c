/* The sound driver's packet buffer fill, mixing thread and platform output
   capabilities (a fragment of snddrv.c). SNDDRV_fillbufwithpackets copies
   1536 bytes per channel from the packet player's packets (silence when none
   is queued) into the channel's half buffer and posts each half to ARAM by
   DMA; the thread runs the sndgs hooks, mixes a 160-sample slice into the mix
   buffers and runs the 100 Hz server every other frame;
   SNDPLATFORM_outputcaps fills sndgs with the GameCube output parameters and
   the ARAM range. sndgs and snddrv are named by their symbols; the views of
   them, of the channel records and the DMA helpers are inferred. everyother
   is the file's frame counter (static in the original; declared here as an
   external). */
struct _AXVPB;
struct OSThreadQueue;
struct ARQRequest;

extern "C" {
extern char sndgs[];
int OSDisableInterrupts(void);
int OSRestoreInterrupts(int);
void DCFlushRange(void*, unsigned long);
void ARQPostRequest(ARQRequest*, unsigned long, unsigned long, unsigned long, unsigned long, unsigned long, unsigned long, void (*)(unsigned long));
void SNDI_memcpy(void*, const void*, int);
void OSSleepThread(OSThreadQueue*);
unsigned long ARGetBaseAddress(void);
extern void* MIXaudioslicefn;
}
void* SNDPKTPLAYI_get(int, int, int*, int*);
void SNDDRV_dmcallback(unsigned long);
void SNDPKTPLAYI_flushcallbackdata(void);
void SNDSYSI_100hzserver(void);
void SNDI_mutexlock(void);
void SNDI_mutexunlock(void);

/* inferred view of snddrv: 61 per-channel records, the silence buffer, the
   DMA request table, the mixer flags and the mix buffers */
struct SNDDRVVOICE {
    char field00[8];
    int field08;
    char field0C[3];
    unsigned char active;
    char field10[4];
    unsigned char field14;
    unsigned char field15;
    char field16[22];
    _AXVPB* axvoice;
};

struct ARQRequest {
    unsigned char unknown00[32];
};

struct SNDDRVDMA {
    int id;
    ARQRequest request;
    int active;
    void* source;
    void* dest;
    int length;
    int field34;
    char field38[8];
};

struct OSThreadQueue {
    void* head;
    void* tail;
};

struct SNDDRVVIEW {
    SNDDRVVOICE voices[61];
    char fieldb70[0x1540 - 61 * 48];
    void* silence;
    char field1544[0x1858 - 0x1544];
    OSThreadQueue threadqueue;
    char field1860[0x9860 - 0x1860];
    int lastdma;
    int dmacount;
    char field9868[0x98a8 - 0x9868];
    SNDDRVDMA dma[64];
    char fielda8a8[0xaac0 - 0xa8a8];
    signed char mixerinited;
    unsigned char activevoices;
    char fieldaac2[0xaac8 - 0xaac2];
    void* mixalloc[4];
    void* mixbuffer[3];
    char fieldaae4[0xab20 - 0xaae4];
};

extern SNDDRVVIEW snddrv;

extern int everyother;

/* inferred views: CHANPUB and SNDDRVPKTCHAN are named by the mangled symbol */
struct CHANPUB {
    char field00[31];
    unsigned char channels;
};

struct SNDDRVPKTCHAN {
    char field0000[4800];
    void* buffer[3];
    char field12CC;
    unsigned char half;
    char field12CE[2];
    char* aram[3];
    char field12DC[4];
    short played[4];
    char* packet[3];
    int size;
    int offset;
};

/* inferred inline helper */
static inline int allocdma(int id) {
    int enabled = OSDisableInterrupts();
    int i;

    for (i = 0; i < 64; i++) {
        if (snddrv.dma[i].id == -1)
            break;
    }
    snddrv.dma[i].id = id;
    OSRestoreInterrupts(enabled);
    return i;
}

/* inferred inline helper */
static inline void postdma(int j, unsigned long owner, char* source, char* dest, int length) {
    snddrv.dma[j].active = 1;
    snddrv.dma[j].source = source;
    snddrv.dma[j].dest = dest;
    snddrv.dma[j].length = length;
    snddrv.dma[j].field34 = *(int*)(sndgs + 372);
    ARQPostRequest(&snddrv.dma[j].request, owner, 0, 1, (unsigned long)source, (unsigned long)dest, length, SNDDRV_dmcallback);
}

/* inferred inline helper */
static inline void transfer(char* source, char* dest, int length) {
    int enabled = OSDisableInterrupts();
    int j = allocdma(snddrv.dmacount);

    postdma(j, snddrv.dmacount, source, dest, length);
    OSRestoreInterrupts(enabled);
    snddrv.dmacount++;
}

void SNDDRV_fillbufwithpackets(int handle, SNDDRVPKTCHAN* chan, CHANPUB* pub) {
    int count;
    int avail;
    int i;
    int rate;
    int remaining = 1536;
    int size;
    char* src[2];

    while (remaining > 0) {
        if (!chan->packet[0]) {
            for (i = 0; i < pub->channels; i++)
                chan->packet[i] = (char*)SNDPKTPLAYI_get(handle, i, &size, &rate);
            chan->offset = 0;
            chan->size = size * 2;
        }
        count = remaining;
        if (chan->packet[0]) {
            avail = chan->size - chan->offset;
            if (avail <= 0) {
                chan->packet[0] = 0;
                continue;
            }
            if (remaining > avail)
                count = avail;
            chan->played[chan->half] += count >> 1;
            for (i = 0; i < pub->channels; i++)
                src[i] = chan->packet[i] + chan->offset;
            chan->offset += count;
        } else {
            for (i = 0; i < pub->channels; i++)
                src[i] = (char*)snddrv.silence;
        }
        for (i = 0; i < pub->channels; i++)
            SNDI_memcpy((char*)chan->buffer[i] + (1536 - remaining), src[i], count);
        remaining -= count;
    }
    for (i = 0; i < pub->channels; i++) {
        DCFlushRange(chan->buffer[i], 1536);
        transfer((char*)chan->buffer[i], chan->aram[i] + chan->half * 1536, 1536);
    }
}

void SNDDRV_thread(void* param) {
    for (;;) {
        SNDI_mutexlock();
        if (*(void (**)(void))(sndgs + 552))
            (*(void (**)(void))(sndgs + 552))();
        if (snddrv.mixerinited) {
            ((void (*)(void**, int))MIXaudioslicefn)(snddrv.mixbuffer, 160);
            SNDPKTPLAYI_flushcallbackdata();
        }
        if (everyother & 1)
            SNDSYSI_100hzserver();
        if (*(void (**)(void))(sndgs + 556))
            (*(void (**)(void))(sndgs + 556))();
        SNDI_mutexunlock();
        OSSleepThread(&snddrv.threadqueue);
    }
}

/* sndgs fields by offset */
#define U8(o) (*(unsigned char*)(sndgs + (o)))
#define U16(o) (*(unsigned short*)(sndgs + (o)))
#define U32(o) (*(unsigned long*)(sndgs + (o)))

int SNDPLATFORM_outputcaps() {
    unsigned long base;

    U8(6) = 16;
    U8(11) = 61;
    U16(0) = 32000;
    U16(2) = 32000;
    U8(4) = 1;
    U8(5) = 3;
    U8(74) = 1;
    U8(19) = 2;
    U16(20) = 544;
    U16(22) = 36;
    U8(77) = 2;
    U16(80) = 544;
    U16(82) = 36;
    U8(46) = 8;
    U8(51) = 48;
    U8(71) = 8;
    U16(42) = 32000;
    U8(72) = 2;
    U8(70) = 1;
    base = ARGetBaseAddress() + 256;
    U32(92) = base;
    U32(96) = 0x800000 - base;
    U8(52) = 10;
    U16(120) = 0;
    U16(128) = 0x4000;
    U16(130) = 0xc000;
    U16(136) = 0x4000;
    U16(138) = 0x8000;
    U16(140) = 0xc000;
    return 0;
}
