/* EA's SNDI_memcpy (smemcpy.c, the whole file): copies n bytes choosing the
   widest access the source/destination distance allows - bytes (four at a
   time, then the rest) when it is odd, halfwords (four at a time, then
   singly) when it is 2 modulo 4 and words (four at a time, then singly) when
   it is a multiple of 4, after aligning the source and before copying the
   tail. The source is inferred from the code: each group loads into int
   temporaries before storing, and the counts are the decremented length. */
extern "C" void SNDI_memcpy(void* dst, const void* src, int n) {
    unsigned char* d = (unsigned char*)dst;
    const unsigned char* s = (const unsigned char*)src;
    int diff;

    diff = s - d;
    if (n == 0 || diff == 0)
        return;
    if (diff & 1) {
        while (n >= 4) {
            int a = s[0];
            int b = s[1];
            int c = s[2];
            int e = s[3];
            d[0] = a;
            d[1] = b;
            d[2] = c;
            d[3] = e;
            d += 4;
            s += 4;
            n -= 4;
        }
        while (n > 0) {
            *d++ = *s++;
            n--;
        }
    } else if (diff & 2) {
        if (((unsigned int)s & 1) && n >= 1) {
            unsigned char c = *s++;
            n--;
            *d++ = c;
        }
        while (n >= 8) {
            int a = ((const short*)s)[0];
            int b = ((const short*)s)[1];
            int c = ((const short*)s)[2];
            int e = ((const short*)s)[3];
            ((short*)d)[0] = a;
            ((short*)d)[1] = b;
            ((short*)d)[2] = c;
            ((short*)d)[3] = e;
            d += 8;
            s += 8;
            n -= 8;
        }
        while (n >= 2) {
            *(short*)d = *(const short*)s;
            d += 2;
            s += 2;
            n -= 2;
        }
        if (n >= 1)
            *d = *s;
    } else {
        if (((unsigned int)s & 1) && n >= 1) {
            unsigned char c = *s++;
            n--;
            *d++ = c;
        }
        if (((unsigned int)s & 2) && n >= 2) {
            *(short*)d = *(const short*)s;
            d += 2;
            s += 2;
            n -= 2;
        }
        while (n >= 16) {
            int a = ((const int*)s)[0];
            int b = ((const int*)s)[1];
            int c = ((const int*)s)[2];
            int e = ((const int*)s)[3];
            ((int*)d)[0] = a;
            ((int*)d)[1] = b;
            ((int*)d)[2] = c;
            ((int*)d)[3] = e;
            d += 16;
            s += 16;
            n -= 16;
        }
        while (n >= 4) {
            *(int*)d = *(const int*)s;
            d += 4;
            s += 4;
            n -= 4;
        }
        if (n >= 2) {
            *(short*)d = *(const short*)s;
            d += 2;
            s += 2;
            n -= 2;
        }
        if (n >= 1)
            *d = *s;
    }
}
