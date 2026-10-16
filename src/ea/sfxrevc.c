/* EA's reverb set-up file (sfxrevc.c) from 0x80167ffc to its end.
   SNDMIXI_initfx2 copies a version-2 effect file's header, finds the
   highest output channel count it has filters for (from the mixer's output
   count down), walks past the filter modules (each sized by its type
   through filtermodsizetable) and entries of the lower channel counts to
   find the module and connection lists of that count, and then runs the
   effect set-up. findprime returns the first prime at or above a delay
   length (rate * milliseconds / 1000, at least 3), testing divisors up to
   the root found by a search. MIX_initreverb restores the reverb, installs
   the effect callbacks and either hands a version-10 effect file to
   SNDMIXI_initfx2 or sets up the three groups of filter-bank entries (gains
   from signed bytes / 127 scaled by (level + 100) / 350, delay lines of a
   prime length allocated and cleared). MIX_restorereverb clears the
   mixer's reverb state, restores the second-generation effects when they
   were initialised, frees the eleven filter-bank buffers and clears the
   reverb flag. SNDMIXI_initfx before them is not part of the unit. The
   file is compiled with -inline on (no automatic inlining), as MIX_initreverb
   calls findprime out of line. The header, mixer and filter-bank views are
   inferred; the effect globals and callbacks belong to sdfx.c. */
struct MIXFXFILTHEADER;

/* inferred view of a version-2 fx header: a filter count and an entry count
   per output channel count */
struct MIXFXHEADERVIEW {
    unsigned char unknown00[8];
    int filtercount[6];
    int entrycount[6];
};

/* inferred view of a reverb header and its 4-byte filter-bank entries */
struct REVERBENTRYVIEW {
    unsigned char delay[2];
    signed char gain;
    signed char mix;
};

struct REVERBHEADERVIEW {
    unsigned char unknown00[2];
    unsigned char version;
    unsigned char unknown03;
    unsigned char counts[3];
    unsigned char level;
    REVERBENTRYVIEW entries[1];
};

struct SNDMIXVIEW {
    unsigned char unknown000[5];
    unsigned char outputs;
    unsigned char unknown006[9];
    unsigned char reverbOn;
    void* reverb0;
    void* reverb1;
    unsigned char unknown018[488];
};

/* inferred: a 36-byte filter-bank entry (a delay line and its gains) */
struct SNDFBENTRYVIEW {
    int position;
    int length;
    int length2;
    int* buffer;
    float gain;
    float negativeGain;
    float state;
    float mix;
    float dry;
};

extern "C" {
extern SNDMIXVIEW sndmix;
void SNDI_memcpy(void*, const void*, int);
void* memset(void*, int, unsigned long);
void SNDMIXI_fxinit(MIXFXFILTHEADER*);
void SNDMIXI_fxadd();
void SNDMIXI_modlapifxadd();
}
int SNDI_getb(void*, int);
void SNDMIXI_initfx(int);
void SNDMIXI_restorefx2();
void* SNDMEMI_allocz(int);
void SNDMEMI_free(void*);
extern "C" void MIX_restorereverb();

extern MIXFXHEADERVIEW filehead;
extern int filtermodsizetable[];
extern int lastoutputchannels;
extern int validstructoutputs;
extern unsigned char* modlist;
extern unsigned char* conlist;
extern int SNDMIXfxv2inited;
extern SNDFBENTRYVIEW sndfb[11];
extern int sndfxnummodules[3];

void SNDMIXI_initfx2(int which, MIXFXFILTHEADER* header) {
    int count;
    int j;
    unsigned char* p;
    int i;
    int last;

    SNDMIXfxv2inited = 1;
    SNDI_memcpy(&filehead, header, 56);
    p = (unsigned char*)header + 56;
    last = sndmix.outputs - 1;
    if (SNDI_getb(&filehead.filtercount[last], 4)) {
        validstructoutputs = sndmix.outputs;
    } else {
        while (!SNDI_getb(&filehead.filtercount[last], 4))
            last--;
        validstructoutputs = last + 1;
    }
    lastoutputchannels = validstructoutputs;
    for (i = 0; i < last; i++) {
        count = SNDI_getb(&filehead.filtercount[i], 4);
        for (j = 0; j < count; j++)
            p += filtermodsizetable[*p];
        p += SNDI_getb(&filehead.entrycount[i], 4) * 12;
    }
    modlist = p;
    count = SNDI_getb(&filehead.filtercount[last], 4);
    for (j = 0; j < count; j++)
        p += filtermodsizetable[*p];
    conlist = p;
    SNDMIXI_initfx(which);
}

int findprime(int rate, int ms) {
    int n = ms * rate / 1000;
    int root;
    int i;
    int limit;
    int product;

    if (n < 3)
        n = 3;
restart:
    i = 0;
    do {
        root = i;
        i++;
        product = i * root;
        if (product == n) {
            i--;
            goto found;
        }
    } while (product <= n);
    i -= 2;
found:
    limit = i + 1;
    for (i = 2; i <= limit; i++) {
        if (n % i == 0) {
            n++;
            goto restart;
        }
        if (i == limit)
            return n;
    }
    goto restart;
}

extern "C" void MIX_initreverb(int rate, MIXFXFILTHEADER* data) {
    REVERBHEADERVIEW* header = (REVERBHEADERVIEW*)data;
    REVERBENTRYVIEW* entries;
    REVERBENTRYVIEW* entry;
    SNDFBENTRYVIEW* fb;
    int i;
    int group;
    int n;
    int length;
    int delay;

    MIX_restorereverb();
    sndmix.reverb0 = (void*)SNDMIXI_fxinit;
    if (header->version == 0) {
        sndmix.reverb1 = (void*)SNDMIXI_fxadd;
    } else if (header->version == 10) {
        SNDMIXI_initfx2(rate, data);
        sndmix.reverb1 = (void*)SNDMIXI_modlapifxadd;
        sndmix.reverbOn = 1;
        return;
    }
    entries = header->entries;
    n = 0;
    for (group = 0; group < 3; group++) {
        sndfxnummodules[group] = header->counts[group];
        for (i = 0; i < sndfxnummodules[group]; i++) {
            entry = &entries[n];
            fb = &sndfb[n];
            fb->mix = (float)entry->mix / 127.0f;
            fb->dry = 1.0f - fb->mix;
            fb->mix *= (float)(header->level + 100) / 350.0f;
            fb->dry *= (float)(header->level + 100) / 350.0f;
            fb->gain = (float)entry->gain / 127.0f;
            fb->negativeGain = -fb->gain;
            delay = SNDI_getb(entry->delay, 2);
            length = findprime(rate, delay);
            fb->buffer = (int*)SNDMEMI_allocz(length * 4);
            memset(fb->buffer, 0, length * 4);
            fb->length2 = length;
            fb->length = length;
            fb->position = 0;
            fb->state = 0.0f;
            n++;
        }
    }
    SNDMIXI_fxinit(data);
    sndmix.reverbOn = 2;
}

extern "C" void MIX_restorereverb() {
    int i;
    sndmix.reverb0 = 0;
    sndmix.reverb1 = 0;
    if (SNDMIXfxv2inited)
        SNDMIXI_restorefx2();
    for (i = 0; i < 11; i++) {
        if (sndfb[i].buffer) {
            SNDMEMI_free(sndfb[i].buffer);
            sndfb[i].buffer = 0;
        }
    }
    sndmix.reverbOn = 0;
}
