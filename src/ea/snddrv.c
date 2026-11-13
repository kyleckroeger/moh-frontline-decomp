// snddrv.c: EA's GameCube sound driver (compiled as C++). sndgs and snddrv
// are named by their symbols; the views of them, of the channel, packet
// player and DMA records and of the AX voice are inferred. Where the
// functions read one slot of a record in two ways, the views keep both names
// in an anonymous union; sndgs is read through byte offsets except where a
// function reads its view (SNDGS). everyother is the file's frame counter.
// The file is compiled without automatic inlining (-inline on): with
// -inline auto one unit inlines SNDPLATFORM_setvol, the pitch and position
// setters and the stop path into their callers, which the original calls.
struct OSMutex {
    unsigned char unknown00[24];
};

struct OSThread {
    unsigned char unknown000[784];
};

struct OSThreadQueue {
    void* head;
    void* tail;
};

struct ARQRequest {
    ARQRequest* next;
    unsigned long owner;
    unsigned long type;
    unsigned long priority;
    unsigned long source;
    unsigned long dest;
    unsigned long length;
    void (*callback)(unsigned long);
};

/* inferred: the AX voice fields the driver reads: the user index (the
   driver voice), the state and the current address */
struct _AXVPB {
    unsigned char unknown000[20];
    int voice;
    unsigned char unknown018[302];
    unsigned short state;
    unsigned char unknown148[106];
    unsigned short currentHi;
    unsigned short currentLo;
};
typedef struct _AXVPB AXVPB;

extern "C" {
extern char sndgs[];
}

struct CHANPUB;
struct SNDIPATCHHEADER;
struct SNDSAMPLEFORMAT;
struct SNDFILTERDEF;

/* inferred: the gain fields of a voice record */
struct SNDVOICEFXVIEW {
    unsigned char unknown48[11];
    signed char gain;
    unsigned char unknown54[9];
    signed char field5D;
    unsigned char unknown5e[32];
};

/* inferred: a sound voice record of sndgs's voice table (128 bytes). The
   channel numbers are indexed up to the channel count (at most 13), over
   the fields that follow the first six. */
struct SNDVOICEVIEW {
    unsigned char unknown00[4];
    short channels[6];
    union {
        int value10;
        unsigned int length;
    };
    unsigned char unknown14[4];
    unsigned short azimuth;
    unsigned char unknown1a[2];
    unsigned short rate;
    union {
        unsigned char format;
        unsigned char type;
    };
    union {
        unsigned char channelCount;
        unsigned char count;
    };
    unsigned short flags;
    unsigned char unknown22[2];
    short master;
    unsigned char priority;
    unsigned char unknown27[30];
    signed char volume;
    unsigned char unknown46[2];
    union {
        unsigned short azimuths[27];
        SNDVOICEFXVIEW fx;
    };
    unsigned short pitch;
};

/* inferred: the sndgs fields the driver reads by name (others are read
   through byte offsets into sndgs) */
struct SNDGSVIEW {
    unsigned char unknown000[42];
    unsigned short sampleRate;
    unsigned char unknown02c[7];
    union {
        unsigned char channelBase;
        unsigned char firstVoice;
    };
    unsigned char unknown034[19];
    unsigned char packetCount;
    unsigned char outputs;
    unsigned char unknown049[295];
    short voiceCount;
    unsigned char unknown172[2];
    int ticks;
    unsigned char unknown178[92];
    SNDVOICEVIEW* voices;
};
#define SNDGS (*(SNDGSVIEW*)sndgs)

/* inferred: a driver channel record (48 bytes): its start offset, packet
   player, flags and AX voice */
struct SNDDRVVOICE {
    char field00[8];
    union {
        int field08;
        int start;
        int offset;
    };
    union {
        signed char field0C;
        signed char packet;
        unsigned char player;
    };
    char field0D[2];
    unsigned char active;
    char field10[4];
    union {
        unsigned char field14;
        unsigned char allocated;
        volatile unsigned char vallocated;
    };
    unsigned char field15;
    char field16[22];
    _AXVPB* axvoice;
};

/* inferred: the same record as output levels (3D position and volume) */
struct SNDDRVCHANNELVIEW {
    short levels[22];
    _AXVPB* voice;
};

/* inferred: the packet player's platform record: the sample buffer, its
   32-byte aligned staging areas, the voice, half and start delay, the DSP
   buffers in ARAM, the frames played per half and the current packets */
struct SNDDRVPKTCHAN {
    unsigned char buffer[4800];
    unsigned char* staging[3];
    union {
        signed char state;
        signed char voice;
    };
    unsigned char half;
    signed char delay;
    unsigned char unknown12cf;
    union {
        unsigned int dsp[3];
        void* dspmem[3];
        char* aram[3];
    };
    unsigned int middle;
    union {
        short frames[2];
        short played[4];
    };
    union {
        int value12e8;
        char* packet[3];
    };
    int size;
    int offset;
};

/* inferred: a DMA request record (64 bytes) */
struct SNDDRVDMA {
    union {
        int id;
        volatile int vid;
    };
    ARQRequest request;
    union {
        int active;
        int mode;
        int priority;
        int pending;
    };
    union {
        void* source;
        unsigned int sourceaddr;
    };
    union {
        void* dest;
        unsigned int destination;
    };
    int length;
    union {
        int field34;
        int extra;
        unsigned int time;
    };
    char field38[8];
};

/* inferred: the driver record: 77 channel records, the ARAM layout, the
   packet players, the silence buffer, the driver thread with its queue and
   stack, the DMA request table, the mutex, the mixer flags and the mix
   buffers */
struct SNDDRVVIEW {
    union {
        SNDDRVVOICE voices[77];
        SNDDRVCHANNELVIEW channels[77];
    };
    unsigned long aram;
    union {
        unsigned long aramhalf;
        unsigned int silence;
    };
    union {
        unsigned long aramdouble;
        unsigned int silenceAdpcm;
    };
    void (*reset)(int);
    unsigned char unknown0e80[4];
    SNDDRVPKTCHAN* packets[8];
    unsigned char unknown0ea4[0xf20 - 0xea4];
    unsigned char buffer[0x1540 - 0xf20];
    union {
        unsigned char* alignedbuffer;
        void* silencebuffer;
    };
    unsigned char unknown1544[4];
    OSThread thread;
    OSThreadQueue threadqueue;
    unsigned char stack[0x8000];
    int lastdma;
    union {
        int nextdma;
        int dmacount;
        int requests;
        int dmaCounter;
    };
    unsigned char unknown9868[0x98a8 - 0x9868];
    SNDDRVDMA dma[64];
    OSMutex mutex;
    unsigned char unknowna8c0[0xaac0 - 0xa8c0];
    signed char mixerinited;
    unsigned char activevoices;
    unsigned char unknownaac2[6];
    void* mixalloc[4];
    void* mixbuffer[3];
    unsigned char unknownaae4[0xab20 - 0xaae4];
};

SNDDRVVIEW snddrv;

