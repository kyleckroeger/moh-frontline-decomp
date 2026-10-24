// A fragment of the DSP mixer (sdspmix.c, 0x8016d2e8): releasing a voice's
// mixer channel (clearing its voice) and setting its volume (flagging the
// volume for the next update), then SNDDRV_DSPMixerUpdate, EA's version of
// the SDK's MIXUpdateSettings (mix.c): for every channel with a voice it
// moves pending input and mix levels to current, takes the channel volume
// (or zero when muted) as the new input level, and writes the AX voice's
// volume envelope and mix levels with per-frame deltas (/320, not the SDK's
// /160) and the mixer-control bits. _AXVPB is named by the mangled symbols;
// the channel record and AX voice members follow the SDK's mix.c names where
// the code matches them and are otherwise inferred (see sdspmix.c).
// __MIXChannel is file-static in the original.
// Compiled with GC/1.3 like sdspmix.c.
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

// File-local in the original (defined in sdspmix.c's part of the file).
extern MIXCHANNEL __MIXChannel[64];

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
