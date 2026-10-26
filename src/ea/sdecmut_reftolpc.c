/* A fragment of EA's speech decoder (sdecmut.c, 0x80162584): the
   file-local reftolpc converts twelve reflection coefficients into
   linear-prediction coefficients through a lattice (the state array starts
   at 1.0 with the coefficients behind it) and the recursion on the earlier
   outputs; the file-local filter runs the 12-pole synthesis filter over
   blocks of 12 samples in place (each output also enters the circular
   history, written out term by term); initmut starts the MicroTalk bit
   reader on a stream (the first byte loaded, the first bit skipped), reads
   a 4-bit shift, a 4-bit scale and a 6-bit growth step, fills a 64-entry
   geometric table from them and clears the filter and sample history.
   Compiled as C++ like the rest of the sound library, with inlining of
   functions marked inline only. The functions, UTALKSTATE and bitmask are
   named by the symbols; the state's members, the bit-reader macro and the
   inline getbits helper are inferred (names not original). The rest of the
   file is not part of this unit. */
struct UTALKSTATE {
    unsigned char* ptr;
    unsigned int bits;
    int count;
    int shift;
    float table[64];
    float a[12];
    float b[12];
    float c[324];
    float samples[432];
};

extern unsigned int bitmask[];

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

static void filter(UTALKSTATE* s, int offset, int blocks) {
    float lpc[12];
    float* out = &s->samples[offset];
    int i;

    reftolpc(s->a, lpc);
    for (i = 0; i < blocks; i++) {
        s->b[11] = out[0] + lpc[0] * s->b[0] + lpc[1] * s->b[1] + lpc[2] * s->b[2] + lpc[3] * s->b[3] + lpc[4] * s->b[4] + lpc[5] * s->b[5] + lpc[6] * s->b[6] + lpc[7] * s->b[7] + lpc[8] * s->b[8] + lpc[9] * s->b[9] + lpc[10] * s->b[10] + lpc[11] * s->b[11];
        out[0] = s->b[11];
        s->b[10] = out[1] + lpc[0] * s->b[11] + lpc[1] * s->b[0] + lpc[2] * s->b[1] + lpc[3] * s->b[2] + lpc[4] * s->b[3] + lpc[5] * s->b[4] + lpc[6] * s->b[5] + lpc[7] * s->b[6] + lpc[8] * s->b[7] + lpc[9] * s->b[8] + lpc[10] * s->b[9] + lpc[11] * s->b[10];
        out[1] = s->b[10];
        s->b[9] = out[2] + lpc[0] * s->b[10] + lpc[1] * s->b[11] + lpc[2] * s->b[0] + lpc[3] * s->b[1] + lpc[4] * s->b[2] + lpc[5] * s->b[3] + lpc[6] * s->b[4] + lpc[7] * s->b[5] + lpc[8] * s->b[6] + lpc[9] * s->b[7] + lpc[10] * s->b[8] + lpc[11] * s->b[9];
        out[2] = s->b[9];
        s->b[8] = out[3] + lpc[0] * s->b[9] + lpc[1] * s->b[10] + lpc[2] * s->b[11] + lpc[3] * s->b[0] + lpc[4] * s->b[1] + lpc[5] * s->b[2] + lpc[6] * s->b[3] + lpc[7] * s->b[4] + lpc[8] * s->b[5] + lpc[9] * s->b[6] + lpc[10] * s->b[7] + lpc[11] * s->b[8];
        out[3] = s->b[8];
        s->b[7] = out[4] + lpc[0] * s->b[8] + lpc[1] * s->b[9] + lpc[2] * s->b[10] + lpc[3] * s->b[11] + lpc[4] * s->b[0] + lpc[5] * s->b[1] + lpc[6] * s->b[2] + lpc[7] * s->b[3] + lpc[8] * s->b[4] + lpc[9] * s->b[5] + lpc[10] * s->b[6] + lpc[11] * s->b[7];
        out[4] = s->b[7];
        s->b[6] = out[5] + lpc[0] * s->b[7] + lpc[1] * s->b[8] + lpc[2] * s->b[9] + lpc[3] * s->b[10] + lpc[4] * s->b[11] + lpc[5] * s->b[0] + lpc[6] * s->b[1] + lpc[7] * s->b[2] + lpc[8] * s->b[3] + lpc[9] * s->b[4] + lpc[10] * s->b[5] + lpc[11] * s->b[6];
        out[5] = s->b[6];
        s->b[5] = out[6] + lpc[0] * s->b[6] + lpc[1] * s->b[7] + lpc[2] * s->b[8] + lpc[3] * s->b[9] + lpc[4] * s->b[10] + lpc[5] * s->b[11] + lpc[6] * s->b[0] + lpc[7] * s->b[1] + lpc[8] * s->b[2] + lpc[9] * s->b[3] + lpc[10] * s->b[4] + lpc[11] * s->b[5];
        out[6] = s->b[5];
        s->b[4] = out[7] + lpc[0] * s->b[5] + lpc[1] * s->b[6] + lpc[2] * s->b[7] + lpc[3] * s->b[8] + lpc[4] * s->b[9] + lpc[5] * s->b[10] + lpc[6] * s->b[11] + lpc[7] * s->b[0] + lpc[8] * s->b[1] + lpc[9] * s->b[2] + lpc[10] * s->b[3] + lpc[11] * s->b[4];
        out[7] = s->b[4];
        s->b[3] = out[8] + lpc[0] * s->b[4] + lpc[1] * s->b[5] + lpc[2] * s->b[6] + lpc[3] * s->b[7] + lpc[4] * s->b[8] + lpc[5] * s->b[9] + lpc[6] * s->b[10] + lpc[7] * s->b[11] + lpc[8] * s->b[0] + lpc[9] * s->b[1] + lpc[10] * s->b[2] + lpc[11] * s->b[3];
        out[8] = s->b[3];
        s->b[2] = out[9] + lpc[0] * s->b[3] + lpc[1] * s->b[4] + lpc[2] * s->b[5] + lpc[3] * s->b[6] + lpc[4] * s->b[7] + lpc[5] * s->b[8] + lpc[6] * s->b[9] + lpc[7] * s->b[10] + lpc[8] * s->b[11] + lpc[9] * s->b[0] + lpc[10] * s->b[1] + lpc[11] * s->b[2];
        out[9] = s->b[2];
        s->b[1] = out[10] + lpc[0] * s->b[2] + lpc[1] * s->b[3] + lpc[2] * s->b[4] + lpc[3] * s->b[5] + lpc[4] * s->b[6] + lpc[5] * s->b[7] + lpc[6] * s->b[8] + lpc[7] * s->b[9] + lpc[8] * s->b[10] + lpc[9] * s->b[11] + lpc[10] * s->b[0] + lpc[11] * s->b[1];
        out[10] = s->b[1];
        s->b[0] = out[11] + lpc[0] * s->b[1] + lpc[1] * s->b[2] + lpc[2] * s->b[3] + lpc[3] * s->b[4] + lpc[4] * s->b[5] + lpc[5] * s->b[6] + lpc[6] * s->b[7] + lpc[7] * s->b[8] + lpc[8] * s->b[9] + lpc[9] * s->b[10] + lpc[10] * s->b[11] + lpc[11] * s->b[0];
        out[11] = s->b[0];
        out += 12;
    }
}

