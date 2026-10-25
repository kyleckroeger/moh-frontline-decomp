/* A fragment of EA's stagpat.c (0x8015ee60): SNDBANKI_playpatch plays a
   patch: it takes a new patch key that no active voice uses, parses the
   patch's timbres and starts every timbre whose volume and key ranges hold
   the play options (one random detune for the whole patch), drops the
   voices that are already over, tags the remaining voices with the patch key
   and marks the last one; when a timbre fails or nothing is left the started
   voices are stopped. The function and SNDIPATCHHEADER/SNDPLAYOPTS/
   TAGGEDPATCH are named by the mangled symbol; the views and the voice
   tagging inline are inferred. Locals are declared in the order that gives
   the original's registers. SNDBANKI_playtimbre before it is in stagpat. */
struct TAGGEDPATCH;

struct SNDIPATCHHEADER {
    short detuneRange;
    unsigned char unknown0002[6];
    signed char lowVolume;
    signed char highVolume;
    signed char lowKey;
    signed char highKey;
    unsigned char unknown000c[184];
};

struct SNDVOICEVIEW {
    unsigned char unknown0000[34];
    unsigned char patchKey;
    unsigned char lastOfPatch;
    unsigned char unknown0024[57];
    signed char active;
    unsigned char unknown005e[34];
};

struct SNDPLAYOPTS {
    unsigned char unknown0000[2];
    signed char key;
    signed char volume;
};

struct SNDGSVIEW {
    unsigned char unknown000[368];
    short voiceCount;
    unsigned char unknown172[98];
    SNDVOICEVIEW* voices;
};

extern "C" SNDGSVIEW sndgs;

int SNDI_parsetimbre(void**, SNDIPATCHHEADER*);
int randrange(int);
int SNDBANKI_playtimbre(int, int, void*, SNDPLAYOPTS*, SNDIPATCHHEADER*, int, int, int);
void SNDBANKI_userdatacallback(SNDIPATCHHEADER*, int);
extern "C" int SNDover(int);
extern "C" void SNDstop(int);
int SNDVOICEI_get(int);

/* inferred: tags the started voices with the patch key; the caller marks the
   last one */
static inline void markvoices(SNDGSVIEW* gs, int* h, int count, unsigned char patchKey, SNDVOICEVIEW*& voice) {
    int i = 0;
    unsigned char last = 0;

    for (; i < count; i++) {
        int index = SNDVOICEI_get(*h);
        if (index >= 0) {
            voice = &gs->voices[index];
            voice->patchKey = patchKey;
            voice->lastOfPatch = last;
        }
        h++;
    }
}

int SNDBANKI_playpatch(void* data, TAGGEDPATCH* patch, int handle, int bank, SNDPLAYOPTS* options) {
    int* out;
    unsigned char patchKey;
    int* h;
    void* p = patch;
    SNDVOICEVIEW* voice = 0;
    int count = 0;
    int randomized = 0;
    int detune = 0;
    int volume;
    int key;
    int result = -9;
    int more;
    SNDGSVIEW* gs;
    int handles[12];
    SNDIPATCHHEADER header;

    if (!patch)
        return -8;
    if (!data)
        data = patch;
    key = options->key;
    volume = options->volume;
    p = (char*)patch + 4;
    static unsigned char freekey = 0;
    gs = &sndgs;
    SNDVOICEVIEW* voices = gs->voices;
    int n = gs->voiceCount;
retry:
    if (++freekey == 0)
        ++freekey;
    patchKey = freekey;
    {
        SNDVOICEVIEW* v = voices;
        for (int i = 0; i < n; i++) {
            if (v->active && v->patchKey == patchKey)
                goto retry;
            v++;
        }
    }
    h = handles;
    out = h;
    do {
        more = SNDI_parsetimbre(&p, &header);
        if (!randomized) {
            if (header.detuneRange)
                detune = randrange(header.detuneRange);
            randomized = 1;
        }
        if (volume >= header.lowVolume && volume <= header.highVolume && key >= header.lowKey && key <= header.highKey) {
            result = SNDBANKI_playtimbre(handle, bank, data, options, &header, key, volume, detune);
            if (result < 0)
                goto stop;
            *out++ = result;
            count++;
            SNDBANKI_userdatacallback(&header, 1);
        }
    } while (more);
    {
        int* src = h;
        int* dst = h;
        int started = count;
        count = 0;
        for (int i = 0; i < started; i++) {
            if (!SNDover(*src)) {
                count++;
                *dst++ = *src;
            }
            src++;
        }
    }
    if (count == 0)
        goto stop;
    if (count == 1) {
        int index = SNDVOICEI_get(handles[0]);
        if (index >= 0) {
            SNDVOICEVIEW* single = &gs->voices[index];
            single->patchKey = 0;
            single->lastOfPatch = 0;
        }
    } else {
        markvoices(gs, h, count, patchKey, voice);
        voice->lastOfPatch = 1;
    }
    return result;
stop:
    for (int i = 0; i < count; i++) {
        SNDstop(*h);
        h++;
    }
    return result;
}
