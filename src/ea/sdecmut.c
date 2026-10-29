/* EA's speech decoder (sdecmut.c, 0x801620fc), the whole file: readsamples
   reads one subframe's excitation into every other sample, either as 54
   two-level codes (0, -2 or +2 from one or two bits) or, for the Huffman
   models, as codes looked up from the next eight bits in the current
   model's 256-entry index table: a code gives the next model, its length and
   (codes 4 and up) a value; codes 2 and 3 are runs of 7 to 70 zeros, codes 0
   and 1 a unary magnitude from 7 with a sign bit. The file-local reftolpc
   converts twelve reflection coefficients into
   linear-prediction coefficients through a lattice (the state array starts
   at 1.0 with the coefficients behind it) and the recursion on the earlier
   outputs; the file-local filter runs the 12-pole synthesis filter over
   blocks of 12 samples in place (each output also enters the circular
   history, written out term by term); initmut starts the MicroTalk bit
   reader on a stream (the first byte loaded, the first bit skipped), reads
   a 4-bit shift, a 4-bit scale and a 6-bit growth step, fills a 64-entry
   geometric table from them and clears the filter and sample history.
   decodemut decodes one MicroTalk frame: twelve reflection-coefficient
   deltas (a quarter of the step to the table value), then four subframes of
   108 samples (pitch lag, pitch gain in 1/15 steps, excitation gain, grid
   alignment and a zero flag; readsamples fills every other excitation
   sample, the rest are zeroed or interpolated by a 3-tap filter), each the
   pitch-predicted history plus the excitation; then the history moves down
   and the filter runs four times as the coefficients step to their targets.
   Compiled as C++ like the rest of the sound library, with GC 1.3 (1.3.2
   hoists readsamples' table addresses) and inlining of functions marked
   inline only. The functions, UTALKSTATE and the file's tables (bitmask,
   coeff_table, index_table, decode_table, defined here with the original
   values) are named by the symbols; the state's members, the code record's
   members, the bit-reader macro, the inline getbits helper and the locals
   are inferred (names not original). */
struct UTALKSTATE {
    unsigned char* ptr;
    unsigned int bits;
    int count;
    int shift;
    float table[64];
    float a[12];
    float b[12];
    float c[324];       /* history; the pitch lag reads on into samples */
    float samples[432];
};

struct UTALKCODE {
    int next;
    int length;
    float value;
};

static unsigned int bitmask[9] = {0, 1, 3, 7, 0xf, 0x1f, 0x3f, 0x7f, 0xff};

static float coeff_table[64] = {
    0.0f, -0.996775985f, -0.990327001f, -0.98387903f,
    -0.977430999f, -0.970982015f, -0.964533985f, -0.958085001f,
    -0.95163703f, -0.930754006f, -0.904959977f, -0.87916702f,
    -0.853372991f, -0.827579021f, -0.801786005f, -0.775991976f,
    -0.750198007f, -0.724404991f, -0.698611021f, -0.670634985f,
    -0.619047999f, -0.567460001f, -0.515873015f, -0.464286f,
    -0.412698001f, -0.361110985f, -0.309524f, -0.257937014f,
    -0.206349f, -0.154762f, -0.103174999f, -0.0515870005f,
    0.0f, 0.0515870005f, 0.103174999f, 0.154762f,
    0.206349f, 0.257937014f, 0.309524f, 0.361110985f,
    0.412698001f, 0.464286f, 0.515873015f, 0.567460001f,
    0.619047999f, 0.670634985f, 0.698611021f, 0.724404991f,
    0.750198007f, 0.775991976f, 0.801786005f, 0.827579021f,
    0.853372991f, 0.87916702f, 0.904959977f, 0.930754006f,
    0.95163703f, 0.958085001f, 0.964533985f, 0.970982015f,
    0.977430999f, 0.98387903f, 0.990327001f, 0.996775985f,
};

