// sdspmix.c (0x8016d00c): the DSP mixer, EA's form of the SDK's mix.c: per-voice
// mixer channels (64 records of 80 bytes) holding the pan, auxiliary bus
// levels and volume that are pushed to the AX voice parameter block when the
// mode flags say so. SetPan and SetAuxBus store a channel's levels and flag
// them; Init resets every channel; InitChannel binds a channel to its voice
// and writes its initial levels to the voice; ReleaseChannel clears its
// voice; SetVol sets its volume (flagged for the next update); Update (the
// SDK's MIXUpdateSettings) for every channel with a voice moves pending input
// and mix levels to current, takes the channel volume (or zero when muted) as
// the new input level, and writes the AX voice's volume envelope and mix
// levels with per-frame deltas (/320, not the SDK's /160) and the
// mixer-control bits. _AXVPB is named by the mangled symbols; the channel
// record and AX voice members follow the SDK's mix.c names where the code
// matches them and are otherwise inferred. This file is compiled with GC/1.3
// (1.3.2 builds the mode flag with oris instead of lis/or).
extern "C" {
int OSDisableInterrupts(void);
int OSRestoreInterrupts(int);
}

struct AXPBMIXVIEW {
    unsigned short vL;
    unsigned short vDeltaL;
    unsigned short vR;
    unsigned short vDeltaR;
    unsigned short vAuxAL;
    unsigned short vDeltaAuxAL;
    unsigned short vAuxAR;
    unsigned short vDeltaAuxAR;
    unsigned short vAuxBL;
    unsigned short vDeltaAuxBL;
    unsigned short vAuxBR;
    unsigned short vDeltaAuxBR;
    unsigned short vAuxBS;
    unsigned short vDeltaAuxBS;
    unsigned short vS;
    unsigned short vDeltaS;
    unsigned short vAuxAS;
    unsigned short vDeltaAuxAS;
};

struct _AXVPB {
    char field000[24];
    unsigned long index;
    unsigned long sync;
    char field020[292];
    unsigned short mixerCtrl;
    unsigned short field146;
    char field148[2];
    AXPBMIXVIEW mix;
    char field16E[46];
    unsigned short currentVolume;
    unsigned short currentDelta;
};

typedef struct MIXCHANNEL {
    _AXVPB* voice;
    unsigned long mode;
    int volume;
    int field0C;
    int field10;
    char field14[20];
    unsigned short v;
    unsigned short v1;
    unsigned short vL;
    unsigned short vL1;
    unsigned short vR;
    unsigned short vR1;
    unsigned short vS;
    unsigned short vS1;
    unsigned short vAL;
    unsigned short vAL1;
    unsigned short vAR;
    unsigned short vAR1;
    unsigned short vAS;
    unsigned short vAS1;
    unsigned short vBL;
    unsigned short vBL1;
    unsigned short vBR;
    unsigned short vBR1;
    unsigned short vBS;
    unsigned short vBS1;
} MIXCHANNEL;

static MIXCHANNEL __MIXChannel[64];
static int numoutputchannels;

void SNDDRV_DSPMixerSetPan(_AXVPB* voice, unsigned short* pan) {
    MIXCHANNEL* channel = &__MIXChannel[voice->index];

    channel->vR1 = pan[0];
    if (numoutputchannels != 3) {
        channel->vL1 = pan[1];
    } else {
        channel->vS1 = pan[1];
        channel->vL1 = pan[2];
    }
    channel->mode |= 0x40000000;
}

void SNDDRV_DSPMixerSetAuxBus(_AXVPB* voice, unsigned short level, int bus) {
    MIXCHANNEL* channel = &__MIXChannel[voice->index];

    if (bus == 1) {
        channel->vAR1 = level;
        channel->vAL1 = level;
        if (numoutputchannels == 3)
            channel->vAS1 = level;
    } else if (bus == 2) {
        channel->vBR1 = level;
        channel->vBL1 = level;
        if (numoutputchannels == 3)
            channel->vBS1 = level;
    }
    channel->mode |= 0x40000000;
}

void SNDDRV_DSPMixerInit(int outputs) {
    int i;

    for (i = 0; i < 64; i++) {
        __MIXChannel[i].mode = 0x50000000;
        __MIXChannel[i].volume = 0;
        __MIXChannel[i].field0C = 0;
        __MIXChannel[i].field10 = 0;
        __MIXChannel[i].v = __MIXChannel[i].vL = __MIXChannel[i].vR =
            __MIXChannel[i].vS = __MIXChannel[i].vAL = __MIXChannel[i].vAR =
                __MIXChannel[i].vAS = __MIXChannel[i].vBL = __MIXChannel[i].vBR =
                    __MIXChannel[i].vBS = 0;
    }
    numoutputchannels = outputs;
}

