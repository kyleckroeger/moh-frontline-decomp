// XA-ADPCM unpack filter (float output). Field meanings beyond the code are
// not established; the decoder class is declared only as far as it is used.
namespace SND {
class CEAXABLKDecf {
public:
    static void* operator new(unsigned long);
    static void operator delete(void*);
    CEAXABLKDecf();
    int Decode(float**, int);
    void Feed(void*, int, int);

private:
    // Only the size (168 bytes, from the allocation) is established.
    unsigned char unknown00[168];
};
}

struct UNPACKINITPARAMS {
    void* data;
    int field04;
    int field08;
    int frames;
    unsigned char unknown10[20];
    int (*getframe)(void*);
};

struct UNPACKXAFSTATE {
    int (*process)(void*, int, void*, void*, int);
    void (*restore)(void*);
    unsigned char unknown08[20];
    SND::CEAXABLKDecf* decoder;
    void* data;
    int frames;
    int frame;
};

extern "C" void* memset(void*, int, unsigned long);


int SFILTER_unpackxaf(void* filter, int count, void*, void* destination, int) {
    UNPACKXAFSTATE* state = static_cast<UNPACKXAFSTATE*>(filter);
    float* out = static_cast<float*>(destination);
    if (state->frame >= state->frames) return -1;
    int decoded;
    state->frame += (decoded = state->decoder->Decode(&out, count));
    out += decoded;
    if (decoded < count) { count -= decoded; memset(out, 0, count * sizeof(float)); decoded += count; }
    return decoded;
}

int SFILTER_unpackgetframexaf(void* filter) {
    return static_cast<UNPACKXAFSTATE*>(filter)->frame;
}

void SFILTER_unpackxafrestore(void* filter) {
    UNPACKXAFSTATE* state = static_cast<UNPACKXAFSTATE*>(filter);
    if (state->decoder)
        delete state->decoder;
}

void SFILTER_unpackxafinit(void* filter, UNPACKINITPARAMS* params) {
    UNPACKXAFSTATE* state = static_cast<UNPACKXAFSTATE*>(filter);
    state->process = SFILTER_unpackxaf;
    state->restore = SFILTER_unpackxafrestore;
    state->data = params->data;
    state->frames = params->frames;
    state->frame = 0;
    state->decoder = new SND::CEAXABLKDecf;
    params->getframe = SFILTER_unpackgetframexaf;
    state->decoder->Feed(state->data, state->frames * 4, state->frames);
}