static unsigned char index_table[2][256] = {
    4, 6, 5, 9, 4, 6, 5, 13, 4, 6, 5, 10, 4, 6, 5, 17,
    4, 6, 5, 9, 4, 6, 5, 14, 4, 6, 5, 10, 4, 6, 5, 21,
    4, 6, 5, 9, 4, 6, 5, 13, 4, 6, 5, 10, 4, 6, 5, 18,
    4, 6, 5, 9, 4, 6, 5, 14, 4, 6, 5, 10, 4, 6, 5, 25,
    4, 6, 5, 9, 4, 6, 5, 13, 4, 6, 5, 10, 4, 6, 5, 17,
    4, 6, 5, 9, 4, 6, 5, 14, 4, 6, 5, 10, 4, 6, 5, 22,
    4, 6, 5, 9, 4, 6, 5, 13, 4, 6, 5, 10, 4, 6, 5, 18,
    4, 6, 5, 9, 4, 6, 5, 14, 4, 6, 5, 10, 4, 6, 5, 0,
    4, 6, 5, 9, 4, 6, 5, 13, 4, 6, 5, 10, 4, 6, 5, 17,
    4, 6, 5, 9, 4, 6, 5, 14, 4, 6, 5, 10, 4, 6, 5, 21,
    4, 6, 5, 9, 4, 6, 5, 13, 4, 6, 5, 10, 4, 6, 5, 18,
    4, 6, 5, 9, 4, 6, 5, 14, 4, 6, 5, 10, 4, 6, 5, 26,
    4, 6, 5, 9, 4, 6, 5, 13, 4, 6, 5, 10, 4, 6, 5, 17,
    4, 6, 5, 9, 4, 6, 5, 14, 4, 6, 5, 10, 4, 6, 5, 22,
    4, 6, 5, 9, 4, 6, 5, 13, 4, 6, 5, 10, 4, 6, 5, 18,
    4, 6, 5, 9, 4, 6, 5, 14, 4, 6, 5, 10, 4, 6, 5, 2,
    4, 11, 7, 15, 4, 12, 8, 19, 4, 11, 7, 16, 4, 12, 8, 23,
    4, 11, 7, 15, 4, 12, 8, 20, 4, 11, 7, 16, 4, 12, 8, 27,
    4, 11, 7, 15, 4, 12, 8, 19, 4, 11, 7, 16, 4, 12, 8, 24,
    4, 11, 7, 15, 4, 12, 8, 20, 4, 11, 7, 16, 4, 12, 8, 1,
    4, 11, 7, 15, 4, 12, 8, 19, 4, 11, 7, 16, 4, 12, 8, 23,
    4, 11, 7, 15, 4, 12, 8, 20, 4, 11, 7, 16, 4, 12, 8, 28,
    4, 11, 7, 15, 4, 12, 8, 19, 4, 11, 7, 16, 4, 12, 8, 24,
    4, 11, 7, 15, 4, 12, 8, 20, 4, 11, 7, 16, 4, 12, 8, 3,
    4, 11, 7, 15, 4, 12, 8, 19, 4, 11, 7, 16, 4, 12, 8, 23,
    4, 11, 7, 15, 4, 12, 8, 20, 4, 11, 7, 16, 4, 12, 8, 27,
    4, 11, 7, 15, 4, 12, 8, 19, 4, 11, 7, 16, 4, 12, 8, 24,
    4, 11, 7, 15, 4, 12, 8, 20, 4, 11, 7, 16, 4, 12, 8, 1,
    4, 11, 7, 15, 4, 12, 8, 19, 4, 11, 7, 16, 4, 12, 8, 23,
    4, 11, 7, 15, 4, 12, 8, 20, 4, 11, 7, 16, 4, 12, 8, 28,
    4, 11, 7, 15, 4, 12, 8, 19, 4, 11, 7, 16, 4, 12, 8, 24,
    4, 11, 7, 15, 4, 12, 8, 20, 4, 11, 7, 16, 4, 12, 8, 3,
};

static UTALKCODE decode_table[29] = {
    {1, 8, 0.0f},
    {1, 7, 0.0f},
    {0, 8, 0.0f},
    {0, 7, 0.0f},
    {0, 2, 0.0f},
    {0, 2, -1.0f},
    {0, 2, 1.0f},
    {0, 3, -1.0f},
    {0, 3, 1.0f},
    {1, 4, -2.0f},
    {1, 4, 2.0f},
    {1, 3, -2.0f},
    {1, 3, 2.0f},
    {1, 5, -3.0f},
    {1, 5, 3.0f},
    {1, 4, -3.0f},
    {1, 4, 3.0f},
    {1, 6, -4.0f},
    {1, 6, 4.0f},
    {1, 5, -4.0f},
    {1, 5, 4.0f},
    {1, 7, -5.0f},
    {1, 7, 5.0f},
    {1, 6, -5.0f},
    {1, 6, 5.0f},
    {1, 8, -6.0f},
    {1, 8, 6.0f},
    {1, 7, -6.0f},
    {1, 7, 6.0f},
};