void SNDDRV_freenextframe();
void SNDDRV_mixvoicefree(int voice);
void SNDDRV_callbackdropvoice(void* data);
int SNDDRV_allocateaxvoice(int voice);
void SNDDRV_freeaxvoice(int channel, int mode);
void SNDDRV_dmcallback(unsigned long request);
void SNDDRV_audiocallback(void* buffers, void* context);
void SNDDRV_initmixer();
void SNDDRV_fillbufwithpackets(int handle, SNDDRVPKTCHAN* chan, CHANPUB* pub);
void SNDDRV_thread(void* param);
int SNDPLATFORM_outputset(void);
int SNDPLATFORM_init(void);
int SNDPLATFORM_restore(void);
void iSNDserve();
void SNDPLATFORM_getvoicerange(int mode, int* low, int* high);
int SNDPLATFORM_asyncresolvetimbre(SNDIPATCHHEADER* header, char* buffer, int* offset);
int SNDPLATFORM_playtimbre(SNDIPATCHHEADER* header, void* data, int index, int time, int unused, int lowpass, int highpass);
int SNDDRV_getmastervoice(int voice);
int SNDDRV_getsamplechan(int voice);
int SNDPLATFORM_packetoverhead(void);
int SNDPLATFORM_packetplaycreate(int index, void* memory);
int SNDPLATFORM_packetplay(int player, int index, int timemult, int, int lowpass, int highpass, SNDSAMPLEFORMAT* format, void** data);
int SNDPLATFORM_packetplaydestroy(int index);
int SNDPLATFORM_outputlatency(void);
void SNDPLATFORM_setvol(int index);
void SNDPLATFORM_setdrylevel(int voice);
void SNDPLATFORM_set3dpos(int index);
int SNDPLATFORM_setpitch(int index);
int SNDPLATFORM_timemult(int voice, int value);
void SNDPLATFORM_lowpass(int voice, int value);
void SNDPLATFORM_highpass(int voice, int value);
int SNDPLATFORM_getcurframe(int voice);
int SNDPLATFORM_stop(int voice);
int SNDPLATFORM_filteradd(int, SNDFILTERDEF*);
void* SNDPLATFORM_memalloc(int, int size);
int SNDPLATFORM_memfree(int, unsigned int memory);
int SNDPLATFORM_download(int, void* source, void* dest, int length);
int SNDPLATFORM_downloadcomplete(int id);
void SNDI_mutexalloc();
void SNDI_mutexfree();
void SNDI_mutexlock();
void SNDI_mutexunlock();

// Freeing the AX voices queued for release at the next frame, and the software
// mixer's voice-free hook. sndgs and snddrv are named by their symbols; the
// views of them are inferred from offsets.
extern "C" {
void AXFreeVoice(_AXVPB*);
extern char sndgs[];
}
void SNDVOICEI_free(int);

void SNDDRV_freenextframe() {
    int i;

    if (snddrv.activevoices) {
        for (i = 0; i < 61; i++) {
            if (snddrv.voices[i].active) {
                AXFreeVoice(snddrv.voices[i].axvoice);
                snddrv.voices[i].active = 0;
                snddrv.activevoices--;
                snddrv.voices[i].axvoice = 0;
                if (!snddrv.activevoices)
                    return;
            }
        }
    }
}

void SNDDRV_mixvoicefree(int voice) {
    SNDVOICEI_free(voice + *(unsigned char*)(sndgs + 51));
}

// The AX callback for a dropped voice releases the AX voices behind every
// channel of its sound voice (or the master's), the dropped one with reason 1
// and the others with reason 2. sndgs and snddrv are named by their symbols;
// their views, the voice records and the AX block's user index are inferred.
extern "C" char sndgs[];

void SNDDRV_callbackdropvoice(void* data) {
    _AXVPB* dropped = (_AXVPB*)data;
    int index = dropped->voice;
    SNDVOICEVIEW* voices = *(SNDVOICEVIEW**)(sndgs + 468);
    int master = voices[index].master;
    SNDVOICEVIEW* sound;
    int i;

    if (master == -1)
        master = index;
    sound = &voices[master];
    for (i = 0; i < sound->count; i++) {
        if (snddrv.voices[sound->channels[i]].axvoice) {
            if (sound->channels[i] == dropped->voice)
                SNDDRV_freeaxvoice(sound->channels[i], 1);
            else
                SNDDRV_freeaxvoice(sound->channels[i], 2);
            snddrv.voices[sound->channels[i]].allocated = 0;
        }
    }
}

// The sound driver's AX voice management and mixer callbacks: acquiring and
// releasing the AX voices behind a sound voice's channels, the DMA completion
// callback, the audio frame callback that feeds the software mixer's output to
// AX, and mixer creation. sndgs and snddrv are named by their symbols; the
// views of them are inferred from offsets.
extern "C" {
void AXFreeVoice(_AXVPB*);
_AXVPB* AXAcquireVoice(unsigned long, void (*)(void*), unsigned long);
extern char sndgs[];
int OSEnableInterrupts(void);
int OSRestoreInterrupts(int);
void OSWakeupThread(OSThreadQueue*);
float* MIX_getwetbuffer(void);
void SNDI_memcpy(void*, const void*, int);
void SNDSYS_linkmaincpumixer(void);
void SNDSYS_entercritical(void);
void SNDSYS_leavecritical(void);
extern void* MIXinitfn;
}
void SNDVOICEI_free(int);
void* SNDMEMI_allocz(int);
void SNDDRV_DSPMixerInitChannel(_AXVPB*, unsigned long, int, unsigned short, unsigned short);
void SNDDRV_DSPMixerReleaseChannel(_AXVPB*);

int SNDDRV_allocateaxvoice(int voice) {
    int i;
    SNDVOICEVIEW* entry = &(*(SNDVOICEVIEW**)(sndgs + 468))[voice];

    for (i = 0; i < entry->channelCount; i++) {
        int priority;

        if (entry->priority == 101)
            priority = 31;
        else
            priority = entry->priority / 3;
        if (priority <= 0)
            priority = 1;
        else if (priority > 31)
            priority = 30;
        if ((snddrv.voices[entry->channels[i]].axvoice = AXAcquireVoice(priority, SNDDRV_callbackdropvoice, entry->channels[i])) != 0) {
            snddrv.voices[entry->channels[i]].field15 = 0;
            SNDDRV_DSPMixerInitChannel(snddrv.voices[entry->channels[i]].axvoice, 0, 0, 0, 0);
        } else {
            snddrv.voices[entry->channels[i]].field15 = 1;
            if (i > 0) {
                for (; i >= 0; i--) {
                    if (snddrv.voices[entry->channels[i]].axvoice) {
                        AXFreeVoice(snddrv.voices[entry->channels[i]].axvoice);
                        snddrv.voices[entry->channels[i]].field15 = 1;
                        snddrv.voices[entry->channels[i]].axvoice = 0;
                    }
                    if ((*(SNDVOICEVIEW**)(sndgs + 468))[entry->channels[i]].fx.field5D)
                        SNDVOICEI_free(entry->channels[i]);
                }
            }
            return 0;
        }
    }
    return 1;
}

void SNDDRV_freeaxvoice(int channel, int mode) {
    SNDVOICEVIEW* entry = &(*(SNDVOICEVIEW**)(sndgs + 468))[channel];
    SNDDRVVOICE* drv = &snddrv.voices[channel];
    _AXVPB* axvoice = drv->axvoice;

    SNDDRV_DSPMixerReleaseChannel(axvoice);
    drv->field08 = -1;
    if (entry->fx.field5D)
        SNDVOICEI_free(channel);
    if (mode == 0) {
        if (axvoice) {
            AXFreeVoice(axvoice);
            drv->axvoice = 0;
        }
    } else if (mode == 2) {
        if (!drv->field15 && axvoice) {
            drv->active = 1;
            snddrv.activevoices++;
        }
    } else if (mode == 1) {
        if (drv->active == 1) {
            drv->active = 0;
            snddrv.activevoices--;
        }
        drv->axvoice = 0;
    }
    if (!drv->field15)
        drv->field15 = 1;
}

