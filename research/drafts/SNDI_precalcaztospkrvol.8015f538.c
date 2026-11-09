/* SNDI_aztospkrvol, the last function of the azimuth-to-speaker file: it
   looks up the precalculated per-speaker levels for an azimuth (its high
   byte selects one of 256 rows) and scales each into a 16-bit volume for
   every output speaker. sndaztospkr and sndgs are named by their symbols;
   the table's row layout and the view of sndgs (the speaker count at +72)
   are inferred. */
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

void SNDI_precalcaztospkrvol() {
    int i;
    int azimuth = 0;

    for (i = 0; i < 256; i++) {
        if (sndgs.speakers == 1) {
            sndaztospkr[i][0] = 127;
        } else {
            int k;
            unsigned short* az;

            for (k = 0; k < sndgs.speakers; k++)
                sndaztospkr[i][k] = 0;
            az = sndgs.azimuths[sndgs.speakers - 1];
            if (azimuth <= az[0]) {
                int before = az[0] - azimuth;
                int after = azimuth + (0x10000 - az[sndgs.speakers - 1]);
                int a = after * 127 / (before + after);
                int b = before * 127 / (before + after);

                sndaztospkr[i][0] = shape(a);
                sndaztospkr[i][sndgs.speakers - 1] = shape(b);
            } else if (azimuth >= az[sndgs.speakers - 1]) {
                int before = azimuth - az[sndgs.speakers - 1];
                int after = 0x10000 - azimuth + az[0];
                int a = before * 127 / (after + before);
                int b = after * 127 / (after + before);

                sndaztospkr[i][0] = shape(a);
                sndaztospkr[i][sndgs.speakers - 1] = shape(b);
            } else {
                signed char* level = sndaztospkr[i];

                for (k = 0; k < sndgs.speakers; k++) {
                    if (azimuth >= az[0] && azimuth <= az[1]) {
                        int after = az[1] - azimuth;
                        int before = azimuth - az[0];
                        int const a = after * 127 / (before + after);
                        int b = before * 127 / (before + after);

                        level[0] = shape(a);
                        level[1] = shape(b);
                    }
                    az++;
                    level++;
                }
            }
        }
        azimuth += 256;
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
