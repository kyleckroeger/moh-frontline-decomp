// The mixer's allocation hooks: SNDI_New takes cleared sound memory and
// SNDI_Delete frees it. The rest of the file is not part of this unit.
void* SNDMEMI_allocz(int);
void SNDMEMI_free(void*);

/* inferred: a float-to-integer stage (FT24_32STATE) as a filter record */
struct FT24_32STATE {
    unsigned char unknown00[24];
    unsigned short priority;
    unsigned char unknown1a[2];
};

/* inferred: the software mixer's state, as far as it is used here */
struct SNDMIXVIEW {
    unsigned int value00;
    unsigned char channels;
    unsigned char outputs;
    unsigned char value06;
    unsigned char value07;
    unsigned int value08;
    unsigned char unknown00c[3];
    signed char value0f;
    unsigned char unknown010[272];
    void* work[2];
    int* alignedWork[2];
    void* mix[4];
    int* alignedMix[4];
    unsigned char unknown150[20];
    void* filters[4];
    unsigned char unknown174[16];
    FT24_32STATE ft[4];
    void* channelRecords;
    void (*scalemix)(int, float, int*, float*);
};


/* inferred: a voice's decoder, called through its first slot */
struct SNDMIXDECODERVIEW {
    int (*decode)(SNDMIXDECODERVIEW*, int, int*, int*, int);
};

/* inferred: the mixer voice, as far as it is used here */
struct SNDMIXVOICE {
    unsigned char unknown00[4];
    float pan[4];
    float targetPan[4];
    float volume;
    float targetVolume;
    float last;
    SNDMIXDECODERVIEW* decoder;
};

void MIXI_interpolatemix(float, float, float*, float*);

extern "C" {
extern SNDMIXVIEW sndmix;
extern float sndfx[];
}

static inline void Ramp(SNDMIXVOICE* voice, SNDMIXVIEW* m) {
    voice->last = ((float*)sndmix.alignedWork[1])[15];
    if (m->value0f) {
        if (voice->volume != voice->targetVolume) {
            MIXI_interpolatemix(voice->volume, voice->targetVolume, (float*)sndmix.alignedWork[1], sndfx);
            voice->volume = voice->targetVolume;
        } else if (0.0f != voice->targetVolume) {
            m->scalemix(16, voice->targetVolume, sndmix.alignedWork[1], sndfx);
        }
    }
    for (int i = 0; i < m->outputs; i++) {
        if (voice->pan[i] != voice->targetPan[i]) {
            MIXI_interpolatemix(voice->pan[i], voice->targetPan[i], (float*)sndmix.alignedWork[1],
                                (float*)m->alignedMix[i]);
            voice->pan[i] = voice->targetPan[i];
        } else if (0.0f != voice->targetPan[i]) {
            m->scalemix(16, voice->targetPan[i], sndmix.alignedWork[1], (float*)m->alignedMix[i]);
        }
    }
}

int SNDMIXI_volramp(SNDMIXVOICE* voice) {
    if (voice->decoder->decode(voice->decoder, 16, sndmix.alignedWork[0], sndmix.alignedWork[1], 0) <= 0) {
        voice->volume = voice->targetVolume;
        for (int i = 0; i < sndmix.outputs; i++)
            voice->pan[i] = voice->targetPan[i];
        return 0;
    }
    Ramp(voice, &sndmix);
    return 16;
}

void* SNDI_New(unsigned long size) {
    return SNDMEMI_allocz(size);
}

void SNDI_Delete(void* memory) {
    SNDMEMI_free(memory);
}