struct SNDDMAREQVIEW {
    char field0[4];
    int id;
};

void SNDDRV_dmcallback(unsigned long request) {
    int i;

    snddrv.lastdma = ((SNDDMAREQVIEW*)request)->id;
    for (i = 0; i < 64; i++) {
        if (snddrv.dma[i].id == ((SNDDMAREQVIEW*)request)->id) {
            snddrv.dma[i].id = -1;
            return;
        }
    }
}

static int everyother;

void SNDDRV_audiocallback(void* buffers, void* context) {
    int enabled = OSEnableInterrupts();

    if (snddrv.mixerinited) {
        static float* ptr;
        float* dst;
        int* src;
        int i;

        dst = ptr = MIX_getwetbuffer();
        src = ((int**)buffers)[0];
        for (i = 0; i < 160; i++)
            *dst++ = (float)*src++;
        SNDI_memcpy(((void**)buffers)[1], snddrv.mixbuffer[0], 640);
        if (*(unsigned char*)(sndgs + 72) < 3) {
            SNDI_memcpy(((void**)buffers)[0], snddrv.mixbuffer[1], 640);
        } else {
            SNDI_memcpy(((void**)buffers)[0], snddrv.mixbuffer[2], 640);
            SNDI_memcpy(((void**)buffers)[2], snddrv.mixbuffer[1], 640);
        }
    }
    OSWakeupThread(&snddrv.threadqueue);
    everyother++;
    OSRestoreInterrupts(enabled);
}

/* inferred: the mixer's creation parameters */
struct MIXINITVIEW {
    int rate;
    unsigned char field04;
    unsigned char channels;
    unsigned char field06;
    void (*voicefree)(int);
};

void SNDDRV_initmixer() {
    MIXINITVIEW params;
    int i;

    SNDSYS_linkmaincpumixer();
    params.field04 = *(unsigned char*)(sndgs + 46);
    if (*(unsigned char*)(sndgs + 72) != 1)
        params.channels = *(unsigned char*)(sndgs + 72);
    else
        params.channels = 2;
    params.rate = *(unsigned short*)(sndgs + 42);
    params.voicefree = SNDDRV_mixvoicefree;
    params.field06 = 0;
    ((void (*)(MIXINITVIEW*))MIXinitfn)(&params);
    for (i = 0; i < params.channels; i++) {
        SNDSYS_entercritical();
        snddrv.mixalloc[i] = SNDMEMI_allocz(672);
        snddrv.mixbuffer[i] = (void*)(((unsigned long)snddrv.mixalloc[i] + 31) & ~31);
        SNDSYS_leavecritical();
    }
    snddrv.mixerinited = 1;
}

// The sound driver's packet buffer fill, mixing thread and platform output
// capabilities and settings. SNDDRV_fillbufwithpackets copies 1536 bytes per
// channel from the packet player's packets (silence when none is queued) into
// the channel's half buffer and posts each half to ARAM by DMA; the thread runs
// the sndgs hooks, mixes a 160-sample slice into the mix buffers and runs the
// 100 Hz server every other frame; SNDPLATFORM_outputcaps fills sndgs with the
// GameCube output parameters and the ARAM range; SNDPLATFORM_outputset clamps
// the requested output options against those limits and applies them. sndgs and
// snddrv are named by their symbols; the views of them, of the channel records
// and the DMA helpers are inferred. everyother is the file's frame counter.
struct _AXVPB;

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
void SNDPKTPLAYI_flushcallbackdata(void);
void SNDSYSI_100hzserver(void);

