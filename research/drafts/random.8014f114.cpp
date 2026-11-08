unsigned long seed[6] = { 0xf22d0e56, 0x883126e9, 0xc624dd2f, 0x0702c49c, 0x9e353f7d, 0x6fdf3b64 };

extern "C" long random(void) {
    bool carry = false;
    unsigned long sum;

    sum = seed[5] + (seed[4] + carry);
    if (sum < seed[5] || sum < seed[4])
        carry = true;
    seed[4] = sum;

    sum = sum + (seed[3] + carry);
    carry = seed[3] > sum;
    seed[3] = sum;

    sum = sum + (seed[2] + carry);
    carry = seed[2] > sum;
    seed[2] = sum;

    sum = sum + (seed[1] + carry);
    carry = seed[1] > sum;
    seed[1] = sum;

    seed[0] = sum + (seed[0] + carry);

    if (++seed[5] == 0 && ++seed[4] == 0 && ++seed[3] == 0 && ++seed[2] == 0 && ++seed[1] == 0)
        ++seed[0];
    return seed[0];
}