extern "C" {
void SNDI_memcpy(void*, const void*, int);
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

void readsamples(UTALKSTATE* s, int huffman, float* out) {
    int model;
    UTALKCODE* entry;
    int code;
    int length;
    int count;
    int i;
    int j;
    int n;

    if (huffman) {
        model = 0;
        i = 0;
        do {
            code = index_table[model][s->bits & 0xff];
            entry = &decode_table[code];
            length = entry->length;
            model = entry->next;
            SKIPBITS(s, length)
            if (code > 3) {
                out[i] = entry->value;
                i += 2;
            } else if (code > 1) {
                count = getbits(s, 6) + 7;
                if (i + count * 2 > 108)
                    count = (108 - i) >> 1;
                for (j = 0; j < count; j++) {
                    out[i] = 0.0f;
                    i += 2;
                }
            } else {
                n = 7;
                while (getbits(s, 1) == 1)
                    n++;
                if (getbits(s, 1) == 1)
                    out[i] = (float)n;
                else
                    out[i] = (float)-n;
                i += 2;
            }
        } while (i < 108);
    } else {
        for (i = 54; i; i--) {
            if ((int)(s->bits & 3) == 1) {
                *out = -2.0f;
                SKIPBITS(s, 2)
            } else if ((int)(s->bits & 3) == 3) {
                *out = 2.0f;
                SKIPBITS(s, 2)
            } else {
                *out = 0.0f;
                SKIPBITS(s, 1)
            }
            out += 2;
        }
    }
}

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

void decodemut(UTALKSTATE* s) {
    float e[118];
    int o;
    int align;
    float* q;
    float pitch;
    int multipulse;
    int k;
    int i;
    float gain;
    int j;
    unsigned int v;
    int lag;
    int zero;
    float* p;
    float delta[12];

    v = getbits(s, 6);
    multipulse = (int)v < s->shift;
    delta[0] = 0.25f * (coeff_table[v] - s->a[0]);
    for (k = 1; k < 4; k++)
        delta[k] = 0.25f * (coeff_table[getbits(s, 6)] - s->a[k]);
    for (k = 4; k < 12; k++)
        delta[k] = 0.25f * (coeff_table[16 + getbits(s, 5)] - s->a[k]);

    for (i = 0; i < 4; i++) {
        o = i * 108;
        lag = (i + 2) * 108 - getbits(s, 8);
        q = &e[6];
        pitch = (float)getbits(s, 4) * 0.06666667f;
        gain = s->table[getbits(s, 6)];
        align = getbits(s, 1);
        zero = getbits(s, 1);
        readsamples(s, multipulse, e + 5 + align);
        if (zero) {
            for (j = 0; j < 108; j += 2)
                e[j + 6 - align] = 0.0f;
        } else {
            e[0] = 0.0f;
            e[1] = 0.0f;
            e[2] = 0.0f;
            e[3] = 0.0f;
            e[4] = 0.0f;
            e[113] = 0.0f;
            e[114] = 0.0f;
            e[115] = 0.0f;
            e[116] = 0.0f;
            e[117] = 0.0f;
            p = q - align;
            for (j = 0; j < 108; j += 2)
                p[j] = 0.59738594f * (p[j - 1] + p[j + 1]) + -0.11459156f * (p[j - 3] + p[j + 3]) +
                       0.018032679f * (p[j - 5] + p[j + 5]);
            gain *= 0.5f;
        }
        for (j = 0; j < 108; j++)
            s->samples[o + j] = pitch * s->c[lag + j] + gain * e[5 + j];

    }

    SNDI_memcpy(s->c, &s->samples[108], 324 * sizeof(float));

    for (k = 0; k < 12; k++)
        s->a[k] += delta[k];
    filter(s, 0, 1);
    for (k = 0; k < 12; k++)
        s->a[k] += delta[k];
    filter(s, 12, 1);
    for (k = 0; k < 12; k++)
        s->a[k] += delta[k];
    filter(s, 24, 1);
    for (k = 0; k < 12; k++)
        s->a[k] += delta[k];
    filter(s, 36, 33);
}
