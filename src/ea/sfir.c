// FIR coefficient design for the sound filters: a windowed-sinc low-pass,
// high-pass or band-pass response for the 8-tap symmetric filter.
struct SNDFIRSTATE {
    float history[8];
    float coeffs[5];
    float low;
    float high;
};

float SNDI_sin(float);
float SNDI_cos(float);

void calcFIRCoeffs(SNDFIRSTATE* state, int type) {
    int i;
    float sum;
    float x;
    float gain;

    if ((type == 3 || type == 4) && state->low > 0.8f)
        state->low = 0.8f;
    switch (type) {
    case 2:
        state->coeffs[4] = state->high;
        for (i = 1; i <= 4; i++) {
            x = 3.1415927f * (float)i;
            state->coeffs[4 - i] = SNDI_sin(state->high * x) / x;
        }
        break;
    case 3:
        state->coeffs[4] = state->low;
        for (i = 1; i <= 4; i++) {
            x = 3.1415927f * (float)i;
            state->coeffs[4 - i] = SNDI_sin(state->low * x) / x;
        }
        break;
    case 4:
        state->coeffs[4] = state->high - state->low;
        for (i = 1; i <= 4; i++) {
            x = 3.1415927f * (float)i;
            state->coeffs[4 - i] = 2.0f * SNDI_sin(x * (0.5f * (state->high - state->low))) * SNDI_cos(x * (0.5f * (state->high + state->low))) / x;
        }
        break;
    }
    sum = 0.0f;
    for (i = 0; i <= 4; i++) {
        if (type == 2 || type == 4) {
            state->coeffs[i] *= 0.54f - 0.46f * SNDI_cos(0.7853982f * (float)i);
            sum += state->coeffs[i];
        } else if (type == 3) {
            state->coeffs[i] *= -(0.54f - 0.46f * SNDI_cos(0.7853982f * (float)i));
            if (i % 2 == 0)
                sum += state->coeffs[i];
            else
                sum -= state->coeffs[i];
        }
    }
    if (type == 3) {
        state->coeffs[4] += 1.0f;
        sum += 1.0f;
    }
    gain = 2.0f * sum - state->coeffs[4];
    if (gain < 0.0f)
        gain *= -1.0f;
    state->coeffs[0] /= gain;
    state->coeffs[1] /= gain;
    state->coeffs[2] /= gain;
    state->coeffs[3] /= gain;
    state->coeffs[4] /= gain;
}
