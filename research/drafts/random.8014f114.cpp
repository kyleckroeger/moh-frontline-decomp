unsigned long seed[6] = { 0xf22d0e56, 0x883126e9, 0xc624dd2f, 0x0702c49c, 0x9e353f7d, 0x6fdf3b64 };

// g2 (11-13): the add-with-carry steps as small inline helpers (inferred names addc/addc2/addlast, the
// iSNDrandom draft's form of the same generator); 82 -> 79. Left: the first carry-in 0 is not folded in the
// original (addi r7,r4,0) and the final increment keeps two copies (addi r0/r3).
static inline unsigned char addc2(unsigned long* dst, unsigned long b, unsigned char c) {
    unsigned long const a = *dst;
    unsigned long sum = a;
    sum += c;
    sum = b + sum;
    unsigned char carry;
    carry = 1;
    if (!(sum < a)) carry = 0;
    *dst = sum;
    return carry;
}
static inline unsigned char addc(unsigned long* dst, unsigned long* src, unsigned int c) {
    unsigned long a = *dst;
    unsigned long b = *src;
    unsigned long sum = a;
    sum += c;
    sum = b + sum;
    unsigned char carry = 0;
    if (sum < b || sum < a) carry = 1;
    *dst = sum;
    return carry;
}
static inline unsigned long addlast(unsigned long* dst, unsigned long b, unsigned char c) {
    unsigned long sum = *dst;
    sum += c;
    sum = b + sum;
    *dst = sum;
    return sum;
}
extern "C" long random(void) {
    unsigned short carry = 0;
    carry = addc(&seed[4], &seed[5], carry);
    carry = addc2(&seed[3], seed[4], carry);
    carry = addc2(&seed[2], seed[3], carry);
    carry = addc2(&seed[1], seed[2], carry);
    long result = addlast(&seed[0], seed[1], carry);

    if (++seed[5] != 0)
        return result;
    if (++seed[4] != 0)
        return result;
    if (++seed[3] != 0)
        return result;
    if (++seed[2] != 0)
        return result;
    if (++seed[1] != 0)
        return result;
    ++seed[0];
    ++result;
    return result;
}