void SNDDRV_DSPMixerInitChannel(_AXVPB* voice, unsigned long mode, int input, unsigned short auxA,
                                unsigned short auxB) {
    MIXCHANNEL* channel = &__MIXChannel[voice->index];
    int enabled;
    unsigned short mix;

    channel->voice = voice;
    channel->mode = mode & 4;
    channel->volume = input;
    channel->vAL1 = auxA;
    channel->vAR1 = auxA;
    channel->vBL1 = auxB;
    channel->vBR1 = auxB;
    if (numoutputchannels > 2) {
        channel->vAS1 = auxA;
        channel->vBS1 = auxB;
    }
    if (channel->mode & 4)
        channel->v = 0;
    else
        channel->v = input;
    enabled = OSDisableInterrupts();
    voice->currentVolume = channel->v;
    voice->currentDelta = 0;
    voice->mix.vL = channel->vL;
    voice->mix.vDeltaL = 0;
    voice->mix.vR = channel->vR;
    voice->mix.vDeltaR = 0;
    voice->mix.vAuxAL = channel->vS;
    voice->mix.vDeltaAuxAL = 0;
    mix = 0;
    if (voice->mix.vS || voice->mix.vAuxAS || voice->mix.vAuxBS)
        mix |= 4;
    voice->mixerCtrl = mix;
    voice->sync |= 0x212;
    OSRestoreInterrupts(enabled);
}

void SNDDRV_DSPMixerReleaseChannel(_AXVPB* voice) {
    __MIXChannel[voice->index].voice = 0;
}

void SNDDRV_DSPMixerSetVol(_AXVPB* voice, int volume) {
    MIXCHANNEL* channel = &__MIXChannel[voice->index];

    channel->volume = volume;
    channel->mode |= 0x10000000;
}

void SNDDRV_DSPMixerUpdate() {
    int i;
    int setNewMixLevel;
    int setNewInputLevel;
    MIXCHANNEL* c;
    _AXVPB* axvpb;
    unsigned short mixerCtrl;
    unsigned short* p;

    for (i = 0; i < 64; i++) {
        setNewInputLevel = 0;
        setNewMixLevel = 0;
        c = &__MIXChannel[i];
        axvpb = c->voice;
        if (axvpb) {
            if (c->mode & 0x20000000) {
                c->v = c->v1;
                c->mode &= ~0x20000000;
                setNewInputLevel = 1;
            }
            if (c->mode & 0x10000000) {
                if (c->mode & 4)
                    c->v1 = 0;
                else
                    c->v1 = c->volume;
                c->mode |= 0x20000000;
                setNewInputLevel = 1;
            }
            if (c->mode & 0x80000000) {
                c->vL = c->vL1;
                c->vR = c->vR1;
                c->vS = c->vS1;
                c->vAL = c->vAL1;
                c->vAR = c->vAR1;
                c->vAS = c->vAS1;
                c->vBL = c->vBL1;
                c->vBR = c->vBR1;
                c->vBS = c->vBS1;
                c->mode &= ~0x80000000;
                setNewMixLevel = 1;
            }
            if (c->mode & 0x40000000) {
                c->mode &= ~0x40000000;
                c->mode |= 0x80000000;
                setNewMixLevel = 1;
            }
            if (setNewInputLevel != 0 && axvpb) {
                axvpb->currentVolume = c->v;
                axvpb->currentDelta = (c->v1 - c->v) / 320;
                axvpb->sync |= 0x200;
            }
            if (setNewMixLevel != 0 && axvpb) {
                p = (unsigned short*)&axvpb->mix;
                *(p) = c->vL; p += 1;
                *(p) = ((c->vL1 - c->vL) / 320); p += 1;
                *(p) = c->vR; p += 1;
                *(p) = ((c->vR1 - c->vR) / 320); p += 1;
                *(p) = c->vAL; p += 1;
                *(p) = ((c->vAL1 - c->vAL) / 320); p += 1;
                *(p) = c->vAR; p += 1;
                *(p) = ((c->vAR1 - c->vAR) / 320); p += 1;
                *(p) = c->vBL; p += 1;
                *(p) = ((c->vBL1 - c->vBL) / 320); p += 1;
                *(p) = c->vBR; p += 1;
                *(p) = ((c->vBR1 - c->vBR) / 320); p += 1;
                *(p) = c->vBS; p += 1;
                *(p) = ((c->vBS1 - c->vBS) / 320); p += 1;
                *(p) = c->vS; p += 1;
                *(p) = ((c->vS1 - c->vS) / 320); p += 1;
                *(p) = c->vAS; p += 1;
                *(p) = ((c->vAS1 - c->vAS) / 320); p += 1;
                mixerCtrl = 0;
                if ((axvpb->mix.vAuxAL != 0) || (axvpb->mix.vAuxAR != 0) || (axvpb->mix.vAuxAS != 0))
                    mixerCtrl |= 1;
                if ((axvpb->mix.vAuxBL != 0) || (axvpb->mix.vAuxBR != 0) || (axvpb->mix.vAuxBS != 0))
                    mixerCtrl |= 2;
                if ((axvpb->mix.vS != 0) || (axvpb->mix.vAuxAS != 0) || (axvpb->mix.vAuxBS != 0))
                    mixerCtrl |= 4;
                if ((axvpb->mix.vDeltaL != 0) || (axvpb->mix.vDeltaR != 0) || (axvpb->mix.vDeltaS != 0) ||
                    (axvpb->mix.vDeltaAuxAL != 0) || (axvpb->mix.vDeltaAuxAR != 0) || (axvpb->mix.vDeltaAuxAS != 0) ||
                    (axvpb->mix.vDeltaAuxBL != 0) || (axvpb->mix.vDeltaAuxBR != 0) || (axvpb->mix.vDeltaAuxBS != 0))
                    mixerCtrl |= 8;
                axvpb->mixerCtrl = mixerCtrl;
                axvpb->sync |= 0x12;
            }
        }
    }
}
