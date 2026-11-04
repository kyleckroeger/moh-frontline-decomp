/* The sound driver's packet buffer fill, mixing thread and platform output
   capabilities and settings (a fragment of snddrv.c). SNDDRV_fillbufwithpackets copies
   1536 bytes per channel from the packet player's packets (silence when none
   is queued) into the channel's half buffer and posts each half to ARAM by
   DMA; the thread runs the sndgs hooks, mixes a 160-sample slice into the mix
   buffers and runs the 100 Hz server every other frame;
   SNDPLATFORM_outputcaps fills sndgs with the GameCube output parameters and
   the ARAM range; SNDPLATFORM_outputset clamps the requested output options
   against those limits and applies them. sndgs and snddrv are named by their symbols; the views of
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

/* inferred: an 8-byte member (two words) */
struct SNDPAIR {
    int a[2];
};

/* inferred: a 7-byte name */
struct SNDNAME7 {
    char c[7];
};

/* inferred: the output options: the options block's first 152 bytes */
struct SNDOUTOPTS {
    int field00;
    int field04;
    unsigned short field08;
    unsigned short field0a;
    unsigned short field0c;
    unsigned char field0e;
    unsigned char field0f;
    unsigned char field10;
    unsigned char field11;
    unsigned char field12;
    unsigned char field13;
    unsigned char field14;
    SNDNAME7 name;
    unsigned char field1c;
    unsigned char field1d;
    unsigned char field1e;
    unsigned char field1f;
    unsigned char field20;
    unsigned char field21;
    unsigned char field22;
    unsigned char field23;
    unsigned char field24;
    unsigned char field25;
    unsigned char field26;
    unsigned char field27;
    unsigned char field28;
    unsigned char field29;
    unsigned char field2a;
    unsigned char field2b;
    unsigned char field2c;
    unsigned char field2d;
    unsigned char field2e;
    SNDPAIR field30;
    int field38;
    int field3c;
    int field40;
    int field44;
    int field48;
    int field4c;
    int field50;
    int field54;
    SNDPAIR field58;
    SNDPAIR field60;
    SNDPAIR field68;
    SNDPAIR field70;
    SNDPAIR field78;
    SNDPAIR field80;
    SNDPAIR field88;
    SNDPAIR field90;
};

/* inferred: the global state as the output settings see it */
struct SNDPLATGSVIEW {
    unsigned char unknown00[4];
    unsigned char minfield28;
    unsigned char maxfield28;
    unsigned char maxfield0e;
    unsigned char unknown07;
    unsigned char unknown08[3];
    unsigned char maxfield13;
    unsigned char unknown0c[20];
    SNDOUTOPTS opts;
    unsigned char unknownb8[24];
    SNDOUTOPTS applied;
    signed char useapplied;
    unsigned char unknown169[7];
    unsigned short channels;
};

