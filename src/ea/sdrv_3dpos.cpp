/* A fragment of the sound driver (snddrv.c, 0x8016bdfc): SNDPLATFORM_set3dpos
   recomputes a voice's channel levels from its azimuth (both levels at
   0x6000 for mono output) and applies them: through the DSP mixer's pan for
   hardware voices (in three-speaker mode, first easing the front levels of
   channels whose second level exceeds 15000), else through the volume.
   SNDPLATFORM_setpitch then sets a voice's pitch from its rate over the
   output rate: as a source ratio on each hardware channel's AX voice (-8
   for a channel without one), else through the mixer's pitch hook (the
   mixer channel is relative to the channel base). snddrv and sndgs
   are named by their symbols; their views and the voice and
   channel records are inferred (as in sdrv_destroy.cpp), and the constants
   are entries of the file's .sdata2 pool. */
struct _AXVPB;

/* inferred: a driver channel record (48 bytes): output levels, AX voice */
struct SNDDRVCHANNELVIEW {
    short levels[22];
    _AXVPB* voice;
};

struct SNDDRVVIEW {
    SNDDRVCHANNELVIEW channels[1];
};

/* inferred: a voice record (128 bytes) */
struct SNDVOICEVIEW {
    unsigned char unknown00[4];
    short channels[10];
    unsigned short azimuth;
    unsigned char unknown1a[2];
    unsigned short rate;
    unsigned char unknown1e;
    unsigned char channelCount;
    unsigned short flags;
    unsigned char unknown22[38];
    unsigned short azimuths[27];
    unsigned short pitch;
};

struct SNDGSVIEW {
    unsigned char unknown000[42];
    unsigned short sampleRate;
    unsigned char unknown02c[7];
    unsigned char channelBase;
    unsigned char unknown034[20];
    unsigned char outputs;
    unsigned char unknown049[395];
    SNDVOICEVIEW* voices;
};

extern SNDGSVIEW sndgs;
extern SNDDRVVIEW snddrv;

void SNDPLATFORM_setvol(int);
void SNDDRV_DSPMixerSetPan(_AXVPB*, unsigned short*);
void SNDI_aztospkrvol(int, short*);

void SNDPLATFORM_set3dpos(int index) {
    SNDVOICEVIEW* voice = &sndgs.voices[index];
    SNDDRVCHANNELVIEW* own = &snddrv.channels[index];
    int i;

    if (sndgs.outputs == 1) {
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
        if (sndgs.outputs == 3) {
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
    SNDVOICEVIEW* voice = &sndgs.voices[index];
    unsigned int pitch;
    float ratio;
    int i;

    if (voice->flags & 0x200) {
        pitch = (voice->rate << 12) / sndgs.sampleRate;
        pitch = (pitch * voice->pitch) >> 12;
        ratio = 1.0f / 4096.0f * (float)pitch;
        for (i = 0; i < voice->channelCount; i++) {
            if (!snddrv.channels[voice->channels[i]].voice)
                return -8;
            AXSetVoiceSrcRatio(snddrv.channels[voice->channels[i]].voice, ratio);
        }
    } else {
        pitch = (voice->rate << 15) / sndgs.sampleRate;
        pitch = (pitch * voice->pitch) >> 11;
        for (i = 0; i < voice->channelCount; i++)
            MIXsetpitchfn(voice->channels[i] - sndgs.channelBase, pitch);
    }
    return 0;
}
