/* EA's stagpat.c: SNDBANKI_playtimbre starts one timbre of a patch on the
   first render mode that has a free voice: the voice record is filled from the
   timbre header (pan with optional random spread, pitch from the root key,
   detune and random pitch, volume with random spread scaled by the play
   volume, fine tune, envelope and its first step, random start offsets, the
   play options' fields and the timbre's channel voices), the pitch, effect
   level and volume are calculated and the platform plays it; on failure its
   voices are freed and the next render mode is tried (success ends the
   search). SNDBANKI_playpatch plays a patch: it takes a new patch key that no
   active voice uses, parses the patch's timbres and starts every timbre whose
   volume and key ranges hold the play options (one random detune for the
   whole patch), drops the voices that are already over, tags the remaining
   voices with the patch key and marks the last one; when a timbre fails or
   nothing is left the started voices are stopped. The functions and
   SNDIPATCHHEADER/SNDPLAYOPTS/TAGGEDPATCH are named by the mangled symbols;
   the views (fields named by offset where their meaning is not established)
   are inferred. Locals are declared in the order that gives the original's
   registers; in SNDBANKI_playpatch one function-level 'i' serves the key
   search, the voice tagging and the stop loops (the tagging loop's 'i = 0'
   re-assigns it, so the compiler shares that zero with the stored 0, as in
   the original). */
struct TAGGEDPATCH;

struct SNDIPATCHHEADER {
    short detuneRange;
    unsigned char unknown0002[1];
    signed char voiceCount;
    short pitch;
    short pitchRandom;
    signed char lowVolume;
    signed char highVolume;
    signed char lowKey;
    signed char highKey;
    signed char priority;
    signed char rootKey;
    unsigned char field0e;
    unsigned char field0f;
    signed char fineTune;
    signed char pan;
    signed char panRandom;
    unsigned char field13;
    signed char volume;
    signed char volumeRandom;
    unsigned char field16;
    unsigned char unknown0017[3];
    signed char field1a;
    unsigned char field1b;
    signed char field1c;
    unsigned char field1d;
    unsigned char field1e;
    unsigned char unknown001f[1];
    int field20;
    int field24;
    int* envelope;
    int field2c;
    int field30;
    unsigned char unknown0034[32];
    short field54;
    unsigned short field56;
    int field58;
    unsigned char unknown005c[4];
    int field60;
    unsigned char unknown0064[48];
    unsigned short links[8];
    unsigned char unknown00a4[32];
};

struct SNDVOICEVIEW {
    unsigned char unknown0000[4];
    short links[4];
    short field0c;
    short field0e;
    int field10;
    int field14;
    unsigned short field18;
    short field1a;
    unsigned short field1c;
    unsigned char field1e;
    unsigned char field1f;
    short mode;
    unsigned char patchKey;
    unsigned char lastOfPatch;
    unsigned char unknown0024[8];
    int field2c;
    unsigned char unknown0030[4];
    int field34;
    int envelopeStep;
    int envelopeLevel;
    int envelopeTime;
    unsigned char volume;
    unsigned char unknown0045[1];
    unsigned short azimuth;
    short links2[4];
    unsigned char field50;
    unsigned char field51;
    unsigned char field52;
    unsigned char field53;
    unsigned char field54;
    unsigned char field55;
    unsigned char unknown0056[2];
    unsigned char field58;
    unsigned char field59;
    unsigned char field5a;
    unsigned char field5b;
    unsigned char field5c;
    signed char active;
    unsigned short field5e;
    int* envelope;
    int field64;
    int field68;
    int field6c;
    int field70;
    short field74;
    short fineTune;
    short pitch;
    short field7a;
    unsigned short field7c;
    unsigned char unknown007e[2];
};

struct SNDPLAYOPTS {
    signed char field00;
    signed char field01;
    signed char key;
    signed char volume;
    unsigned char field04;
    signed char field05;
    unsigned char unknown0006[2];
    unsigned short field08;
    short field0a;
    unsigned short field0c;
    unsigned short field0e;
    unsigned char unknown0010[2];
    unsigned short field12;
    unsigned short field14;
    unsigned short field16;
};

struct SNDGSVIEW {
    unsigned char unknown000[368];
    short voiceCount;
    unsigned char unknown172[98];
    SNDVOICEVIEW* voices;
};

extern "C" SNDGSVIEW sndgs;

int SNDI_validrendermode(int*, SNDIPATCHHEADER*);
void SNDPLATFORM_getvoicerange(int, int*, int*);
int SNDVOICEI_alloc(int, int, int*, int, int);
int randrange(int);
unsigned short SNDI_pantoazimuth(int);
unsigned int iSNDrandom();
void iSNDcalcpitch(int);
void SNDI_calcfxlevel(int, int);
void iSNDcalcvol(int);
int SNDPLATFORM_playtimbre(SNDIPATCHHEADER*, void*, int, int, int, int, int);
void SNDVOICEI_free(int);

