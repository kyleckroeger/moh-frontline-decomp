/* SFILTER_rsfsetpitch and SFILTER_rsf, the first functions of the resampling
   filter file. SFILTER_rsf runs the filter on a block: at pitch 0x10000
   (16.16 fixed point) it passes the source's samples through after the four
   history samples, otherwise it reads the source samples the pitch needs
   (after the history, or after zero samples on the first call), resamples
   them with the state's interpolation function (rsflc, set by
   SFILTER_rsfinit) and keeps the 16.16 position's fraction; both paths keep
   the last four input samples as the next block's history, and clear the
   history when the source runs dry. RSFSTATE, SFILTER_rsf and
   SFILTER_rsfsetpitch are named by the mangled symbols; the state layout,
   the source record (a read function at +0) and the interpolation
   function's signature are inferred. The mask is written as a long long
   literal to reproduce the original's unfolded mask. */
struct SNDSOURCE {
    int (*read)(SNDSOURCE*, int, float*, float*, int);
};

struct RSFSTATE {
    int (*process)(void*, int, void*, void*, int);
    int field04;
    SNDSOURCE* source;
    unsigned char unknown0c[14];
    unsigned char flag;
    unsigned char unknown1b;
    int pitch;
    unsigned int position;
    void (*interpolate)(int, float*, float*, int*, unsigned int*, unsigned int, unsigned int);
    short started;
    short lead;
    float history[4];
};

void SFILTER_rsfsetpitch(RSFSTATE* state, int pitch) {
    state->pitch = pitch;
}

int SFILTER_rsf(void* filter, int count, void* buffer, void* scratch, int) {
    RSFSTATE* state = (RSFSTATE*)filter;
    float* in = (float*)buffer;
    float* out = (float*)scratch;
    int i;
    int got = count;

    if (state->pitch == 0x10000) {
        int need = count;
        if (!state->started) {
            need += 4 - state->lead;
        } else {
            for (i = 0; i < 4 - state->lead; i++)
                *out++ = state->history[i + state->lead];
        }
        if (state->source)
            got = state->source->read(state->source, need, in, out, state->flag);
        if (got <= 0) {
            if (!state->started)
                state->started = 1;
            state->history[0] = 0.0f;
            state->history[1] = 0.0f;
            state->history[2] = 0.0f;
            state->history[3] = 0.0f;
            return got;
        }
        float* last;
        if (!state->started) {
            state->started = 1;
            last = out + (count - state->lead);
        } else {
            last = out + (count - 4);
        }
        state->history[0] = last[0];
        state->history[1] = last[1];
        state->history[2] = last[2];
        state->history[3] = last[3];
        return count;
    } else {
        int need = (state->position + state->pitch * count) >> 16;
        if (!state->started) {
            need += 4 - state->lead;
            for (i = 0; i < state->lead; i++)
                *in++ = 0.0f;
        } else {
            in[0] = state->history[0];
            in[1] = state->history[1];
            in[2] = state->history[2];
            in[3] = state->history[3];
            in += 4;
        }
        if (need > 0 && state->source) {
            got = state->source->read(state->source, need, out, in, state->flag);
            if (got <= 0) {
                if (!state->started)
                    state->started = 1;
                state->history[0] = 0.0f;
                state->history[1] = 0.0f;
                state->history[2] = 0.0f;
                state->history[3] = 0.0f;
                return got;
            }
        }
        if (state->started)
            in -= 4 - state->lead;
        else
            state->started = 1;
        unsigned int pitch = state->pitch;
        unsigned int fraction = state->position << 16;
        unsigned int whole = 0;
        state->interpolate(count, in, out, (int*)&whole, &fraction, pitch >> 16, (pitch & 0xffff) << 16);
        state->position = (whole << 16) | (fraction >> 16);
        float* const last = in + ((state->position >> 16) - state->lead);
        state->history[0] = last[0];
        state->history[1] = last[1];
        state->history[2] = last[2];
        state->history[3] = last[3];
        state->position &= 0xffffLL;
        return count;
    }
}
