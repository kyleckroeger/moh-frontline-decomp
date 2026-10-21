/* A fragment of the sound driver (snddrv.c, 0x8016a578..0x8016ada0):
   SNDPLATFORM_init sets up the platform layer (DSP mixer, ARAM and the ARAM
   request queue once, the DMA slot table, the ARAM pool, an aligned transfer
   buffer posted to ARAM twice through inline DMA helpers, AI/AX, the voice
   records, the software mixer when a voice type needs it, the driver thread,
   the AUX A callback and the 100 Hz clients). SNDPLATFORM_restore undoes it
   - the installed reset hook is called, the AUX A callback
   removed, the software mixer's buffers freed and the mixer restored (when it
   was set up), ARAM restored, pending critical sections drained, the driver
   thread cancelled, the 100 Hz clients removed and AX shut down. sndgs and
   snddrv are named by their symbols; their views are inferred. */
struct OSThread;
struct OSThreadQueue;
struct ARQRequest;

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
void SNDDRV_freenextframe(void);
void SNDDRV_DSPMixerUpdate(void);
void SNDDRV_DSPMixerInit(int);
void SNDARAM_init(int);
void SNDARAM_setpool(unsigned int, int);
unsigned long SNDPLATFORM_memalloc(int, int);
void SNDDRV_dmcallback(unsigned long);
void* SNDDRV_thread(void*);
void SNDDRV_audiocallback(void*, void*);
void SNDDRV_initmixer(void);

/* inferred: an OS thread block */
struct OSThread {
    unsigned char unknown000[784];
};

struct OSThreadQueue {
    void* head;
    void* tail;
};

/* the ARAM queue request record (SDK layout) */
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

/* inferred: a voice record and a DMA request slot of the driver */
struct SNDDRVVOICE {
    char field00[8];
    int field08;
    signed char field0C;
    char field0D[2];
    unsigned char active;
    char field10[4];
    unsigned char field14;
    unsigned char field15;
    char field16[22];
    void* axvoice;
};

struct SNDDRVDMA {
    int id;
    ARQRequest request;
    int mode;
    void* source;
    void* dest;
    int length;
    int extra;
    char field38[8];
};

struct SNDDRVVIEW {
    SNDDRVVOICE voices[61];
    unsigned char unknown0b70[0xe70 - 0xb70];
    unsigned long aram;
    unsigned long aramhalf;
    unsigned long aramdouble;
    void (*reset)(int);
    unsigned char unknown0e80[0xf20 - 0xe80];
    unsigned char buffer[0x1540 - 0xf20];
    unsigned char* alignedbuffer;
    unsigned char unknown1544[4];
    OSThread thread;
    OSThreadQueue threadqueue;
    unsigned char stack[0x8000];
    int lastdma;
    int nextdma;
    unsigned char unknown9868[0x98a8 - 0x9868];
    SNDDRVDMA dma[64];
    unsigned char unknowna8a8[0xaac0 - 0xa8a8];
    signed char mixerinited;
    unsigned char activevoices;
    unsigned char unknownaac2[6];
    void* mixalloc[4];
};

extern SNDDRVVIEW snddrv;

/* inferred: claims a free DMA slot for a request id */
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

/* inferred: queues an ARAM transfer */
static inline void postdma(void* source, void* dest, int length) {
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
    aram = SNDPLATFORM_memalloc(0, 1280);
    snddrv.aram = aram;
    snddrv.aramhalf = aram >> 1;
    snddrv.aramdouble = aram << 1;
    snddrv.alignedbuffer = (unsigned char*)((unsigned long)(snddrv.buffer + 31) & ~31);
    postdma(snddrv.alignedbuffer, (void*)snddrv.aram, 1280);
    postdma(snddrv.alignedbuffer, (void*)ARGetBaseAddress(), 256);
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
    OSCreateThread(&snddrv.thread, SNDDRV_thread, 0, snddrv.stack + sizeof(snddrv.stack), sizeof(snddrv.stack), 1, 1);
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