/* inferred views: CHANPUB and SNDDRVPKTCHAN are named by the mangled symbol */
struct CHANPUB {
    char field00[31];
    unsigned char channels;
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
                src[i] = (char*)snddrv.silencebuffer;
        }
        for (i = 0; i < pub->channels; i++)
            SNDI_memcpy((char*)chan->staging[i] + (1536 - remaining), src[i], count);
        remaining -= count;
    }
    for (i = 0; i < pub->channels; i++) {
        DCFlushRange(chan->staging[i], 1536);
        transfer((char*)chan->staging[i], chan->aram[i] + chan->half * 1536, 1536);
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

// SNDPLATFORM_init sets up the platform layer (DSP mixer, ARAM and the ARAM
// request queue once, the DMA slot table, the ARAM pool, an aligned transfer
// buffer posted to ARAM twice through inline DMA helpers, AI/AX, the voice
// records, the software mixer when a voice type needs it, the driver thread,
// the AUX A callback and the 100 Hz clients). SNDPLATFORM_restore undoes it -
// the installed reset hook is called, the AUX A callback removed, the software
// mixer's buffers freed and the mixer restored (when it was set up), ARAM
// restored, pending critical sections drained, the driver thread cancelled, the
// 100 Hz clients removed and AX shut down. sndgs and snddrv are named by their
// symbols; their views are inferred.
extern "C" {
extern char sndgs[];
int OSDisableInterrupts(void);
int OSRestoreInterrupts(int);
void ARInit(unsigned long*, unsigned long);
void ARQInit(void);
unsigned long ARGetBaseAddress(void);
void ARQPostRequest(ARQRequest*, unsigned long, unsigned long, unsigned long, unsigned long, unsigned long, unsigned long,
                    void (*)(unsigned long));
void AIInit(unsigned char*);
void AXInit(void);
void AXSetMode(unsigned long);
int OSCreateThread(OSThread*, void* (*)(void*), void*, void*, unsigned long, long, unsigned short);
void OSInitThreadQueue(OSThreadQueue*);
int OSResumeThread(OSThread*);
void SNDSYS_add100hzclient(void (*)(void));
extern void (*MIXrestorefn)(void);
void AXRegisterAuxACallback(void*, void*);
void SNDSYS_entercritical(void);
void SNDSYS_leavecritical(void);
void OSCancelThread(OSThread*);
void SNDSYS_remove100hzclient(void (*)(void));
void AXQuit(void);
}
void SNDMEMI_free(void*);
void SNDARAM_restore(void);
void SNDDRV_DSPMixerUpdate(void);
void SNDDRV_DSPMixerInit(int);
void SNDARAM_init(int);
void SNDARAM_setpool(unsigned int, int);

/* inferred: queues an ARAM transfer */
static inline void postrestoredma(void* source, void* dest, int length) {
    int level = OSDisableInterrupts();
    int i = allocdma(snddrv.nextdma);
    int id = snddrv.nextdma;

    snddrv.dma[i].mode = 1;
    snddrv.dma[i].source = source;
    snddrv.dma[i].dest = dest;
    snddrv.dma[i].length = length;
    snddrv.dma[i].extra = *(int*)(sndgs + 372);
    ARQPostRequest(&snddrv.dma[i].request, id, 0, 1, (unsigned long)source, (unsigned long)dest, length,
                   SNDDRV_dmcallback);
    OSRestoreInterrupts(level);
    snddrv.nextdma++;
}

int SNDPLATFORM_init(void) {
    static int araminited = 0;
    int i;
    unsigned long aram;

    SNDDRV_DSPMixerInit(*(unsigned char*)(sndgs + 72));
    if (*(unsigned char*)(sndgs + 70) && !araminited) {
        ARInit(0, 0);
        ARQInit();
        araminited = 1;
    }
    for (i = 0; i < 64; i++)
        snddrv.dma[i].id = -1;
    if (!*(void**)(sndgs + 184) || !*(void**)(sndgs + 188)) {
        SNDARAM_init(*(unsigned short*)(sndgs + 40) + 32);
        SNDARAM_setpool(*(unsigned int*)(sndgs + 92), *(int*)(sndgs + 96));
    }
    aram = (unsigned long)SNDPLATFORM_memalloc(0, 1280);
    snddrv.aram = aram;
    snddrv.aramhalf = aram >> 1;
    snddrv.aramdouble = aram << 1;
    snddrv.alignedbuffer = (unsigned char*)((unsigned long)(snddrv.buffer + 31) & ~31);
    postrestoredma(snddrv.alignedbuffer, (void*)snddrv.aram, 1280);
    postrestoredma(snddrv.alignedbuffer, (void*)ARGetBaseAddress(), 256);
    AIInit(0);
    AXInit();
    if (*(unsigned char*)(sndgs + 72) == 3)
        AXSetMode(1);
    else
        AXSetMode(0);
    for (i = 0; i < 61; i++) {
        snddrv.voices[i].field08 = -1;
        snddrv.voices[i].field0C = -1;
        snddrv.voices[i].axvoice = 0;
        snddrv.voices[i].field14 = 0;
        snddrv.voices[i].field15 = 1;
        snddrv.voices[i].active = 0;
    }
    for (i = 0; i < *(unsigned char*)(sndgs + 77); i++) {
        if ((*(unsigned short*)(sndgs + 80 + i * 2) & 4) && !snddrv.mixerinited) {
            SNDDRV_initmixer();
            break;
        }
    }
    snddrv.activevoices = 0;
    OSCreateThread(&snddrv.thread, (void* (*)(void*))SNDDRV_thread, 0, snddrv.stack + sizeof(snddrv.stack), sizeof(snddrv.stack), 1, 1);
    OSInitThreadQueue(&snddrv.threadqueue);
    OSResumeThread(&snddrv.thread);
    AXRegisterAuxACallback((void*)SNDDRV_audiocallback, 0);
    *(unsigned char*)(sndgs + 360) = 1;
    SNDSYS_add100hzclient(SNDDRV_freenextframe);
    SNDSYS_add100hzclient(SNDDRV_DSPMixerUpdate);
    return 0;
}

/* inferred: frees the software mixer's buffers */
static inline void freemixbuffers(void) {
    int i;

    SNDSYS_entercritical();
    for (i = 0; i < *(unsigned char*)(sndgs + 72); i++)
        SNDMEMI_free(snddrv.mixalloc[i]);
    SNDSYS_leavecritical();
}

int SNDPLATFORM_restore(void) {
    if (snddrv.reset)
        snddrv.reset(4);
    AXRegisterAuxACallback(0, 0);
    if (snddrv.mixerinited) {
        snddrv.mixerinited = 0;
        freemixbuffers();
        MIXrestorefn();
    }
    SNDARAM_restore();
    while (*(signed char*)(sndgs + 363))
        SNDSYS_leavecritical();
    OSCancelThread(&snddrv.thread);
    SNDSYS_remove100hzclient(SNDDRV_freenextframe);
    SNDSYS_remove100hzclient(SNDDRV_DSPMixerUpdate);
    AXQuit();
    return 0;
}

// ISNDserve, the driver's service pass. For every packet player with a voice it
// counts down the start delay (then sets the voice's pitch, position and volume
// and starts each channel's AX voice as a 16-bit stream over its DSP buffer),
// and when playback passes the middle of the buffer into the other half it
// frees that half's frames and refills it; it then flushes the packet callbacks
// and frees the AX voices of software voices whose first AX voice has stopped.
// Then the voice range a render mode may use (mode bit 0x200: none below the
// first driver voice; bit 4: from the first driver voice to the voice count),
// resolving a timbre's sample data asynchronously (the data offset is taken
// from the header when none is given, and the sample pointer becomes the buffer
// plus the remaining offset), and SNDPLATFORM_playtimbre, which starts a timbre
// on a voice: through the mixer (each channel's mixer voice initialised with
// its sample data, length and loop, the voice's pitch, position, effect, time
// and filters set, then played) or, for DSP voices, through AX (ADPCM or 16-bit
// PCM addresses in nibbles or samples with the loop, or a silent loop, ADPCM
// coefficients and loop context per channel; -9 when no AX voice is free).
// sndgs, snddrv and the AX types are named by their symbols; the views of
// sndgs, snddrv, the AX voice, the packet player, the voice record and the
// patch header's members are inferred. The driver voice's active flag is
// volatile: the AX callback clears it.
extern "C" {
void* memset(void*, int, unsigned long);
void SNDI_memcpy(void*, const void*, int);
}

struct AXPBADDR {
    unsigned short loopFlag;
    unsigned short format;
    unsigned short loopAddressHi;
    unsigned short loopAddressLo;
    unsigned short endAddressHi;
    unsigned short endAddressLo;
    unsigned short currentAddressHi;
    unsigned short currentAddressLo;
};

struct AXPBADPCM {
    unsigned short a[8][2];
    unsigned short gain;
    unsigned short pred_scale;
    unsigned short yn1;
    unsigned short yn2;
};

struct AXPBADPCMLOOP {
    unsigned short loop_pred_scale;
    unsigned short loop_yn1;
    unsigned short loop_yn2;
};

extern "C" {
void AXSetVoiceSrcType(AXVPB*, unsigned long);
void AXSetVoiceAdpcm(AXVPB*, AXPBADPCM*);
void AXSetVoiceAdpcmLoop(AXVPB*, AXPBADPCMLOOP*);
void AXSetVoiceAddr(AXVPB*, AXPBADDR*);
void AXSetVoiceState(AXVPB*, unsigned short);
}

/* inferred: the patch header's sample fields */
struct SNDIPATCHHEADER {
    unsigned char unknown00[3];
    signed char channels;
    unsigned char unknown04[15];
    unsigned char mixmode;
    unsigned char unknown14[66];
    unsigned short rate;
    unsigned int length;
    unsigned int loopStart;
    int loopEnd;
    unsigned char** data;
    int offset;
    int offsets[4];
    int extra[8];
    unsigned char* coefs[4];
    unsigned short* loopContext[4];
};

void SNDPKTPLAYI_freeframes(int, int, int);
void SNDPKTPLAYI_flushcallbackdata();

void iSNDserve() {
    SNDDRVPKTCHAN* packet;
    SNDVOICEVIEW* voice;
    SNDDRVVOICE* channel;
    unsigned int start;
    unsigned int end;
    int i;
    int j;
    int half;

    for (i = 0; i < SNDGS.packetCount; i++) {
        packet = snddrv.packets[i];
        if (!packet || packet->voice < 0)
            continue;
        channel = &snddrv.voices[packet->voice];
        voice = &SNDGS.voices[packet->voice];
        if (packet->delay >= 0) {
            packet->delay--;
            if (packet->delay < 0) {
                AXPBADPCMLOOP loop;
                AXPBADDR addr;
                AXPBADPCM adpcm;

                addr.loopFlag = 1;
                addr.format = 10;
                adpcm.gain = 0x800;
                memset(&loop, 0, sizeof(loop));
                SNDPLATFORM_setpitch(packet->voice);
                SNDPLATFORM_set3dpos(packet->voice);
                SNDPLATFORM_setvol(packet->voice);
                for (j = 0; j < voice->channelCount; j++) {
                    start = packet->dsp[j] >> 1;
                    end = (packet->dsp[j] + 3070) >> 1;
                    AXSetVoiceAdpcm(snddrv.voices[voice->channels[j]].axvoice, &adpcm);
                    AXSetVoiceAdpcmLoop(snddrv.voices[voice->channels[j]].axvoice, &loop);
                    addr.endAddressHi = end >> 16;
                    addr.endAddressLo = end;
                    addr.currentAddressHi = start >> 16;
                    addr.currentAddressLo = start;
                    addr.loopAddressHi = addr.currentAddressHi;
                    addr.loopAddressLo = start;
                    AXSetVoiceAddr(snddrv.voices[voice->channels[j]].axvoice, &addr);
                    AXSetVoiceState(snddrv.voices[voice->channels[j]].axvoice, 1);
                    snddrv.voices[voice->channels[j]].vallocated = 1;
                }
            }
        }
        half = (int)((channel->axvoice->currentHi << 16) + channel->axvoice->currentLo) > (int)packet->middle;
        if (half != packet->half) {
            if (packet->frames[packet->half]) {
                for (j = 0; j < voice->channelCount; j++)
                    SNDPKTPLAYI_freeframes(i, j, packet->frames[packet->half]);
                packet->frames[packet->half] = 0;
            }
            SNDDRV_fillbufwithpackets(i, (SNDDRVPKTCHAN*)packet, (CHANPUB*)voice);
            packet->half = half;
        }
    }
    SNDPKTPLAYI_flushcallbackdata();
    for (i = 0; i < SNDGS.firstVoice; i++) {
        channel = &snddrv.voices[i];
        voice = &SNDGS.voices[i];
        if (channel->axvoice && channel->vallocated && !channel->axvoice->state && channel->offset >= 0 &&
            channel->vallocated == 1) {
            for (j = 0; j < SNDGS.voices[i].channelCount; j++) {
                if (snddrv.voices[voice->channels[j]].vallocated) {
                    SNDDRV_freeaxvoice(voice->channels[j], 0);
                    snddrv.voices[voice->channels[j]].vallocated = 0;
                }
            }
        }
    }
}

void SNDPLATFORM_getvoicerange(int mode, int* low, int* high) {
    if (mode & 0x200) {
        *low = 0;
        *high = SNDGS.firstVoice;
    } else if (mode & 4) {
        *low = SNDGS.firstVoice;
        *high = SNDGS.voiceCount;
    }
}

int SNDPLATFORM_asyncresolvetimbre(SNDIPATCHHEADER* header, char* buffer, int* offset) {
    if (*offset == 0)
        *offset = header->offset;
    *header->data = (unsigned char*)(buffer + (header->offset - *offset));
    return 0;
}
void SNDPLATFORM_setfxlevel(int, int);
extern "C" {
void MIX_playinit(int, int, int, void*, int, int, int, unsigned int, unsigned int, int, int, int, int);
void MIX_play(int);
}

int SNDPLATFORM_playtimbre(SNDIPATCHHEADER* header, void* data, int index, int time, int unused, int lowpass, int highpass) {
    SNDVOICEVIEW* voice;
    SNDDRVVOICE* drv;
    unsigned int current;
    unsigned int loopAddress;
    unsigned int endAddress;
    unsigned int step;
    unsigned int offset;
    unsigned int start;
    unsigned int end;
    int i;

    voice = &SNDGS.voices[index];
    drv = &snddrv.voices[index];
    voice->rate = header->rate;
    voice->length = header->length;
    step = 0;
    offset = 0;
    voice->channelCount = header->channels;
    if (voice->flags & 0x200) {
        AXPBADPCMLOOP loop;
        AXPBADDR addr;
        AXPBADPCM adpcm;

        memset(&loop, 0, sizeof(loop));
        if (header->loopEnd >= 0) {
            addr.loopFlag = 1;
            end = header->loopEnd;
            start = header->loopStart;
        } else {
            addr.loopFlag = 0;
            if (voice->format == 18)
                start = snddrv.silenceAdpcm;
            else
                start = snddrv.silence;
            end = header->length - 1;
        }
        if (voice->format == 18) {
            addr.format = 0;
            adpcm.gain = 0;
            adpcm.yn1 = 0;
            adpcm.yn2 = 0;
            step = ((header->length + 13) / 14 * 14 << 3) / 7;
            offset = (unsigned int)*header->data << 1;
            start = start * 8 / 7;
            end = end * 8 / 7;
        } else if (voice->format == 7) {
            addr.format = 10;
            adpcm.gain = 0x800;
            step = header->length;
            offset = (unsigned int)*header->data >> 1;
        }
        drv->offset = offset;
        if (!SNDDRV_allocateaxvoice(index))
            return -9;
        SNDPLATFORM_set3dpos(index);
        SNDPLATFORM_setvol(index);
        SNDPLATFORM_setpitch(index);
        SNDPLATFORM_setfxlevel(voice->channels[0], 0);
        for (i = 0; i < header->channels; i++) {
            if (voice->format == 18) {
                SNDI_memcpy(&adpcm, header->coefs[i], 32);
                adpcm.pred_scale = header->coefs[i][32];
                if (header->loopEnd > 0) {
                    loop.loop_pred_scale = *(unsigned char*)&header->loopContext[i][2];
                    loop.loop_yn1 = header->loopContext[i][0];
                    loop.loop_yn2 = header->loopContext[i][1];
                }
            }
            current = offset + step * i;
            loopAddress = header->loopEnd > 0 ? current + start : start;
            endAddress = current + end;
            endAddress -= 2;
            if (voice->format == 18) {
                current += 2;
                if ((loopAddress & 15) == 0)
                    loopAddress += 2;
                else if ((loopAddress & 15) == 1)
                    loopAddress += 1;
                if ((endAddress & 15) == 0)
                    endAddress += 2;
                else if ((endAddress & 15) == 1)
                    endAddress += 1;
            }
            AXSetVoiceSrcType(snddrv.voices[voice->channels[i]].axvoice, 1);
            AXSetVoiceAdpcm(snddrv.voices[voice->channels[i]].axvoice, &adpcm);
            AXSetVoiceAdpcmLoop(snddrv.voices[voice->channels[i]].axvoice, &loop);
            addr.endAddressHi = endAddress >> 16;
            addr.endAddressLo = endAddress;
            addr.currentAddressHi = current >> 16;
            addr.currentAddressLo = current;
            addr.loopAddressHi = loopAddress >> 16;
            addr.loopAddressLo = loopAddress;
            AXSetVoiceAddr(snddrv.voices[voice->channels[i]].axvoice, &addr);
            AXSetVoiceState(snddrv.voices[voice->channels[i]].axvoice, 1);
            snddrv.voices[voice->channels[i]].vallocated = 1;
        }
    } else {
        for (i = 0; i < voice->channelCount; i++)
            MIX_playinit(voice->channels[i] - SNDGS.firstVoice, header->mixmode, 1, (char*)data + (&header->offset)[i], -1,
                         header->extra[i], voice->channelCount, voice->length, header->loopStart, header->loopEnd, 0, 0, i);
        SNDPLATFORM_setpitch(voice->channels[0]);
        SNDPLATFORM_set3dpos(voice->channels[0]);
        SNDPLATFORM_setfxlevel(voice->channels[0], 0);
        SNDPLATFORM_timemult(voice->channels[0], time);
        SNDPLATFORM_lowpass(voice->channels[0], lowpass);
        SNDPLATFORM_highpass(voice->channels[0], highpass);
        for (i = 0; i < voice->channelCount; i++)
            MIX_play(voice->channels[i] - SNDGS.firstVoice);
    }
    return 0;
}

// A driver voice's master voice (itself when it has none; the master field is
// read through a byte offset into the voice table, as the original adds the
// field offset to the scaled index before the table base) and the channel a
// driver voice occupies within its master voice (1 to 3, or 0; an inline
// search). sndgs is named by its symbol; its view and the voice records are
// inferred.
extern "C" char sndgs[];

int SNDDRV_getmastervoice(int voice) {
    int index = voice + *(unsigned char*)(sndgs + 51);
    int master = *(short*)(*(char**)(sndgs + 468) + index * 128 + 36);

    if (master == -1)
        master = index;
    return master;
}

/* inferred: the channel slot holding a driver voice */
static inline int channelof(SNDVOICEVIEW* master, int index) {
    int i;

    for (i = 1; i < 4; i++) {
        if (master->channels[i] == index)
            return i;
    }
    return 0;
}

int SNDDRV_getsamplechan(int voice) {
    SNDVOICEVIEW* voices = *(SNDVOICEVIEW**)(sndgs + 468);
    int index = voice + *(unsigned char*)(sndgs + 51);
    int master = voices[index].master;

    if (master == -1)
        return 0;
    return channelof(&voices[master], index);
}

// The packet player's platform overhead and creation (two 3 KB DSP buffers and
// three 32-byte aligned staging areas in the caller's memory). snddrv is named
// by its symbol; its view and the packet player's platform record are inferred.
extern "C" {
void* memset(void*, int, unsigned long);
}

int SNDPLATFORM_packetoverhead(void) {
    return sizeof(SNDDRVPKTCHAN);
}

int SNDPLATFORM_packetplaycreate(int index, void* memory) {
    SNDDRVPKTCHAN* packet = (SNDDRVPKTCHAN*)memory;

    memset(packet, 0, sizeof(SNDDRVPKTCHAN));
    packet->dspmem[0] = SNDPLATFORM_memalloc(0, 3072);
    if (!packet->dspmem[0])
        return -9;
    packet->dspmem[1] = SNDPLATFORM_memalloc(0, 3072);
    if (!packet->dspmem[1])
        return -9;
    packet->staging[0] = (unsigned char*)(((unsigned int)packet->buffer + 31) & ~31);
    packet->staging[1] = (unsigned char*)(((unsigned int)packet->buffer + 1631) & ~31);
    packet->staging[2] = (unsigned char*)(((unsigned int)packet->buffer + 3231) & ~31);
    packet->state = -1;
    snddrv.packets[index] = packet;
    return 0;
}

// SNDPLATFORM_packetplay starts a packet-stream voice. Software voices start a
// mixer channel per channel and are set up through the pitch, position, volume,
// effect, time, low-pass and high-pass setters before playing; hardware voices
// reset the packet player's state, take an AX voice, and for each channel clear
// the staging area, flush it and queue its DMA to the channel's ARAM buffer
// (through a free DMA request slot), then set the AX sample-rate converter.
// snddrv and sndgs are named by their symbols; their views, the voice, channel,
// packet-player and DMA-slot records, the sample-format view and the DMA
// helpers are inferred.
/* inferred: the sample format */
struct SNDSAMPLEFORMAT {
    unsigned short rate;
    unsigned char channels;
};
extern void (*MIXplayinitfn)(int, int, int, int, int, void*, int, int, int, int, int, int, int);
extern void (*MIXplayfn)(int);

extern "C" {
void* memset(void*, int, unsigned long);
void DCFlushRange(void*, unsigned long);
int OSDisableInterrupts(void);
int OSRestoreInterrupts(int);
void ARQPostRequest(ARQRequest*, unsigned long, unsigned long, unsigned long, unsigned long, unsigned long,
                    unsigned long, void (*)(unsigned long));
void AXSetVoiceSrcType(_AXVPB*, unsigned long);
}
void SNDPLATFORM_setfxlevel(int, int);

static inline void postplaydma(int j, unsigned long owner, void* source, unsigned int dest, unsigned int length) {
    snddrv.dma[j].pending = 1;
    snddrv.dma[j].source = source;
    snddrv.dma[j].destination = dest;
    snddrv.dma[j].length = length;
    snddrv.dma[j].time = SNDGS.ticks;
    ARQPostRequest(&snddrv.dma[j].request, owner, 0, 1, (unsigned long)source, dest, length, SNDDRV_dmcallback);
}

static inline void dmatoaram(void* source, unsigned int dest, unsigned int length) {
    int enabled = OSDisableInterrupts();
    int j = allocdma(snddrv.dmaCounter);

    postplaydma(j, snddrv.dmaCounter, source, dest, length);
    OSRestoreInterrupts(enabled);
    snddrv.dmaCounter++;
}

int SNDPLATFORM_packetplay(int player, int index, int timemult, int, int lowpass, int highpass,
                           SNDSAMPLEFORMAT* format, void** data) {
    SNDVOICEVIEW* voice;
    int i;
    SNDDRVPKTCHAN* packet;
    unsigned int start;
    unsigned int end;

    voice = &SNDGS.voices[index];
    voice->rate = format->rate;
    voice->value10 = 0;
    snddrv.voices[index].player = player;
    voice->channelCount = format->channels;
    if (voice->flags & 4) {
        for (i = 0; i < voice->channelCount; i++)
            MIXplayinitfn(voice->channels[i] - SNDGS.channelBase, voice->format, 0, 0, -1, data[i],
                          voice->channelCount, 0, -1, -1, 0, 0, i);
        SNDPLATFORM_setpitch(voice->channels[0]);
        SNDPLATFORM_set3dpos(voice->channels[0]);
        SNDPLATFORM_setvol(voice->channels[0]);
        SNDPLATFORM_setfxlevel(voice->channels[0], 0);
        SNDPLATFORM_timemult(voice->channels[0], timemult);
        SNDPLATFORM_lowpass(voice->channels[0], lowpass);
        SNDPLATFORM_highpass(voice->channels[0], highpass);
        for (i = 0; i < voice->channelCount; i++)
            MIXplayfn(voice->channels[i] - SNDGS.channelBase);
        return 0;
    } else {
        packet = snddrv.packets[player];
        packet->voice = index;
        packet->frames[0] = 0;
        packet->frames[1] = 0;
        packet->value12e8 = 0;
        packet->delay = 2;
        packet->half = 1;
        SNDDRV_allocateaxvoice(index);
        for (i = 0; i < format->channels; i++) {
            start = packet->dsp[i] >> 1;
            end = (packet->dsp[i] + 3070) >> 1;
            if (i == 0)
                packet->middle = (start + end) >> 1;
            memset(packet->staging[i], 0, 1536);
            DCFlushRange(packet->staging[i], 1536);
            dmatoaram(packet->staging[i], packet->dsp[i], 1536);
            AXSetVoiceSrcType(snddrv.voices[voice->channels[i]].axvoice, 1);
            snddrv.voices[voice->channels[i]].allocated = 1;
        }
        return 0;
    }
}

// Destroying a packet player's platform record (freeing its two DSP buffers and
// unregistering it), the fixed output latency, and SNDPLATFORM_setvol, which
// applies a voice's volume (the product of its two volume bytes) to each of its
// channels: through the DSP mixer for hardware voices, else as dry gains of the
// software mixer per output. snddrv and sndgs are named by their symbols; their
// views, the voice and channel records and the platform record are inferred,
// and the scale factors are entries of the file's .sdata2 pool.
void SNDDRV_DSPMixerSetVol(_AXVPB*, int);
extern "C" void SNDMIX_setdrygain(int, int, float);

int SNDPLATFORM_packetplaydestroy(int index) {
    SNDDRVPKTCHAN* packet = snddrv.packets[index];

    SNDPLATFORM_memfree(0, (unsigned int)packet->dspmem[0]);
    SNDPLATFORM_memfree(0, (unsigned int)packet->dspmem[1]);
    snddrv.packets[index] = 0;
    return 0;
}

int SNDPLATFORM_outputlatency(void) {
    return -15;
}

void SNDPLATFORM_setvol(int index) {
    SNDVOICEVIEW* voice = &SNDGS.voices[index];
    int i;
    int j;

    if (voice->flags & 0x200) {
        int volume = (int)(0.007874016f * ((float)voice->volume * (float)voice->fx.gain));
        volume <<= 8;
        for (i = 0; i < voice->channelCount; i++)
            SNDDRV_DSPMixerSetVol(snddrv.channels[voice->channels[i]].voice, volume);
    } else {
        float gain = 1.8921513e-09f * ((float)voice->volume * (float)voice->fx.gain);
        int outputs = SNDGS.outputs == 1 ? 2 : SNDGS.outputs;
        for (i = 0; i < voice->channelCount; i++) {
            SNDDRVCHANNELVIEW* channel = &snddrv.channels[voice->channels[i]];
            for (j = 0; j < outputs; j++)
                SNDMIX_setdrygain(voice->channels[i] - SNDGS.channelBase, j, gain * (float)channel->levels[j]);
        }
    }
}

// The dry level, applied through the volume for AX and mixer voices. sndgs is
// named by its symbol; its view is inferred. The voice record's flags are read
// through a byte offset into the voice table (the original adds the field
// offset to the scaled index before the table base).
extern "C" char sndgs[];

void SNDPLATFORM_setdrylevel(int voice) {
    if (*(unsigned short*)(*(char**)(sndgs + 468) + voice * 128 + 32) & 0x200 || *(unsigned short*)(*(char**)(sndgs + 468) + voice * 128 + 32) & 4)
        SNDPLATFORM_setvol(voice);
}

// SNDPLATFORM_set3dpos recomputes a voice's channel levels from its azimuth
// (both levels at 0x6000 for mono output) and applies them: through the DSP
// mixer's pan for hardware voices (in three-speaker mode, first easing the
// front levels of channels whose second level exceeds 15000), else through the
// volume. SNDPLATFORM_setpitch then sets a voice's pitch from its rate over the
// output rate: as a source ratio on each hardware channel's AX voice (-8 for a
// channel without one), else through the mixer's pitch hook (the mixer channel
// is relative to the channel base). snddrv and sndgs are named by their
// symbols; their views and the voice and channel records are inferred, and the
// constants are entries of the file's .sdata2 pool.
void SNDDRV_DSPMixerSetPan(_AXVPB*, unsigned short*);
void SNDI_aztospkrvol(int, short*);

void SNDPLATFORM_set3dpos(int index) {
    SNDVOICEVIEW* voice = &SNDGS.voices[index];
    SNDDRVCHANNELVIEW* own = &snddrv.channels[index];
    int i;

    if (SNDGS.outputs == 1) {
        for (i = 0; i < voice->channelCount; i++) {
            snddrv.channels[voice->channels[i]].levels[0] = 0x6000;
            snddrv.channels[voice->channels[i]].levels[1] = 0x6000;
        }
        if (voice->flags & 0x200) {
            for (i = 0; i < voice->channelCount; i++)
                SNDDRV_DSPMixerSetPan(snddrv.channels[voice->channels[i]].voice,
                                      (unsigned short*)snddrv.channels[voice->channels[i]].levels);
        } else {
            SNDPLATFORM_setvol(index);
        }
        return;
    }
    if (voice->channelCount == 1) {
        SNDI_aztospkrvol(voice->azimuth, own->levels);
    } else {
        for (i = 0; i < voice->channelCount; i++)
            SNDI_aztospkrvol(voice->azimuths[i], snddrv.channels[voice->channels[i]].levels);
    }
    if (voice->flags & 0x200) {
        if (SNDGS.outputs == 3) {
            for (i = 0; i < 3; i++) {
                SNDDRVCHANNELVIEW* channel = &snddrv.channels[voice->channels[i]];
                if (channel->levels[1] > 15000) {
                    channel->levels[0] = (float)channel->levels[0] *
                                         (float)(1.0 - (0.5f * (float)channel->levels[1]) / 100000.0f);
                    channel = &snddrv.channels[voice->channels[i]];
                    channel->levels[2] = (float)channel->levels[2] *
                                         (1.0f - (0.5f * (float)channel->levels[1]) / 100000.0f);
                }
            }
        }
        for (i = 0; i < voice->channelCount; i++)
            SNDDRV_DSPMixerSetPan(snddrv.channels[voice->channels[i]].voice,
                                  (unsigned short*)snddrv.channels[voice->channels[i]].levels);
    } else {
        SNDPLATFORM_setvol(index);
    }
}

extern "C" void AXSetVoiceSrcRatio(_AXVPB*, float);
extern void (*MIXsetpitchfn)(int, int);

int SNDPLATFORM_setpitch(int index) {
    SNDVOICEVIEW* voice = &SNDGS.voices[index];
    unsigned int pitch;
    float ratio;
    int i;

    if (voice->flags & 0x200) {
        pitch = (voice->rate << 12) / SNDGS.sampleRate;
        pitch = (pitch * voice->pitch) >> 12;
        ratio = 1.0f / 4096.0f * (float)pitch;
        for (i = 0; i < voice->channelCount; i++) {
            if (!snddrv.channels[voice->channels[i]].voice)
                return -8;
            AXSetVoiceSrcRatio(snddrv.channels[voice->channels[i]].voice, ratio);
        }
    } else {
        pitch = (voice->rate << 15) / SNDGS.sampleRate;
        pitch = (pitch * voice->pitch) >> 11;
        for (i = 0; i < voice->channelCount; i++)
            MIXsetpitchfn(voice->channels[i] - SNDGS.channelBase, pitch);
    }
    return 0;
}

// The time multiplier and the low-pass cutoff (the value over half the output
// rate at +42; the constants are entries of the file's .sdata2 pool) of a mixer
// voice's channels. sndgs is named by its symbol; its view and the voice
// records are inferred.
extern "C" char sndgs[];

extern "C" void MIX_settimemult(int, int);

extern "C" void MIX_setlowpass(int, float);

int SNDPLATFORM_timemult(int voice, int value) {
    SNDVOICEVIEW* sound = &(*(SNDVOICEVIEW**)(sndgs + 468))[voice];
    int i;

    if (sound->flags & 4) {
        for (i = 0; i < sound->count; i++)
            MIX_settimemult(sound->channels[i] - *(unsigned char*)(sndgs + 51), value);
    }
    return 0;
}

void SNDPLATFORM_lowpass(int voice, int value) {
    SNDVOICEVIEW* sound = &(*(SNDVOICEVIEW**)(sndgs + 468))[voice];
    int i;

    if (sound->flags & 4) {
        int cutoff = value * (1.0f / (0.5f * *(unsigned short*)(sndgs + 42)));
        for (i = 0; i < sound->count; i++)
            MIX_setlowpass(sound->channels[i] - *(unsigned char*)(sndgs + 51), cutoff);
    }
}

// The high-pass setting of a mixer voice's channels, and the current frame of a
// voice (for an AX voice, its playback address minus the start, scaled by 7/8
// for type 18 samples; otherwise the software mixer's frame). SNDPLATFORM_stop
// stops a voice: an AX voice's packet player is detached (through an inline
// helper) and its AX voices stopped and released; a mixer voice's channels are
// stopped and freed. sndgs and snddrv are named by their symbols; their views,
// the voice records and the AX voice block's members are inferred.
extern "C" {
extern char sndgs[];
extern void (*MIXstopfn)(int);
int MIX_getframe(int);
void AXSetVoiceState(_AXVPB*, unsigned short);
}
void SNDVOICEI_free(int);
extern "C" void MIX_sethighpass(int, int);

void SNDPLATFORM_highpass(int voice, int value) {
    SNDVOICEVIEW* sound = &(*(SNDVOICEVIEW**)(sndgs + 468))[voice];
    int i;

    if (sound->flags & 4) {
        for (i = 0; i < sound->count; i++)
            MIX_sethighpass(sound->channels[i] - *(unsigned char*)(sndgs + 51), value);
    }
}

int SNDPLATFORM_getcurframe(int voice) {
    SNDDRVVOICE* driver = &snddrv.voices[voice];
    SNDVOICEVIEW* sound = &(*(SNDVOICEVIEW**)(sndgs + 468))[voice];

    if (sound->flags & 0x200) {
        int position;

        if (!driver->axvoice)
            return -8;
        position = (driver->axvoice->currentHi << 16) + driver->axvoice->currentLo;
        if (sound->type == 18)
            return ((position - driver->start) * 7) >> 3;
        return position - driver->start;
    }
    return MIX_getframe(voice - *(unsigned char*)(sndgs + 51));
}

/* inferred: detaches a voice from its packet player */
static inline void stoppacket(SNDDRVVOICE* driver) {
    if (driver->packet >= 0) {
        snddrv.packets[driver->packet]->state = -1;
        driver->packet = -1;
    }
}

int SNDPLATFORM_stop(int voice) {
    SNDVOICEVIEW* sound = &(*(SNDVOICEVIEW**)(sndgs + 468))[voice];
    SNDDRVVOICE* driver = &snddrv.voices[voice];
    int i;

    if (sound->flags & 0x200) {
        stoppacket(driver);
        for (i = 0; i < sound->count; i++) {
            if (!snddrv.voices[sound->channels[i]].axvoice)
                return -8;
            AXSetVoiceState(snddrv.voices[sound->channels[i]].axvoice, 0);
            if (snddrv.voices[sound->channels[i]].allocated) {
                SNDDRV_freeaxvoice(sound->channels[i], 0);
                snddrv.voices[sound->channels[i]].allocated = 0;
            }
        }
    } else {
        for (i = 0; i < sound->count; i++) {
            MIXstopfn(sound->channels[i] - *(unsigned char*)(sndgs + 51));
            SNDVOICEI_free(sound->channels[i]);
        }
    }
    return 0;
}

// Filter installation (unsupported), memory allocation and freeing through the
// installed allocator or ARAM (sizes rounded up to 32 bytes), and
// SNDPLATFORM_download, which flushes the block and posts its transfer to ARAM
// through ARQ in a new DMA slot, returning the request number, and
// SNDPLATFORM_downloadcomplete, which reports whether a request is finished (1
// when no DMA slot holds it any more) and re-posts every slot of a request that
// has been pending for more than 200 ticks. snddrv and sndgs are named by their
// symbols; their views, the DMA slot layout and the slot, posting and
// re-posting inline helpers are inferred. A slot's id is volatile: the DMA
// callback frees the slot.
extern "C" {
extern char sndgs[];
void OSInitMutex(OSMutex*);
void OSLockMutex(OSMutex*);
void OSUnlockMutex(OSMutex*);
int OSDisableInterrupts(void);
int OSRestoreInterrupts(int);
void ARQRemoveRequest(ARQRequest*);
}
void* SNDARAM_alloc(int);
void SNDARAM_free(unsigned int);

/* inferred: posts a transfer, in a new slot when slot is -1 */
static inline int postmemdma(int slot, int priority, unsigned int source, unsigned int dest, int length) {
    int level = OSDisableInterrupts();
    int id;
    int index;

    if (slot == -1) {
        index = allocdma(snddrv.requests);
        id = snddrv.requests;
        snddrv.dma[index].priority = priority;
        snddrv.dma[index].sourceaddr = source;
        snddrv.dma[index].destination = dest;
        snddrv.dma[index].length = (length + 31) & ~31;
        snddrv.dma[index].time = *(unsigned int*)(sndgs + 372);
    } else {
        index = slot;
        id = snddrv.dma[index].vid;
    }
    ARQPostRequest(&snddrv.dma[index].request, id, 0, priority, source, dest, (length + 31) & ~31, SNDDRV_dmcallback);
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
    return postmemdma(-1, 0, (unsigned int)source, (unsigned int)dest, length);
}
static inline void repostdma(int id) {
    int j;
    int level;

    level = OSDisableInterrupts();
    for (j = 0; j < 64; j++) {
        if (snddrv.dma[j].vid == id) {
            ARQRemoveRequest(&snddrv.dma[j].request);
            postmemdma(j, snddrv.dma[j].priority, snddrv.dma[j].sourceaddr, snddrv.dma[j].destination, snddrv.dma[j].length);
        }
    }
    OSRestoreInterrupts(level);
}

int SNDPLATFORM_downloadcomplete(int id) {
    int level;
    int i;
    int j;
    int priority;
    unsigned int source;
    unsigned int dest;
    unsigned int length;

    level = OSDisableInterrupts();
    for (i = 0; i < 64; i++) {
        if (snddrv.dma[i].vid != -1 && snddrv.dma[i].vid == id) {
            if (*(unsigned int*)(sndgs + 372) - snddrv.dma[i].time > 200) {
                OSRestoreInterrupts(level);
                repostdma(id);
            } else {
                OSRestoreInterrupts(level);
            }
            return 0;
        }
    }
    OSRestoreInterrupts(level);
    return 1;
}

// The sound driver's mutex wrappers, the last functions of the file: they
// initialise, lock and unlock the driver's mutex through the Dolphin OS
// (freeing it does nothing). snddrv is named by its symbol; the view of it is
// inferred (only the mutex's offset is known; the rest of the 0xab20-byte
// structure is opaque).
extern "C" {
void OSInitMutex(OSMutex*);
void OSLockMutex(OSMutex*);
void OSUnlockMutex(OSMutex*);
}

void SNDI_mutexalloc() {
    OSInitMutex(&snddrv.mutex);
}

void SNDI_mutexfree() {
}

void SNDI_mutexlock() {
    OSLockMutex(&snddrv.mutex);
}

void SNDI_mutexunlock() {
    OSUnlockMutex(&snddrv.mutex);
}
