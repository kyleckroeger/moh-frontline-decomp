// A fragment of stretch.c (0x80166ca0..0x801671b0): stretchframesneeded,
// which counts the input frames the time-stretch filter needs for a number of
// output frames (simulating determineaction over the available frames); stretch, which produces
// the time-stretch filter's output from its pending frames and then from the
// input through determineaction/applyaction, carrying a frame over when the
// last action stretched; SFILTER_timestretchsetratio (the ratio in 1/4096
// steps, clamped to 0.5..2); SFILTER_timestretch, the filter's process
// function (a plain 200-frame pass from the input filter while nothing is
// available, otherwise the frames needed are pulled through the input filter
// unless the voice's packet player is short of them, then stretched, and the
// rest of the output is cleared); and SFILTER_timestretchinit (the header's
// frame size and available count, the voice's packet handle or -1). The
// filter-chain fields (process, input, the byte passed on) follow the other
// SFILTER states (stretchframesneeded's final clamp is written
// 'carried >= needed ? 0 : needed - carried', the form that gives the
// original's compare idiom). Compiled as C++ like the rest of the
// sound library. The names come from the symbols; the state layout is inferred
// and its fields are named by offset.
struct TIMESTRETCHSTATE {
    int (*process)(void*, int, void*, void*, int);
    int field04;
    TIMESTRETCHSTATE* input;
    unsigned char unknown0c[14];
    unsigned char field1a;
    unsigned char field1b;
    unsigned char* m_1c;
    int m_packet;
    float m_24;
    float m_28;
    int m_framesize;
    int m_available;
    int m_carried;
    int m_pending;
    int m_position;
    float m_carry[510];
    float m_output[1];
};

int determineaction(int, float, unsigned char**, float*);
void applyaction(TIMESTRETCHSTATE*, float**, int);
extern "C" void SNDI_memcpy(void*, const void*, int);
extern "C" void* memset(void*, int, unsigned long);
extern "C" int SNDPKTPLAY_framesoutstanding(int);
int SNDDRV_getmastervoice(int);
int SNDPKTPLAYI_voicetopackethandle(int);
int SNDI_getb(void*, int);

int stretchframesneeded(void* handle, int frames) {
    TIMESTRETCHSTATE* state = (TIMESTRETCHSTATE*)handle;
    int consumed;
    int action;
    int produced;
    int needed;
    int available;
    int remaining;
    unsigned char* pointer;
    float value;

    if (frames <= state->m_pending)
        return 0;
    available = state->m_available;
    remaining = frames - state->m_pending;
    needed = 0;
    action = 0;
    pointer = state->m_1c;
    value = state->m_28;
    while (remaining > 0) {
        if (available < state->m_framesize)
            break;
        if (available < state->m_framesize * 2)
            produced = 0;
        else
            produced = determineaction(state->m_framesize, state->m_24, &pointer, &value);
        action = produced;
        if (produced >= 0) {
            consumed = state->m_framesize;
            produced = consumed + produced;
        } else {
            consumed = state->m_framesize * 2;
            produced += consumed;
        }
        needed += consumed;
        available -= consumed;
        if (remaining <= produced)
            remaining = 0;
        else
            remaining -= produced;
    }
    if (action > 0)
        needed += state->m_framesize;
    if (remaining > 0) {
        if (remaining > available)
            needed += available;
        else
            needed += remaining;
    }
    return state->m_carried >= needed ? 0 : needed - state->m_carried;
}

int stretch(void* handle, int frames, float* in, float* out) {
    TIMESTRETCHSTATE* state = (TIMESTRETCHSTATE*)handle;
    int total;
    int count;
    int action;
    float* source;

    total = state->m_pending;
    source = &state->m_output[state->m_position];
    if (frames <= total)
        total = frames;
    SNDI_memcpy(out, source, total * sizeof(float));
    frames -= total;
    state->m_position += total;
    out += total;
    state->m_pending -= total;
    if (frames == 0)
        return total;
    action = 0;
    while (frames > 0) {
        if (state->m_available < state->m_framesize)
            break;
        if (state->m_available < state->m_framesize * 2)
            action = 0;
        else
            action = determineaction(state->m_framesize, state->m_24, &state->m_1c, &state->m_28);
        applyaction(state, &in, action);
        count = state->m_pending;
        source = &state->m_output[state->m_position];
        if (frames <= count)
            count = frames;
        SNDI_memcpy(out, source, count * sizeof(float));
        frames -= count;
        total += count;
        state->m_position += count;
        out += count;
        state->m_pending -= count;
    }
    if (action > 0) {
        SNDI_memcpy(state->m_carry, in, state->m_framesize * sizeof(float));
        state->m_carried = state->m_framesize;
        in += state->m_framesize;
    }
    if (frames > 0) {
        if (frames > state->m_available)
            frames = state->m_available;
        SNDI_memcpy(out, in, frames * sizeof(float));
        total += frames;
        state->m_available -= frames;
    }
    return total;
}

void SFILTER_timestretchsetratio(TIMESTRETCHSTATE* state, int ratio) {
    state->m_24 = (1.0f / 4096.0f) * ratio;
    if (state->m_24 < 0.5)
        state->m_24 = 0.5f;
    else if (state->m_24 > 2.0)
        state->m_24 = 2.0f;
}

int SFILTER_timestretch(void* filter, int count, void* source, void* destination, int) {
    TIMESTRETCHSTATE* state = (TIMESTRETCHSTATE*)filter;
    int needed;
    float* in = (float*)source;
    float* out = (float*)destination;
    int done;

    if (state->m_available == 0)
        return state->input->process(state->input, 200, out, in, state->field1a);
    needed = stretchframesneeded(state, count);
    if (state->m_packet >= 0 && needed > SNDPKTPLAY_framesoutstanding(state->m_packet))
        return 0;
    if (needed > 0) {
        int result = state->input->process(state->input, needed, out, in, state->field1a);
        if (result <= 0)
            return result;
    }
    done = stretch(state, count, in, out);
    if (done < count) {
        out += done;
        memset(out, 0, (count - done) * sizeof(float));
    }
    return count;
}

int SFILTER_timestretchinit(TIMESTRETCHSTATE* state, unsigned char* header, int voice) {
    state->process = SFILTER_timestretch;
    state->field04 = 0;
    if (voice >= 0)
        state->m_packet = SNDPKTPLAYI_voicetopackethandle(SNDDRV_getmastervoice(voice));
    else
        state->m_packet = -1;
    state->m_1c = header + 6;
    state->m_24 = 1.0f;
    state->m_28 = 0.0f;
    state->m_framesize = header[1] << 1;
    state->m_available = SNDI_getb(header + 2, 4);
    state->m_carried = 0;
    state->m_pending = 0;
    state->m_position = 0;
    return 0;
}
