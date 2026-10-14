// A fragment of the software mixer (smixer.c, 0x8015fd30): the 16-sample
// level ramps. MIXI_interpolateto0 fades a 16-sample block from the given
// level to zero (sample i gets (16 - i) / 17 of the level) and clears the
// level; MIXI_interpolatemix mixes a 16-sample block into another while the
// level moves from one value toward another in 17 steps (sample i gets
// from + (i + 1) * (to - from) / 17). Each step is written out, as the
// separate constants of the original show. The functions are named by their
// symbols; the parameter names are inferred. The rest of the file is not part
// of this unit.
void MIXI_interpolateto0(float* level, float* buffer) {
    buffer[0] += (16.0f / 17.0f) * *level;
    buffer[1] += (15.0f / 17.0f) * *level;
    buffer[2] += (14.0f / 17.0f) * *level;
    buffer[3] += (13.0f / 17.0f) * *level;
    buffer[4] += (12.0f / 17.0f) * *level;
    buffer[5] += (11.0f / 17.0f) * *level;
    buffer[6] += (10.0f / 17.0f) * *level;
    buffer[7] += (9.0f / 17.0f) * *level;
    buffer[8] += (8.0f / 17.0f) * *level;
    buffer[9] += (7.0f / 17.0f) * *level;
    buffer[10] += (6.0f / 17.0f) * *level;
    buffer[11] += (5.0f / 17.0f) * *level;
    buffer[12] += (4.0f / 17.0f) * *level;
    buffer[13] += (3.0f / 17.0f) * *level;
    buffer[14] += (2.0f / 17.0f) * *level;
    buffer[15] += (1.0f / 17.0f) * *level;
    *level = 0.0f;
}

void MIXI_interpolatemix(float from, float to, float* src, float* dst) {
    float step = (1.0f / 17.0f) * (to - from);
    dst[0] += src[0] * (from + step);
    dst[1] += src[1] * (from + 2.0f * step);
    dst[2] += src[2] * (from + 3.0f * step);
    dst[3] += src[3] * (from + 4.0f * step);
    dst[4] += src[4] * (from + 5.0f * step);
    dst[5] += src[5] * (from + 6.0f * step);
    dst[6] += src[6] * (from + 7.0f * step);
    dst[7] += src[7] * (from + 8.0f * step);
    dst[8] += src[8] * (from + 9.0f * step);
    dst[9] += src[9] * (from + 10.0f * step);
    dst[10] += src[10] * (from + 11.0f * step);
    dst[11] += src[11] * (from + 12.0f * step);
    dst[12] += src[12] * (from + 13.0f * step);
    dst[13] += src[13] * (from + 14.0f * step);
    dst[14] += src[14] * (from + 15.0f * step);
    dst[15] += src[15] * (from + 16.0f * step);
}