#define SKIPBITS(s, n)                         \
    (s)->bits >>= (n);                         \
    (s)->count -= (n);                         \
    if ((s)->count < 8) {                      \
        (s)->bits |= *(s)->ptr << (s)->count;  \
        (s)->ptr++;                            \
        (s)->count += 8;                       \
    }

static inline unsigned int getbits(UTALKSTATE* s, int n) {
    unsigned int v = s->bits & bitmask[n];
    SKIPBITS(s, n)
    return v;
}

void initmut(char* data, UTALKSTATE* s) {
    unsigned int v;
    float ratio;
    int i;

    s->bits = *(unsigned char*)data;
    s->count = 8;
    s->ptr = (unsigned char*)data + 1;
    SKIPBITS(s, 1)
    v = getbits(s, 4);
    s->shift = 32 - v;
    v = getbits(s, 4);
    s->table[0] = 8.0f * (float)(v + 1);
    v = getbits(s, 6);
    ratio = 1.04f + 0.001f * (float)v;
    for (i = 1; i < 64; i++)
        s->table[i] = s->table[i - 1] * ratio;
    for (i = 0; i < 12; i++) {
        s->a[i] = 0.0f;
        s->b[i] = 0.0f;
    }
    for (i = 0; i < 324; i++)
        s->c[i] = 0.0f;
}
