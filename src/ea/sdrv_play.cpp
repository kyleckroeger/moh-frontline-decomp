/* A fragment of the sound driver (snddrv.c, 0x8016b788): SNDPLATFORM_packetplay
   starts a packet-stream voice. Software voices start a mixer channel per
   channel and are set up through the pitch, position, volume, effect, time,
   low-pass and high-pass setters before playing; hardware voices reset the
   packet player's state, take an AX voice, and for each channel clear the
   staging area, flush it and queue its DMA to the channel's ARAM buffer
   (through a free DMA request slot), then set the AX sample-rate converter.
   snddrv and sndgs are named by their symbols; their views, the voice,
   channel, packet-player and DMA-slot records, the sample-format view and
   the DMA helpers are inferred. */
struct _AXVPB;
struct ARQRequest;

/* inferred: a DMA request slot (64 bytes) */
struct SNDDRVDMAVIEW {
    int id;
    unsigned char request[32];
    int pending;
    void* source;
    unsigned int destination;
    unsigned int length;
    int time;
    unsigned char unknown38[8];
};

/* inferred: a driver channel record (48 bytes) */
struct SNDDRVCHANNELVIEW {
    unsigned char unknown00[12];
    unsigned char player;
    unsigned char unknown0d[7];
    unsigned char active;
    unsigned char unknown15[23];
    _AXVPB* voice;
};

/* inferred: a packet player's platform record */
struct SNDDRVPACKETVIEW {
    unsigned char buffer[4800];
    unsigned char* staging[3];
    signed char voice;
    unsigned char half;
    signed char delay;
    unsigned char unknown12cf;
    unsigned int dsp[3];
    unsigned int middle;
    short frames[2];
    unsigned char unknown12e4[4];
    int value12e8;
};

struct SNDDRVVIEW {
    SNDDRVCHANNELVIEW channels[77];
    unsigned char unknown0e70[20];
    SNDDRVPACKETVIEW* packets[8];
    unsigned char unknown0ea4[0x9864 - 0xea4];
    int dmaCounter;
    unsigned char unknown9868[0x98a8 - 0x9868];
    SNDDRVDMAVIEW dma[64];
};

/* inferred: a voice record (128 bytes) */
struct SNDVOICEVIEW {
    unsigned char unknown00[4];
    short channels[6];
    int value10;
    unsigned char unknown14[8];
    unsigned short rate;
    unsigned char format;
    unsigned char channelCount;
    unsigned short flags;
    unsigned char unknown22[94];
};

struct SNDGSVIEW {
    unsigned char unknown000[51];
    unsigned char channelBase;
    unsigned char unknown034[320];
    int ticks;
    unsigned char unknown178[92];
    SNDVOICEVIEW* voices;
};

/* inferred: the sample format */
struct SNDSAMPLEFORMAT {
    unsigned short rate;
    unsigned char channels;
};

extern SNDGSVIEW sndgs;
extern SNDDRVVIEW snddrv;
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
int SNDPLATFORM_setpitch(int);
void SNDPLATFORM_set3dpos(int);
void SNDPLATFORM_setvol(int);
void SNDPLATFORM_setfxlevel(int, int);
void SNDPLATFORM_timemult(int, int);
void SNDPLATFORM_lowpass(int, int);
void SNDPLATFORM_highpass(int, int);
void SNDDRV_allocateaxvoice(int);
void SNDDRV_dmcallback(unsigned long);

/* inferred inline helpers: claim a free DMA slot and queue a transfer */
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

static inline void postdma(int j, unsigned long owner, void* source, unsigned int dest, unsigned int length) {
    snddrv.dma[j].pending = 1;
    snddrv.dma[j].source = source;
    snddrv.dma[j].destination = dest;
    snddrv.dma[j].length = length;
    snddrv.dma[j].time = sndgs.ticks;
    ARQPostRequest((ARQRequest*)snddrv.dma[j].request, owner, 0, 1, (unsigned long)source, dest, length, SNDDRV_dmcallback);
}

static inline void dmatoaram(void* source, unsigned int dest, unsigned int length) {
    int enabled = OSDisableInterrupts();
    int j = allocdma(snddrv.dmaCounter);

    postdma(j, snddrv.dmaCounter, source, dest, length);
    OSRestoreInterrupts(enabled);
    snddrv.dmaCounter++;
}

int SNDPLATFORM_packetplay(int player, int index, int timemult, int, int lowpass, int highpass,
                           SNDSAMPLEFORMAT* format, void** data) {
    SNDVOICEVIEW* voice;
    int i;
    SNDDRVPACKETVIEW* packet;
    unsigned int start;
    unsigned int end;

    voice = &sndgs.voices[index];
    voice->rate = format->rate;
    voice->value10 = 0;
    snddrv.channels[index].player = player;
    voice->channelCount = format->channels;
    if (voice->flags & 4) {
        for (i = 0; i < voice->channelCount; i++)
            MIXplayinitfn(voice->channels[i] - sndgs.channelBase, voice->format, 0, 0, -1, data[i],
                          voice->channelCount, 0, -1, -1, 0, 0, i);
        SNDPLATFORM_setpitch(voice->channels[0]);
        SNDPLATFORM_set3dpos(voice->channels[0]);
        SNDPLATFORM_setvol(voice->channels[0]);
        SNDPLATFORM_setfxlevel(voice->channels[0], 0);
        SNDPLATFORM_timemult(voice->channels[0], timemult);
        SNDPLATFORM_lowpass(voice->channels[0], lowpass);
        SNDPLATFORM_highpass(voice->channels[0], highpass);
        for (i = 0; i < voice->channelCount; i++)
            MIXplayfn(voice->channels[i] - sndgs.channelBase);
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
            AXSetVoiceSrcType(snddrv.channels[voice->channels[i]].voice, 1);
            snddrv.channels[voice->channels[i]].active = 1;
        }
        return 0;
    }
}
