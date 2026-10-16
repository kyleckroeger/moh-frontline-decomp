/* EA's resonance filter (sfreson.c, the whole file): resonx87 runs the
   two-pole resonator over the samples (output = coefficient2 * input plus
   the feedback of the last two outputs, a 1e-20 bias keeping the input out
   of denormals) when the bandwidth, frequency and rate are usable, and
   copies the input otherwise. Processing runs an optional source plug-in
   first (stopping if it produces nothing) and then the resonator over the
   samples, and creating installs the process function and clears the state
   and filter history. SFILTER_modifyRESON sets the resonator from the
   frequency, rate, bandwidth and gain parameters (radius 1 - pi *
   bandwidth / rate, the two coefficients through SNDI_cos and
   SNDI_rootof1plusx). The file is compiled with -inline on (no automatic
   inlining), as SFILTER_reson calls resonx87 out of line. RESONSTATE and
   the functions are named by the mangled symbols; the state layout and the
   plug-in view are inferred. */
struct SFILTERPLUGINVIEW {
    int (*process)(SFILTERPLUGINVIEW*, int, void*, void*, int);
};

struct RESONSTATE {
    int (*filter)(void*, int, void*, void*, int);
    void (*restore)(void*);
    SFILTERPLUGINVIEW* plugin;
    int unknown0c;
    int unknown10;
    int unknown14;
    unsigned char unknown18[2];
    unsigned char pluginFlags;
    unsigned char unknown1b;
    int rate;
    float frequency;
    float bandwidth;
    float radius;
    float coefficient1;
    float coefficient2;
    float history[3];
};

float SNDI_cos(float);
float SNDI_rootof1plusx(float);

void resonx87(RESONSTATE* state, int count, void* source, void* destination) {
    float* in = (float*)source;
    float* out = (float*)destination;
    int i;

    if (state->bandwidth < 2.0f * state->rate / 3.1415927f && state->frequency > 0.0f &&
        state->frequency < (float)(state->rate >> 1)) {
        for (i = 0; i < count; i++) {
            out[i] = state->coefficient2 * (1e-20f + in[i]) +
                     state->history[1] * (2.0f * state->radius * state->coefficient1) -
                     state->history[2] * (state->radius * state->radius);
            state->history[2] = state->history[1];
            state->history[1] = out[i];
        }
    } else {
        for (i = 0; i < count; i++)
            out[i] = in[i];
    }
}

int SFILTER_reson(void* p, int count, void* in, void* out, int) {
    RESONSTATE* state = (RESONSTATE*)p;
    int result;
    if (state->plugin) {
        result = state->plugin->process(state->plugin, count, out, in, state->pluginFlags);
        if (result <= 0)
            return result;
    } else {
        result = count;
    }
    resonx87(state, result, in, out);
    return result;
}

int SFILTER_createRESON(RESONSTATE* state) {
    state->plugin = 0;
    state->unknown0c = 0;
    state->unknown10 = 0;
    state->unknown14 = 0;
    state->unknown1b = 0;
    state->restore = 0;
    state->filter = SFILTER_reson;
    state->history[0] = 0.0f;
    state->history[1] = 0.0f;
    state->history[2] = 0.0f;
    return 0;
}

void SFILTER_modifyRESON(RESONSTATE* state, int* params) {
    int frequency = params[0] >> 8;
    int rate = params[1] >> 8;
    int bandwidth = params[2] >> 8;
    int gain = params[3];
    float scale = 0.00390625f * gain;
    state->rate = rate;
    state->frequency = frequency;
    state->bandwidth = bandwidth;
    state->radius = 1.0f - 3.1415927f * bandwidth / rate;
    float radiusSq = state->radius * state->radius;
    state->coefficient1 = 2.0f * state->radius / (1.0f + radiusSq) * SNDI_cos(6.2831855f * frequency / rate);
    state->coefficient2 = (1.0f - radiusSq) * scale * SNDI_rootof1plusx(-state->coefficient1 * state->coefficient1);
}
