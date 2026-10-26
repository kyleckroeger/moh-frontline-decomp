/* The echo filter: SFILTER_echo first runs the chained filter (out to in),
   then processes the block in runs up to the end of the circular delay
   buffer, four samples at a time: each output is the delayed sample plus
   the new delay value times the negated gain, and the delay buffer takes
   the input plus the delayed sample times the gain (with a 1e-20
   anti-denormal bias). The run loop is an inline helper that declares its
   pointers src, dst but assigns dst, src (register order).
   The echo filter's state functions, the last of the file: restoring frees
   the delay buffer, creating installs the echo and restore functions and
   clears the state, and modifying sets the feedback gain from the first
   parameter (in 1/127 steps), sizes a 16-byte-aligned delay buffer from the
   other two and clears it (its float constants are entries of the file's
   .sdata2 pool). ECHOSTATE and the functions are named by the
   mangled symbols; the state layout is inferred. */
extern "C" void* memset(void*, int, unsigned long);

void SNDMEMI_free(void*);
void* SNDMEMI_allocz(int);
int SNDI_findmult16(int, int);

struct ECHOSTATE;
struct ECHOSTATE {
    int (*filter)(void*, int, void*, void*, int);
    void (*restore)(void*);
    ECHOSTATE* next;
    int unknown0c;
    int unknown10;
    int unknown14;
    unsigned char unknown18[2];
    unsigned char nextMode;
    unsigned char unknown1b;
    float* buffer;
    void* memory;
    int position;
    int length;
    float gain;
    float negGain;
};

inline void echoblock(ECHOSTATE* state, float* out, float* in, int n) {
    float* src;
    float* dst;
    float* end;
    float* buf;
    dst = out;
    src = in;
    end = dst + n;
    buf = state->buffer + state->position;
    for (; dst < end; dst += 4, src += 4, buf += 4) {
        float v0, v3, v2, v1;
        v0 = 1e-20f + (src[0] + buf[0] * state->gain);
        v1 = 1e-20f + (src[1] + buf[1] * state->gain);
        v2 = 1e-20f + (src[2] + buf[2] * state->gain);
        v3 = 1e-20f + (src[3] + buf[3] * state->gain);
        dst[0] = buf[0] + v0 * state->negGain;
        dst[1] = buf[1] + v1 * state->negGain;
        dst[2] = buf[2] + v2 * state->negGain;
        dst[3] = buf[3] + v3 * state->negGain;
        buf[0] = v0;
        buf[1] = v1;
        buf[2] = v2;
        buf[3] = v3;
    }
}

int SFILTER_echo(void* p, int count, void* in, void* out, int mode) {
    ECHOSTATE* state = (ECHOSTATE*)p;
    int n;
    float* src;
    float* dst;
    int remaining;
    if (state->next) {
        int result = state->next->filter(state->next, count, out, in, state->nextMode);
        if (result <= 0)
            return result;
    }
    remaining = count;
    src = (float*)in;
    dst = (float*)out;
    while (remaining > 0) {
        if (state->position >= state->length)
            state->position = 0;
        n = state->length - state->position;
        if (n > remaining)
            n = remaining;
        echoblock(state, dst, src, n);
        remaining -= n;
        src += n;
        dst += n;
        state->position += n;
    }
    return count;
}

void SFILTER_echorestore(void* p) {
    ECHOSTATE* state = (ECHOSTATE*)p;
    if (state->memory) {
        SNDMEMI_free(state->memory);
        state->memory = 0;
        state->buffer = 0;
    }
}

int SFILTER_createECHO(ECHOSTATE* state) {
    state->next = 0;
    state->unknown0c = 0;
    state->unknown10 = 0;
    state->unknown14 = 0;
    state->unknown1b = 0;
    state->restore = SFILTER_echorestore;
    state->filter = SFILTER_echo;
    state->memory = 0;
    state->buffer = 0;
    return 0;
}

void SFILTER_modifyECHO(ECHOSTATE* state, int* params) {
    int feedback = params[0] >> 1;
    int delay = params[1] >> 8;
    int rate = params[2] >> 8;
    SFILTER_echorestore(state);
    state->gain = (1.0f / 127.0f) * feedback;
    state->negGain = -state->gain;
    state->length = SNDI_findmult16(delay, rate);
    {
        int size = state->length * 4;
        state->memory = SNDMEMI_allocz(size + 16);
        state->buffer = (float*)(((int)state->memory + 15) & ~15);
        memset(state->buffer, 0, size);
    }
    state->position = 0;
}