int SNDBANKI_playtimbre(int handle, int bank, void* data, SNDPLAYOPTS* options, SNDIPATCHHEADER* header, int key,
                        int volume, int detune) {
    int iter;
    int low;
    int high;
    int result;
    int mode;
    int start;
    SNDVOICEVIEW* v;
    int voice;
    int field01;
    int field05;
    int pan;
    int level;
    int i;
    result = -9;
    start = options->field00;
    start <<= 16;
    field01 = options->field01;
    field05 = options->field05;
    iter = 0;
    while (1) {
        mode = SNDI_validrendermode(&iter, header);
        if (!mode)
            break;
        SNDPLATFORM_getvoicerange(mode, &low, &high);
        voice = SNDVOICEI_alloc(header->voiceCount, header->priority, &result, low, high);
        if (voice >= 0) {
            v = &sndgs.voices[voice];
            pan = header->pan;
            if (header->panRandom) {
                pan += randrange(header->panRandom);
                if (pan < 0)
                    pan = 0;
                else if (pan > 127)
                    pan = 127;
            }
            v->azimuth = SNDI_pantoazimuth(pan);
            v->pitch = header->pitch;
            v->pitch -= (header->rootKey - key) * 100;
            if (detune)
                v->pitch += detune;
            if (header->pitchRandom)
                v->pitch += randrange(header->pitchRandom);
            v->field64 = header->field20;
            v->field68 = header->field24;
            v->field2c = 0;
            v->field34 = start;
            level = header->volume;
            if (header->volumeRandom)
                level += randrange(header->volumeRandom);
            if (level > 127)
                level = 127;
            if (level < 0)
                level = 0;
            v->volume = level * volume / 127;
            v->fineTune = header->fineTune * 100;
            v->field58 = field01;
            v->envelope = header->envelope;
            v->envelopeLevel = header->field1a << 16;
            v->field51 = 0;
            v->field50 = header->field0f;
            v->field52 = header->field0e;
            v->envelopeTime = v->envelope[0];
            if (v->envelopeTime < 0)
                v->envelopeTime = 0x7fffffff;
            v->envelopeStep = ((v->envelope[1] << 16) - v->envelopeLevel) / v->envelopeTime;
            v->field6c = header->field2c;
            v->field70 = header->field30;
            v->field59 = header->field1b;
            v->field5a = header->field1d;
            v->field74 = header->field54;
            if (header->field1c)
                v->field5b = iSNDrandom() % header->field1c;
            else
                v->field5b = 0;
            if (header->field1e)
                v->field5c = iSNDrandom() % header->field1d;
            else
                v->field5c = 0;
            v->field5e = options->field0e;
            v->field7c = options->field0c;
            v->field7a = 0;
            iSNDcalcpitch(voice);
            v->field0e = handle;
            v->field0c = bank;
            v->field18 = v->azimuth + options->field08;
            v->field1a = options->field0a;
            v->field14 = header->field60;
            v->field10 = header->field58;
            v->field1c = header->field56;
            v->field1e = header->field13;
            v->field1f = header->voiceCount;
            v->mode = mode;
            for (i = 0; i < header->voiceCount; i++)
                v->links2[i] = header->links[i];
            v->field53 = options->field04;
            v->field54 = header->field16;
            v->field55 = field05;
            SNDI_calcfxlevel(0, voice);
            iSNDcalcvol(voice);
            if (SNDPLATFORM_playtimbre(header, data, voice, options->field0e, options->field12, options->field14,
                                       options->field16) < 0) {
                for (i = 0; i < header->voiceCount; i++)
                    SNDVOICEI_free(sndgs.voices[voice].links[i]);
                result = -9;
            } else
                break;
        }
    }
    return result;
}

int SNDI_parsetimbre(void**, SNDIPATCHHEADER*);
void SNDBANKI_userdatacallback(SNDIPATCHHEADER*, int);
extern "C" int SNDover(int);
extern "C" void SNDstop(int);
int SNDVOICEI_get(int);

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
    int i;
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
        for (i = 0; i < n; i++) {
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
        for (int j = 0; j < started; j++) {
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
        for (i = 0; i < count; i++) {
            int index = SNDVOICEI_get(*h);
            if (index >= 0) {
                voice = &gs->voices[index];
                voice->patchKey = patchKey;
                voice->lastOfPatch = 0;
            }
            h++;
        }
        voice->lastOfPatch = 1;
    }
    return result;
stop:
    for (i = 0; i < count; i++) {
        SNDstop(*h);
        h++;
    }
    return result;
}
