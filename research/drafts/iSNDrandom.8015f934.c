// The sound library's random generator state: six words seeded by adding a
// value to each original word (iSNDrandom steps it with add-with-carry).
unsigned int SNDIrandseedorig[6] = {0xf22d0e56, 0x883126e9, 0xc624dd2f, 0x0702c49c, 0x9e353f7d, 0x6fdf3b64};
unsigned int SNDIrandseed[6];

void SNDI_randomseed(unsigned int seed) {
    SNDIrandseed[0] = seed + SNDIrandseedorig[0];
    SNDIrandseed[1] = seed + SNDIrandseedorig[1];
    SNDIrandseed[2] = seed + SNDIrandseedorig[2];
    SNDIrandseed[3] = seed + SNDIrandseedorig[3];
    SNDIrandseed[4] = seed + SNDIrandseedorig[4];
    SNDIrandseed[5] = seed + SNDIrandseedorig[5];
}

static inline unsigned char addc2(unsigned int* dst, unsigned int b, unsigned char c) {
    unsigned int const a = *dst;
    unsigned int sum = a;
    sum += c;
    sum += b;
    unsigned char carry;
    carry = 1;
    if (!(sum < a)) carry = 0;
    *dst = sum;
    return carry;
}
static inline unsigned char addc(unsigned int* dst, unsigned int* src, unsigned int c) {
    unsigned int a = *dst;
    unsigned int b = *src;
    unsigned int sum = a;
    sum += c;
    sum = b + sum;
    unsigned char carry = 0;
    if (sum < b || sum < a) carry = 1;
    *dst = sum;
    return carry;
}
int iSNDrandom(void) {
    unsigned short carry = 0;
    carry = addc(&SNDIrandseed[4], &SNDIrandseed[5], carry);
    carry = addc2(&SNDIrandseed[3], SNDIrandseed[4], carry);
    carry = addc2(&SNDIrandseed[2], SNDIrandseed[3], carry);
    carry = addc2(&SNDIrandseed[1], SNDIrandseed[2], carry);
    addc2(&SNDIrandseed[0], SNDIrandseed[1], carry);

    if (++SNDIrandseed[5] != 0)
        return SNDIrandseed[0];
    if (++SNDIrandseed[4] != 0)
        return SNDIrandseed[0];
    if (++SNDIrandseed[3] != 0)
        return SNDIrandseed[0];
    if (++SNDIrandseed[2] != 0)
        return SNDIrandseed[0];
    if (++SNDIrandseed[1] != 0)
        return SNDIrandseed[0];
    return ++SNDIrandseed[0];
}
