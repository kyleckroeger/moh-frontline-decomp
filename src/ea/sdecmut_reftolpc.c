/* A fragment of EA's speech decoder (sdecmut.c, 0x80162584): the
   file-local reftolpc converts twelve reflection coefficients into
   linear-prediction coefficients through a lattice (the state array starts
   at 1.0 with the coefficients behind it) and the recursion on the earlier
   outputs. Compiled as C++ like the rest of the sound library. The name
   comes from the symbol; the arrays' roles are inferred. The rest of the
   file is not part of this unit. */
static void reftolpc(float* k, float* a) {
    float t[12];
    float h[12];
    float x;
    int i;
    int j;

    for (j = 10; j >= 0; j--)
        t[j + 1] = k[j];
    t[0] = 1.0f;
    for (i = 0; i < 12; i++) {
        x = -k[11] * t[11];
        for (j = 10; j >= 0; j--) {
            x -= k[j] * t[j];
            t[j + 1] = x * k[j] + t[j];
        }
        t[0] = x;
        h[i] = x;
        for (j = 0; j < i; j++)
            x -= a[j] * h[i - j - 1];
        a[i] = x;
    }
}
