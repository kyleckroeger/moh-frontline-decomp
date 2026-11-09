// A fragment of the software mixer (smixer.c, 0x801609d0): MIX_audioslice
// mixes one slice. It fades out the effect send and the output gains that
// are not yet zero, clears the output mix buffers, then for every playing
// channel applies a pending volume ramp, runs the channel's filter chain into
// the work buffer (stopping the channel and calling the stop hook on an
// error), keeps the last sample, and adds the result to each output with a
// non-zero dry gain and to the effect send; finally it runs the post hook and
// each output's filter chain (through its effect stage when it has one) into
// the caller's buffers. sndmix, sndfx and the functions are named by their
// symbols; the mixer, channel and filter views are inferred (see
// smixer_create.c and sfilter.c), and 0.0f is an entry of the file's .sdata2
// pool.
/* inferred: a filter stage's process entry, as in sfilter.c */
struct SFILTERVIEW {
    int (*process)(SFILTERVIEW*, int, void*, void*, int);
};

/* inferred: a mixer channel record (80 bytes) */
struct SNDMIXVOICE {
    unsigned char state;
    signed char dirty;
    unsigned char unknown02[18];
    float dry[5];
    float wet;
    float last;
    SFILTERVIEW* filters;
    unsigned char unknown34[28];
};

typedef void (*SNDMIXFN)(int, float*, float*, float);

/* inferred: the effect send buffer and its active flag */
struct SNDFXVIEW {
    float samples[480];
    int active;
};

/* inferred: the software mixer's state, as far as it is used here */
struct SNDMIXVIEW {
    unsigned char unknown000[4];
    unsigned char channels;
    unsigned char outputs;
    unsigned char unknown006[2];
    void (*stop)(int);
    unsigned char unknown00c[3];
    signed char effects;
    unsigned char unknown010[4];
    void (*post)(int);
    unsigned char unknown018[272];
    float* work[2];
    unsigned char unknown130[16];
    float* mix[4];
    float gain[4];
    float effectGain;
    SFILTERVIEW* filters[4];
    SFILTERVIEW* effectFilters[4];
    unsigned char unknown184[112];
    SNDMIXVOICE* voices;
    SNDMIXFN mixfn;
};

int SNDMIXI_volramp(SNDMIXVOICE*);
void MIXI_interpolateto0(float*, float*);

extern "C" {
extern SNDMIXVIEW sndmix;
extern struct SNDFXVIEW sndfx;
void* memset(void*, int, unsigned long);
void MIX_stop(int);
}

extern "C" void MIX_audioslice(float** out, int samples) {
    SNDMIXVOICE* voice;
    SNDMIXVIEW* m;
    int i;
    int count;
    int ramp;
    int remaining;
    int channel;

    m = &sndmix;

    {
        float* buf = sndfx.samples;
        if (m->effects && m->effectGain != 0.0f)
            MIXI_interpolateto0(&m->effectGain, buf);
    }
    for (i = 0; i < sndmix.outputs; i++) {
        memset(m->mix[i], 0, samples * 4);
        if (m->gain[i] != 0.0f)
            MIXI_interpolateto0(&m->gain[i], m->mix[i]);
    }
    for (channel = 0; channel < sndmix.channels; channel++) {
        voice = &sndmix.voices[channel];

        if (voice->state != 2)
            continue;
        if (voice->dirty) {
            ramp = SNDMIXI_volramp(voice);
            voice->dirty = 0;
            remaining = samples - ramp;
        } else {
            ramp = 0;
            remaining = samples;
        }
        if (!remaining)
            continue;
        count = voice->filters->process(voice->filters, remaining, sndmix.work[0], sndmix.work[1], 0);
        if (count < 0) {
            MIX_stop(channel);
            sndmix.stop(channel);
            continue;
        }
        if (!count)
            continue;
        voice->last = sndmix.work[1][count - 1];
        for (i = 0; i < m->outputs; i++) {
            if (0.0f != voice->dry[i])
                sndmix.mixfn(count, sndmix.work[1], m->mix[i] + ramp, voice->dry[i]);
        }
        {
            float* const dst = sndfx.samples + ramp;
            if (sndmix.effects && voice->wet != 0.0f) {
                sndmix.mixfn(count, sndmix.work[1], dst, voice->wet);
                sndfx.active = 0;
            }
        }
    }
    if (sndmix.post)
        sndmix.post(samples);
    for (i = 0; i < sndmix.outputs; i++) {
        if (m->effectFilters[i]) {
            m->effectFilters[i]->process(m->effectFilters[i], samples, m->mix[i], sndmix.work[0], 0);
            m->filters[i]->process(m->filters[i], samples, sndmix.work[0], out[i], 0);
        } else {
            m->filters[i]->process(m->filters[i], samples, m->mix[i], out[i], 0);
        }
    }
}
