// The start of stretch.c (0x801665d4): the time-stretch filter's file-local
// helpers. crossfade blends two frame buffers into an output with a weight
// stepping by 1/n (down from 1 for a positive shift, up from 0 for a
// negative one, after skipping |shift| frames); determineaction picks, from
// the stored error and the ratio, whether to drop or repeat a frame-sized
// overlap (the offset comes from the next byte of the action stream) or to
// pass the frame through; applyaction performs it on the frame (or the
// carried frame), filling the output buffer and updating the frame counts.
// stretchframesneeded and the rest of the file are in src/ea/stretch.c. The
// file is compiled with -inline on, as the original calls these helpers out
// of line. Compiled as C++ like the rest of the sound library. The names
// come from the symbols; the state layout follows stretch.c and is inferred.
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

extern "C" void SNDI_memcpy(void*, const void*, int);

static void crossfade(float* a, float* b, float* out, int n, int shift) {
    float weight;
    float step;
    int i;

    if (shift > 0) {
        step = -1.0f / n;
        weight = 1.0f;
    } else {
        shift = -shift;
        step = 1.0f / n;
        weight = 0.0f;
    }
    for (i = shift; i < n; i++) {
        out[i - shift] = weight * a[i] + (1.0f - weight) * a[i - shift];
        weight += step;
    }
    for (i = 0; i < shift; i++) {
        out[n - shift + i] = weight * b[i] + (1.0f - weight) * a[n - shift + i];
        weight += step;
    }
}

static int determineaction(int framesize, float ratio, unsigned char** pointer, float* value) {
    int offset = **pointer + framesize / 2;

    if (ratio < 1.0f) {
        float b = ((float)framesize + *value) - ratio * (float)framesize;
        float a = (*value + (float)(framesize * 2 - offset)) - (float)(2.0f * ratio) * (float)framesize;

        if ((b < 0.0f ? -b : b) < (a < 0.0f ? -a : a)) {
            (*pointer)++;
            *value = b;
            return 0;
        } else {
            *pointer += 2;
            *value = a;
            return -offset;
        }
    } else {
        float b = ((float)framesize + *value) - ratio * (float)framesize;
        float a = (*value + (float)(framesize + offset)) - ratio * (float)framesize;

        if ((b < 0.0f ? -b : b) < (a < 0.0f ? -a : a)) {
            (*pointer)++;
            *value = b;
            return 0;
        } else {
            (*pointer)++;
            *value = a;
            return offset;
        }
    }
}

static void applyaction(TIMESTRETCHSTATE* state, float** in, int action) {
    float* first;
    float* second;

    if (state->m_carried > 0) {
        second = *in;
        first = state->m_carry;
    } else {
        first = *in;
        second = first + state->m_framesize;
    }
    if (action == 0) {
        SNDI_memcpy(state->m_output, first, state->m_framesize * sizeof(float));
        state->m_available -= state->m_framesize;
        state->m_pending = state->m_framesize;
        *in = second;
    } else if (action > 0) {
        SNDI_memcpy(state->m_output, first, action * sizeof(float));
        crossfade(first, second, &state->m_output[action], state->m_framesize, action);
        state->m_available -= state->m_framesize;
        state->m_pending = state->m_framesize + action;
        *in = second;
    } else {
        crossfade(first, second, state->m_output, state->m_framesize, action);
        SNDI_memcpy(&state->m_output[state->m_framesize], second - action, (state->m_framesize + action) * sizeof(float));
        state->m_available -= state->m_framesize * 2;
        state->m_pending = action + state->m_framesize * 2;
        *in = second + state->m_framesize;
    }
    state->m_carried = 0;
    state->m_position = 0;
}
