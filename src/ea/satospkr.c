/* azimuth-to-speaker volume tables. SNDI_precalcaztospkrvol fills sndaztospkr: for each of 256
   azimuths (azimuth = i * 256 in 16-bit turns) it pans between the two speakers whose configured
   azimuths (sndgs.azimuths[speakers - 1], ascending) bracket it, wrapping between the last and the
   first speaker; shape() bends the linear 0..127 share into the level curve. SNDI_aztospkrvol looks
   a row up and scales it to 16-bit volumes. sndaztospkr and sndgs are named by their symbols; the
   view of sndgs (speaker count at +72, azimuth table at +120) is inferred.
   Matching notes: the first wrap branch goes through the single-use pan() helper (arguments are
   evaluated right to left); the second is written out with a named sum s declared after d and e,
   and the third branch's quotients a and b are function-scope locals, so that s is coloured after
   them (r30) as in the original. */
struct SNDGSVIEW {
    unsigned char unknown000[72];
    unsigned char speakers;
    unsigned char unknown049[47];
    unsigned short azimuths[4][4];
    unsigned char unknown098[416];
};

extern signed char sndaztospkr[256][4];
extern SNDGSVIEW sndgs;

static inline signed char shape(int v) {
    return v + (v - v * v / 127);
}

static inline void pan(signed char* p, signed char* q, int x, int y) {
    *p = shape(x * 127 / (y + x));
    *q = shape(y * 127 / (y + x));
}

void SNDI_precalcaztospkrvol() {
    int a, b;
    int i;

    for (i = 0; i < 256; i++) {
        int azimuth = i * 256;
        if (sndgs.speakers == 1) {
            sndaztospkr[i][0] = 127;
        } else {
            int k;
            unsigned short* az;

            for (k = 0; k < sndgs.speakers; k++)
                sndaztospkr[i][k] = 0;
            az = sndgs.azimuths[sndgs.speakers - 1];
            if (azimuth <= az[0]) {
                pan(&sndaztospkr[i][0], &sndaztospkr[i][sndgs.speakers - 1], azimuth + (0x10000 - az[sndgs.speakers - 1]), az[0] - azimuth);
            } else if (azimuth >= az[sndgs.speakers - 1]) {
                int e = 0x10000 - azimuth + az[0];
                int d = azimuth - az[sndgs.speakers - 1];
                int s = e + d;

                sndaztospkr[i][0] = shape(d * 127 / s);
                sndaztospkr[i][sndgs.speakers - 1] = shape(e * 127 / s);
            } else {
                for (k = 0; k < sndgs.speakers; k++) {
                    if (azimuth >= az[k] && azimuth <= az[k + 1]) {
                        int before = azimuth - az[k];
                        int after = az[k + 1] - azimuth;
                        a = after * 127 / (before + after);
                        b = before * 127 / (before + after);

                        sndaztospkr[i][k] = shape(a);
                        sndaztospkr[i][k + 1] = shape(b);
                    }
                }
            }
        }
    }
}

void SNDI_aztospkrvol(int azimuth, short* volumes) {
    signed char* levels = sndaztospkr[(azimuth >> 8) & 0xFF];
    int i = 0;
    while (i < sndgs.speakers) {
        int level = *levels++;
        i++;
        *volumes++ = level * 258;
    }
}
