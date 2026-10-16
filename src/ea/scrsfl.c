/* EA's linear-interpolation resampling loop (scrsfl.c, the whole file):
   rsflc fills count output samples from the input at a 16.16 fixed-point
   position (whole samples in *index, the fraction kept in the high half of
   *fraction between calls) advancing by a 16.16 step built from two
   arguments, interpolating each output between two neighbouring inputs.
   The name comes from the symbol; the parameter roles are inferred.
   Compiled as C++ like the rest of the sound library. */
extern "C" void rsflc(int count, float* in, float* out, int* index, unsigned int* fraction, int stephigh,
                      unsigned int steplow) {
    unsigned int step = stephigh << 16 | steplow >> 16;
    int i;

    *fraction >>= 16;
    for (i = 0; i < count; i++) {
        float* sample = &in[*index];

        out[i] = sample[0] + 1.5258e-05f * (float)*fraction * (sample[1] - sample[0]);
        *fraction += step;
        *index += *fraction >> 16;
        *fraction &= 0xFFFF;
    }
    *fraction <<= 16;
}
