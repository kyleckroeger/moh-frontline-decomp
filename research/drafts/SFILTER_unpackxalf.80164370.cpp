// Looping unpacker for EA-XA compressed audio in memory, decoding through
// SND::CEAXABLKDecf: SFILTER_unpackxalf feeds the data up to the loop start
// once, decodes into the output and at the end of the data refeeds from the
// loop start's block, restoring the prediction state saved the first time
// (and discarding the samples before the loop start). UNPACKINITPARAMS and CEAXABLKDecf are named by the mangled
// symbols; the state record (UNPACKXALOOPSTATE, not an original name) and the
// members are inferred from offsets.

namespace SND {
/* inferred: the decoder's prediction state */
struct XAFSTATE {
    float previous;
    float current;
};

class CEAXABLKDecf {
public:
    CEAXABLKDecf();
    void* operator new(unsigned long);
    void operator delete(void*);
    int Decode(float**, int);
    void Feed(void*, int, int);
    XAFSTATE GetState();
    void SetState(XAFSTATE*);

private:
    char m_state[168];
};
}

typedef SND::XAFSTATE XAFSTATE;

struct UNPACKINITPARAMS {
    void* data;
    int field4;
    int field8;
    int length;
    int loopstart;
    int loopend;
    int field18[3];
    int (*getframe)(void*);
};

struct UNPACKXALOOPSTATE {
    int (*unpack)(void*, int, void*, void*, int);
    void (*restore)(void*);
    char field8[20];
    SND::CEAXABLKDecf* decoder;
    void* data;
    int length;
    int position;
    int field2C;
    int field30;
    int loopstart;
    int loopend;
    SND::XAFSTATE loopState;
};

int SFILTER_unpackxalf(void* data, int count, void*, void* out, int) {
    UNPACKXALOOPSTATE* state = (UNPACKXALOOPSTATE*)data;
    float* destination = (float*)out;
    float* buffer;
    int skip;
    int total = 0;
    float discard[28];
    XAFSTATE saved;

    if (!state->field30) {
        int start = state->loopstart / 28 * 28;
        state->decoder->Feed(state->data, start * 4, start);
        state->field30 = 1;
    }
    buffer = discard;
    while (count) {
        int decoded = state->decoder->Decode(&destination, count);
        total += decoded;
        destination += decoded;
        state->position += decoded;
        if (decoded < count) {
            int length;
            count -= decoded;
            skip = state->loopstart % 28;
            length = state->loopend - (state->loopstart - skip) + 1;
            state->position = state->loopstart - skip;
            state->decoder->Feed((char*)state->data + state->position / 28 * 15, length * 4, length);
            if (!state->field2C) {
                saved = state->decoder->GetState();
                state->loopState.previous = saved.previous;
                state->loopState.current = saved.current;
                decoded = state->decoder->Decode(&destination, count);
                total += decoded;
                count -= decoded;
                destination += decoded;
                state->position += decoded;
                state->field2C = 1;
            } else {
                float* skipped;
                saved.previous = state->loopState.previous;
                saved.current = state->loopState.current;
                state->decoder->SetState(&saved);
                skipped = buffer;
                state->position += state->decoder->Decode(&skipped, skip);
            }
        } else {
            count -= decoded;
        }
    }
    return total;
}

int SFILTER_unpackgetframexalf(void* data) {
    return ((UNPACKXALOOPSTATE*)data)->position;
}

void SFILTER_unpackxalfrestore(void* data) {
    if (((UNPACKXALOOPSTATE*)data)->decoder)
        delete ((UNPACKXALOOPSTATE*)data)->decoder;
}

void SFILTER_unpackxalfinit(void* data, UNPACKINITPARAMS* params) {
    UNPACKXALOOPSTATE* state = (UNPACKXALOOPSTATE*)data;

    state->unpack = SFILTER_unpackxalf;
    state->restore = SFILTER_unpackxalfrestore;
    state->data = params->data;
    state->length = params->length;
    state->position = 0;
    state->field2C = 0;
    state->field30 = 0;
    state->loopstart = params->loopstart;
    state->loopend = params->loopend;
    state->decoder = new SND::CEAXABLKDecf;
    params->getframe = SFILTER_unpackgetframexalf;
}