int SNDPLATFORM_outputset(void) {
    SNDPLATGSVIEW* gs = (SNDPLATGSVIEW*)sndgs;
    SNDOUTOPTS opts;

    opts.field00 = gs->opts.field00;
    opts.field04 = gs->opts.field04;
    opts.field08 = gs->opts.field08;
    opts.field0a = gs->opts.field0a;
    opts.field0c = gs->opts.field0c;
    opts.field0e = gs->opts.field0e;
    opts.field0f = gs->opts.field0f;
    opts.field10 = gs->opts.field10;
    opts.field11 = gs->opts.field11;
    opts.field12 = gs->opts.field12;
    opts.field13 = gs->opts.field13;
    opts.field14 = gs->opts.field14;
    opts.name = gs->opts.name;
    opts.field1c = ((SNDPLATGSVIEW*)sndgs)->opts.field1c;
    opts.field1d = ((SNDPLATGSVIEW*)sndgs)->opts.field1d;
    opts.field1e = ((SNDPLATGSVIEW*)sndgs)->opts.field1e;
    opts.field1f = ((SNDPLATGSVIEW*)sndgs)->opts.field1f;
    opts.field20 = ((SNDPLATGSVIEW*)sndgs)->opts.field20;
    opts.field21 = ((SNDPLATGSVIEW*)sndgs)->opts.field21;
    opts.field22 = ((SNDPLATGSVIEW*)sndgs)->opts.field22;
    opts.field23 = ((SNDPLATGSVIEW*)sndgs)->opts.field23;
    opts.field24 = ((SNDPLATGSVIEW*)sndgs)->opts.field24;
    opts.field25 = ((SNDPLATGSVIEW*)sndgs)->opts.field25;
    opts.field26 = ((SNDPLATGSVIEW*)sndgs)->opts.field26;
    opts.field27 = ((SNDPLATGSVIEW*)sndgs)->opts.field27;
    opts.field28 = ((SNDPLATGSVIEW*)sndgs)->opts.field28;
    opts.field29 = ((SNDPLATGSVIEW*)sndgs)->opts.field29;
    opts.field2a = ((SNDPLATGSVIEW*)sndgs)->opts.field2a;
    opts.field2b = ((SNDPLATGSVIEW*)sndgs)->opts.field2b;
    opts.field2c = ((SNDPLATGSVIEW*)sndgs)->opts.field2c;
    opts.field2d = ((SNDPLATGSVIEW*)sndgs)->opts.field2d;
    opts.field2e = ((SNDPLATGSVIEW*)sndgs)->opts.field2e;
    opts.field30 = ((SNDPLATGSVIEW*)sndgs)->opts.field30;
    opts.field38 = ((SNDPLATGSVIEW*)sndgs)->opts.field38;
    opts.field3c = ((SNDPLATGSVIEW*)sndgs)->opts.field3c;
    opts.field40 = ((SNDPLATGSVIEW*)sndgs)->opts.field40;
    opts.field44 = ((SNDPLATGSVIEW*)sndgs)->opts.field44;
    opts.field48 = ((SNDPLATGSVIEW*)sndgs)->opts.field48;
    opts.field4c = ((SNDPLATGSVIEW*)sndgs)->opts.field4c;
    opts.field50 = ((SNDPLATGSVIEW*)sndgs)->opts.field50;
    opts.field54 = ((SNDPLATGSVIEW*)sndgs)->opts.field54;
    opts.field58 = ((SNDPLATGSVIEW*)sndgs)->opts.field58;
    opts.field60 = ((SNDPLATGSVIEW*)sndgs)->opts.field60;
    opts.field68 = ((SNDPLATGSVIEW*)sndgs)->opts.field68;
    opts.field70 = ((SNDPLATGSVIEW*)sndgs)->opts.field70;
    opts.field78 = ((SNDPLATGSVIEW*)sndgs)->opts.field78;
    opts.field80 = ((SNDPLATGSVIEW*)sndgs)->opts.field80;
    opts.field88 = ((SNDPLATGSVIEW*)sndgs)->opts.field88;
    opts.field90 = ((SNDPLATGSVIEW*)sndgs)->opts.field90;
    if (((SNDPLATGSVIEW*)sndgs)->useapplied) {
        gs->opts.field00 = ((SNDPLATGSVIEW*)sndgs)->applied.field00;
        gs->opts.field04 = ((SNDPLATGSVIEW*)sndgs)->applied.field04;
        gs->opts.field08 = ((SNDPLATGSVIEW*)sndgs)->applied.field08;
        gs->opts.field0a = ((SNDPLATGSVIEW*)sndgs)->applied.field0a;
        gs->opts.field0c = ((SNDPLATGSVIEW*)sndgs)->applied.field0c;
        gs->opts.field0e = ((SNDPLATGSVIEW*)sndgs)->applied.field0e;
        gs->opts.field0f = ((SNDPLATGSVIEW*)sndgs)->applied.field0f;
        gs->opts.field10 = ((SNDPLATGSVIEW*)sndgs)->applied.field10;
        gs->opts.field11 = ((SNDPLATGSVIEW*)sndgs)->applied.field11;
        gs->opts.field12 = ((SNDPLATGSVIEW*)sndgs)->applied.field12;
        gs->opts.field13 = ((SNDPLATGSVIEW*)sndgs)->applied.field13;
        gs->opts.field14 = ((SNDPLATGSVIEW*)sndgs)->applied.field14;
        ((SNDPLATGSVIEW*)sndgs)->opts.name = ((SNDPLATGSVIEW*)sndgs)->applied.name;
        ((SNDPLATGSVIEW*)sndgs)->opts.field1c = ((SNDPLATGSVIEW*)sndgs)->applied.field1c;
        ((SNDPLATGSVIEW*)sndgs)->opts.field1d = ((SNDPLATGSVIEW*)sndgs)->applied.field1d;
        ((SNDPLATGSVIEW*)sndgs)->opts.field1e = ((SNDPLATGSVIEW*)sndgs)->applied.field1e;
        ((SNDPLATGSVIEW*)sndgs)->opts.field1f = ((SNDPLATGSVIEW*)sndgs)->applied.field1f;
        ((SNDPLATGSVIEW*)sndgs)->opts.field20 = ((SNDPLATGSVIEW*)sndgs)->applied.field20;
        ((SNDPLATGSVIEW*)sndgs)->opts.field21 = ((SNDPLATGSVIEW*)sndgs)->applied.field21;
        ((SNDPLATGSVIEW*)sndgs)->opts.field22 = ((SNDPLATGSVIEW*)sndgs)->applied.field22;
        ((SNDPLATGSVIEW*)sndgs)->opts.field23 = ((SNDPLATGSVIEW*)sndgs)->applied.field23;
        ((SNDPLATGSVIEW*)sndgs)->opts.field24 = ((SNDPLATGSVIEW*)sndgs)->applied.field24;
        ((SNDPLATGSVIEW*)sndgs)->opts.field25 = ((SNDPLATGSVIEW*)sndgs)->applied.field25;
        ((SNDPLATGSVIEW*)sndgs)->opts.field26 = ((SNDPLATGSVIEW*)sndgs)->applied.field26;
        ((SNDPLATGSVIEW*)sndgs)->opts.field27 = ((SNDPLATGSVIEW*)sndgs)->applied.field27;
        ((SNDPLATGSVIEW*)sndgs)->opts.field28 = ((SNDPLATGSVIEW*)sndgs)->applied.field28;
        ((SNDPLATGSVIEW*)sndgs)->opts.field29 = ((SNDPLATGSVIEW*)sndgs)->applied.field29;
        ((SNDPLATGSVIEW*)sndgs)->opts.field2a = ((SNDPLATGSVIEW*)sndgs)->applied.field2a;
        ((SNDPLATGSVIEW*)sndgs)->opts.field2b = ((SNDPLATGSVIEW*)sndgs)->applied.field2b;
        ((SNDPLATGSVIEW*)sndgs)->opts.field2c = ((SNDPLATGSVIEW*)sndgs)->applied.field2c;
        ((SNDPLATGSVIEW*)sndgs)->opts.field2d = ((SNDPLATGSVIEW*)sndgs)->applied.field2d;
        ((SNDPLATGSVIEW*)sndgs)->opts.field2e = ((SNDPLATGSVIEW*)sndgs)->applied.field2e;
        ((SNDPLATGSVIEW*)sndgs)->opts.field30 = ((SNDPLATGSVIEW*)sndgs)->applied.field30;
        ((SNDPLATGSVIEW*)sndgs)->opts.field38 = ((SNDPLATGSVIEW*)sndgs)->applied.field38;
        ((SNDPLATGSVIEW*)sndgs)->opts.field3c = ((SNDPLATGSVIEW*)sndgs)->applied.field3c;
        ((SNDPLATGSVIEW*)sndgs)->opts.field40 = ((SNDPLATGSVIEW*)sndgs)->applied.field40;
        ((SNDPLATGSVIEW*)sndgs)->opts.field44 = ((SNDPLATGSVIEW*)sndgs)->applied.field44;
        ((SNDPLATGSVIEW*)sndgs)->opts.field48 = ((SNDPLATGSVIEW*)sndgs)->applied.field48;
        ((SNDPLATGSVIEW*)sndgs)->opts.field4c = ((SNDPLATGSVIEW*)sndgs)->applied.field4c;
        ((SNDPLATGSVIEW*)sndgs)->opts.field50 = ((SNDPLATGSVIEW*)sndgs)->applied.field50;
        ((SNDPLATGSVIEW*)sndgs)->opts.field54 = ((SNDPLATGSVIEW*)sndgs)->applied.field54;
        ((SNDPLATGSVIEW*)sndgs)->opts.field58 = ((SNDPLATGSVIEW*)sndgs)->applied.field58;
        ((SNDPLATGSVIEW*)sndgs)->opts.field60 = ((SNDPLATGSVIEW*)sndgs)->applied.field60;
        ((SNDPLATGSVIEW*)sndgs)->opts.field68 = ((SNDPLATGSVIEW*)sndgs)->applied.field68;
        ((SNDPLATGSVIEW*)sndgs)->opts.field70 = ((SNDPLATGSVIEW*)sndgs)->applied.field70;
        ((SNDPLATGSVIEW*)sndgs)->opts.field78 = ((SNDPLATGSVIEW*)sndgs)->applied.field78;
        ((SNDPLATGSVIEW*)sndgs)->opts.field80 = ((SNDPLATGSVIEW*)sndgs)->applied.field80;
        ((SNDPLATGSVIEW*)sndgs)->opts.field88 = ((SNDPLATGSVIEW*)sndgs)->applied.field88;
        ((SNDPLATGSVIEW*)sndgs)->opts.field90 = ((SNDPLATGSVIEW*)sndgs)->applied.field90;
    }
    opts.field0a = 32000;
    if (opts.field28 < ((SNDPLATGSVIEW*)sndgs)->minfield28)
        opts.field28 = ((SNDPLATGSVIEW*)sndgs)->minfield28;
    if (opts.field28 > ((SNDPLATGSVIEW*)sndgs)->maxfield28)
        opts.field28 = ((SNDPLATGSVIEW*)sndgs)->maxfield28;
    if (opts.field0e > ((SNDPLATGSVIEW*)sndgs)->maxfield0e)
        opts.field0e = ((SNDPLATGSVIEW*)sndgs)->maxfield0e;
    if (opts.field13 > ((SNDPLATGSVIEW*)sndgs)->maxfield13)
        opts.field13 = ((SNDPLATGSVIEW*)sndgs)->maxfield13;
    if (opts.field27 > 32)
        opts.field27 = 32;
    {
        int rate = gs->opts.field14;
        if (rate < 1)
            rate = 1;
        else if (rate > 200)
            rate = 200;
        gs->opts.field14 = rate;
    }
    if (((SNDPLATGSVIEW*)sndgs)->useapplied)
        return 0;
    int const a = opts.field0e;
    int const b = opts.field13;
    ((SNDPLATGSVIEW*)sndgs)->channels = a + b;
    gs->opts.field00 = opts.field00;
    gs->opts.field04 = opts.field04;
    gs->opts.field08 = opts.field08;
    gs->opts.field0a = opts.field0a;
    gs->opts.field0c = opts.field0c;
    gs->opts.field0e = a;
    gs->opts.field0f = opts.field0f;
    gs->opts.field10 = opts.field10;
    gs->opts.field11 = opts.field11;
    gs->opts.field12 = opts.field12;
    gs->opts.field13 = b;
    gs->opts.field14 = opts.field14;
    ((SNDPLATGSVIEW*)sndgs)->opts.name = opts.name;
    ((SNDPLATGSVIEW*)sndgs)->opts.field1c = opts.field1c;
    ((SNDPLATGSVIEW*)sndgs)->opts.field1d = opts.field1d;
    ((SNDPLATGSVIEW*)sndgs)->opts.field1e = opts.field1e;
    ((SNDPLATGSVIEW*)sndgs)->opts.field1f = opts.field1f;
    ((SNDPLATGSVIEW*)sndgs)->opts.field20 = opts.field20;
    ((SNDPLATGSVIEW*)sndgs)->opts.field21 = opts.field21;
    ((SNDPLATGSVIEW*)sndgs)->opts.field22 = opts.field22;
    ((SNDPLATGSVIEW*)sndgs)->opts.field23 = opts.field23;
    ((SNDPLATGSVIEW*)sndgs)->opts.field24 = opts.field24;
    ((SNDPLATGSVIEW*)sndgs)->opts.field25 = opts.field25;
    ((SNDPLATGSVIEW*)sndgs)->opts.field26 = opts.field26;
    ((SNDPLATGSVIEW*)sndgs)->opts.field27 = opts.field27;
    ((SNDPLATGSVIEW*)sndgs)->opts.field28 = opts.field28;
    ((SNDPLATGSVIEW*)sndgs)->opts.field29 = opts.field29;
    ((SNDPLATGSVIEW*)sndgs)->opts.field2a = opts.field2a;
    ((SNDPLATGSVIEW*)sndgs)->opts.field2b = opts.field2b;
    ((SNDPLATGSVIEW*)sndgs)->opts.field2c = opts.field2c;
    ((SNDPLATGSVIEW*)sndgs)->opts.field2d = opts.field2d;
    ((SNDPLATGSVIEW*)sndgs)->opts.field2e = opts.field2e;
    ((SNDPLATGSVIEW*)sndgs)->opts.field30 = opts.field30;
    ((SNDPLATGSVIEW*)sndgs)->opts.field38 = opts.field38;
    ((SNDPLATGSVIEW*)sndgs)->opts.field3c = opts.field3c;
    ((SNDPLATGSVIEW*)sndgs)->opts.field40 = opts.field40;
    ((SNDPLATGSVIEW*)sndgs)->opts.field44 = opts.field44;
    ((SNDPLATGSVIEW*)sndgs)->opts.field48 = opts.field48;
    ((SNDPLATGSVIEW*)sndgs)->opts.field4c = opts.field4c;
    ((SNDPLATGSVIEW*)sndgs)->opts.field50 = opts.field50;
    ((SNDPLATGSVIEW*)sndgs)->opts.field54 = opts.field54;
    ((SNDPLATGSVIEW*)sndgs)->opts.field58 = opts.field58;
    ((SNDPLATGSVIEW*)sndgs)->opts.field60 = opts.field60;
    ((SNDPLATGSVIEW*)sndgs)->opts.field68 = opts.field68;
    ((SNDPLATGSVIEW*)sndgs)->opts.field70 = opts.field70;
    ((SNDPLATGSVIEW*)sndgs)->opts.field78 = opts.field78;
    ((SNDPLATGSVIEW*)sndgs)->opts.field80 = opts.field80;
    ((SNDPLATGSVIEW*)sndgs)->opts.field88 = opts.field88;
    ((SNDPLATGSVIEW*)sndgs)->opts.field90 = opts.field90;
    return 0;
}
