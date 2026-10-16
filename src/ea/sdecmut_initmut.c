/* A fragment of EA's speech decoder (sdecmut.c, 0x801631f4): initmut
   starts the MicroTalk bit reader on a stream (the first byte loaded, the
   first bit skipped), reads a 4-bit shift, a 4-bit scale and a 6-bit growth
   step, fills a 64-entry geometric table from them and clears the filter and
   sample history. Compiled as C++ like the rest of the sound library. The
   function, UTALKSTATE and bitmask are named by the symbols; the state's
   members, the bit-reader macro and the inline getbits helper are inferred
   (names not original). The rest of the file is not part of this unit. */
struct UTALKSTATE {
    unsigned char* ptr;
    unsigned int bits;
    int count;
    int shift;
    float table[64];
    float a[12];
    float b[12];
    float c[324];
};

extern unsigned int bitmask[];

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
